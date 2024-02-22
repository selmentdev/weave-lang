#include <fmt/format.h>
#include <fmt/chrono.h>


#include <charconv>
#include <cstdlib>
#include <map>
#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/filesystem/FileSystem.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/time/Instant.hxx"
#include "weave/session/CodeGeneratorOptions.hxx"
#include "weave/session/ExperimentalOptions.hxx"
#include "weave/session/EmitOptions.hxx"

#include "weave/threading/CriticalSection.hxx"
#include "weave/threading/Thread.hxx"
#include "weave/threading/Runnable.hxx"
#include "weave/filesystem/FileHandle.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/filesystem/FileWriter.hxx"
#include "weave/profiler/Profiler.hxx"
#include "weave/threading/Yield.hxx"
#include "weave/time/DateTime.hxx"
#include "weave/time/DateTimeOffset.hxx"
#include "weave/bugcheck/Assert.hxx"
#include "weave/filesystem/FileInfo.hxx"
#include "weave/system/Environment.hxx"
#include "weave/syntax/Visitor.hxx"

#include "weave/filesystem/Utilities.hxx"

#include "weave/syntax/Lexer.hxx"
#include "weave/syntax/Parser.hxx"
#include "weave/syntax/Visitor.hxx"

#include <atomic>

#if defined(WIN32)
WEAVE_EXTERNAL_HEADERS_BEGIN
#include <Windows.h>
#include <psapi.h>
WEAVE_EXTERNAL_HEADERS_END
#endif

namespace weave::driver
{
    struct Driver
    {
    public:
        session::CodeGeneratorOptions CodeGenerator;
        session::ExperimentalOptions Experimental;
        session::EmitOptions Emit;

        /// \brief The path to source file or directory.
        std::string SourcePath;

        /// \brief The path to output directory.
        std::string OutputPath;

        /// \brief The path to intermediate directory.
        std::string IntermediatePath;

        /// \brief The name of the module.
        std::string ModuleName;
    };
}

class TreeLinearizer : public weave::syntax::SyntaxWalker
{
private:
    std::vector<weave::syntax::SyntaxKind> _kinds{};

public:
    void OnDefault(weave::syntax::SyntaxNode* node) override
    {
        this->_kinds.push_back(node->Kind);
    }

    std::span<weave::syntax::SyntaxKind const> GetKinds() const
    {
        return this->_kinds;
    }
};

class ErrorReporter final : public weave::syntax::SyntaxWalker
{
public:
    weave::source::DiagnosticSink& Diagnostic;

public:
    explicit ErrorReporter(weave::source::DiagnosticSink& diagnostic)
        : Diagnostic(diagnostic)
    {
    }

    void OnToken(weave::syntax::SyntaxToken* token) override
    {
        if (token->IsMissing())
        {
            this->Diagnostic.AddError(token->Source, fmt::format("Expected '{}'", weave::syntax::GetSpelling(token->Kind)));
        }
    }

    void OnUnexpectedNodesSyntax(weave::syntax::UnexpectedNodesSyntax* node) override
    {
        auto first = static_cast<weave::syntax::SyntaxToken*>(node->Nodes.GetElement(0));
        auto last = static_cast<weave::syntax::SyntaxToken*>(node->Nodes.GetElement(node->Nodes.GetCount() - 1));
        auto source = weave::source::Combine(first->Source, last->Source);
        this->Diagnostic.AddError(source, fmt::format("Unexpected tokens"));
    }
};

class SyntaxTreeStructurePrinter final : public weave::syntax::SyntaxWalker
{
private:
    void Indent()
    {
        fmt::print("{:{}}", "", this->Depth);
    }

    weave::source::SourceText const& _text;

public:
    SyntaxTreeStructurePrinter(weave::source::SourceText const& text)
        : SyntaxWalker{true}
        , _text{text}
    {
    }

public:
    void OnDefault(weave::syntax::SyntaxNode* node) override
    {
        Indent();
        fmt::println("{}", weave::syntax::GetName(node->Kind));

        SyntaxWalker::OnDefault(node);
    }

    void OnToken(weave::syntax::SyntaxToken* token) override
    {
        // this->Dispatch(token->LeadingTrivia.GetNode());
        Indent();
        auto startPosition = this->_text.GetLinePosition(token->Source.Start);
        auto endPosition = this->_text.GetLinePosition(token->Source.End);
        fmt::println("{} <{}:{}> [{}:{}:{}:{}]{} '{}'",
            weave::syntax::GetName(token->Kind),
            token->Source.Start.Offset,
            token->Source.End.Offset,
            startPosition.Line, startPosition.Column,
            endPosition.Line, endPosition.Column,
            token->IsMissing() ? " missing" : "",
            (not token->IsMissing()) ? this->_text.GetText(token->Source) : "");

        // this->Dispatch(token->TrailingTrivia.GetNode());
    }

    void OnTrivia(weave::syntax::SyntaxTrivia* trivia) override
    {
        Indent();
        auto startPosition = this->_text.GetLinePosition(trivia->Source.Start);
        auto endPosition = this->_text.GetLinePosition(trivia->Source.End);
        fmt::println("{} <{}:{}> [{}:{}:{}:{}]",
            weave::syntax::GetName(trivia->Kind),
            trivia->Source.Start.Offset,
            trivia->Source.End.Offset,
            startPosition.Line, startPosition.Column,
            endPosition.Line, endPosition.Column);
    }
};

namespace xxx2
{
    struct CommandLineGroup;
    struct CommandLineOption;

    struct CommandLineGroup final
    {
        std::string_view Name{};
        bool Hidden = false;
        std::vector<CommandLineOption> Options{};

        // CommandLineGroup(
        //     std::string_view name,
        //     std::initializer_list<CommandLineOption> options,
        //     bool hidden = false)
        //     : Name{name
        //     , Options{options}
        //     , Hidden{hidden}
        //{
        // }
    };

    struct CommandLineOption final
    {
        std::string_view Name{};
        std::string_view Description{};
        bool Hidden = false;

        std::function<void(std::string_view)> OnValue{};
        std::function<void()> OnFlag{};
    };

    std::vector<std::string_view> Parse(std::vector<CommandLineGroup> const& groups, std::span<const char*> args)
    {
        auto it = args.begin();
        ++it; // Skip the first argument - it's the path to executable.
        auto end = args.end();

        auto consume = [&]() -> std::optional<std::string_view>
        {
            if (it != end)
            {
                std::string_view name{*it};
                ++it;
                return name;
            }

            return {};
        };

        auto findOption = [&](std::string_view name) -> CommandLineOption const*
        {
            for (auto const& group : groups)
            {
                for (auto const& option : group.Options)
                {
                    if (option.Name == name)
                    {
                        return &option;
                    }
                }
            }

            return {};
        };

        std::vector<std::string_view> result{};

        bool parseOptions = true;

        while (it != end)
        {
            if (auto argument = consume())
            {
                if (argument->starts_with("-") and parseOptions)
                {
                    if (argument == "--")
                    {
                        parseOptions = false;
                        continue;
                    }

                    if (auto option = findOption(*argument))
                    {
                        if (option->OnFlag)
                        {
                            option->OnFlag();
                        }
                        else if (auto value = consume())
                        {
                            option->OnValue(*value);
                        }
                        else
                        {
                            fmt::println(stderr, "error: missing value for '{}'", *argument);
                        }
                    }
                    else
                    {
                        fmt::println(stderr, "error: unknown option '{}'", *argument);
                    }
                }
                else
                {
                    result.push_back(*argument);
                }
            }
        }

        return result;
    }

    void PrintHelp(std::vector<CommandLineGroup> const& groups)
    {
        fmt::println("Usage: <executable> [OPTIONS] [FILES]");
        fmt::println("");

        size_t const longestOptionName = [&]() -> size_t
        {
            size_t result = 0;
            for (auto const& group : groups)
            {
                for (auto const& option : group.Options)
                {
                    result = (std::max)(result, option.Name.size());
                }
            }
            return result;
        }();

        for (auto const& group : groups)
        {
            if (group.Hidden)
            {
                continue;
            }

            fmt::println("{}", group.Name);
            for (auto const& option : group.Options)
            {
                if (option.Hidden)
                {
                    continue;
                }

                fmt::println("    {:{}} : {}", option.Name, longestOptionName, option.Description);
                fmt::println("");
            }

            fmt::println("");
        }
    }
}

#include <weave/commandline/CommandLineParser.hxx>

namespace xxx
{
    enum class TargetKind
    {
        Application,
        ConsoleApplication,
        Library,
        Module,
    };

    enum class TargetPlatform
    {
        Linux,
        Windows,
        MacOS,
        Android,
    };

    enum class TargetArchitecture
    {
        X64,
        AArch64,
        RiscV64,
    };

    enum class OptimizationLevel
    {
        None,
        Minimal,
        Default,
        Full,
    };

    enum class WarningReport
    {
        Disabled,
        Warning,
        Error,
    };

    class CompilerOptions : public weave::commandline::CommandLineParser
    {
    public:
        struct OutputOptions
        {
            std::string OutputPath{};
            std::string ImmediatePath{};
            std::string GeneratedPath{};
            TargetKind Target{};
            std::string Name{};
            std::string DocumentationPath{};
        } Output{};

        struct InputOptions
        {
            std::vector<std::string> References{};
            std::vector<std::string> Sources{};
        } Input{};

        struct CodeGenerationOptions
        {
            bool Checked{};
            bool Debug{};
            bool Unsafe{};
            OptimizationLevel OptimizationLevel{};
            TargetPlatform Platform{};
            TargetArchitecture Architecture{};
            bool Deterministic{};
            bool Coverage{};
            bool SanitizeAddress{};
            bool SanitizeThread{};
            bool SanitizeMemory{};
            bool SanitizeUndefined{};
        } CodeGeneration{};

        struct ErrorsAndWarningsOptions
        {
            int WarningLevel{};
            std::map<uint32_t, WarningReport> Warnings{};
        } ErrorsAndWarnings{};

        struct ResourceOptions
        {
            std::vector<std::string> ResourcePaths{};
        } Resources{};

        struct AnalyzeOptions
        {
            bool Analyze{};
        } Analyze{};

        struct EmitOptions
        {
            bool Documentation{};
            bool Dependency{};
            bool Metadata{};
            bool AssemblyHeader{};
        } Emit{};

        bool Help{};
        bool Verbose{};
        bool NoLogo{};
        bool Version{};

        struct ExperimentalOptions
        {
            bool PrintSyntaxTree{};
            bool PrintSemanticTree{};
        } Experimental{};

    protected:
        void OnPrintHelp() const override
        {
            fmt::print(R"__(usage: weavec [options] arguments

Miscellaneous options:

    -help       Prints this help message
    -version    Prints version information
    -nologo     Suppresses the logo banner
    -verbose    Use verbose output

Output options:

    -o:output=<path>            The path to the output files
    -o:immediate=<path>         The path to the immediate files
    -o:generated=<path>         The path to the generated files
    -o:target=<value>           The target kind
    -o:name=<value>             The name of the module
    -o:documentation            The path to the generated documentation files

Input options:

    -i:reference                The input path to module reference

Code generation options:

    -c:architecture=<value>     The target architecture
    -c:platform=<value>         The target platform
    -c:architecture             The target architecture
    -c:platform                 The target platform
    -c:checked                  Enable overflow checking
    -c:debug                    Enable debugging information
    -c:unsafe                   Allow unsafe code
    -c:optimize=<level>         Set optimization level
    -c:deterministic            Produce deterministic output
    -c:coverage                 Enable code coverage
    -c:sanitize-address         Enable address sanitizer
    -c:sanitize-thread          Enable thread sanitizer
    -c:sanitize-memory          Enable memory sanitizer
    -c:sanitize-undefined       Enable undefined behavior sanitizer

Diagnostic options:
    -w:level                    Set warning level
    -w:error=<id>               Treats a specific warning as an error
    -w:warn=<id>                Treats a specific warning as a warning
    -w:disable=<id>             Disables a specific warning
    -w:default=<id>             Resets a specific warning to its default level

Resource options:

    -r:resource=<path>          The input path to resource file

Analyze options:

    -a:analyze                  Enable static analysis

Emit options:

    -e:documentation            Emit documentation
    -e:dependency               Emit dependency
    -e:metadata                 Emit metadata
    -e:assembly-header          Emit assembly header

Experimental options;

    -x:print-syntax-tree        Print syntax tree
    -x:print-semantic-tree      Print semantic tree

)__");
        }

        void OnPositionalArgument(std::string_view value) override
        {
            this->Input.Sources.emplace_back(value);
        }

        bool OnOption(std::string_view name, std::optional<std::string_view> value) override
        {
            if (name == "help")
            {
                auto const parsed = TryParseBool(value);
                this->Help = parsed.value_or(true);
                return true;
            }
            else if (name == "verbose")
            {
                auto const parsed = TryParseBool(value);
                this->Verbose = parsed.value_or(true);
                return true;
            }
            else if (name == "version")
            {
                auto const parsed = TryParseBool(value);
                this->Verbose = parsed.value_or(true);
                return true;
            }
            else if (name == "nologo")
            {
                auto const parsed = TryParseBool(value);
                this->NoLogo = parsed.value_or(true);
                return true;
            }
            else if (name == "o:output")
            {
                if (auto const parsed = TryParseFilePath(value))
                {
                    this->Output.OutputPath = *parsed;
                    return true;
                }
            }
            else if (name == "o:immediate")
            {
                if (auto const parsed = TryParseFilePath(value))
                {
                    this->Output.ImmediatePath = *parsed;
                    return true;
                }
            }
            else if (name == "o:generated")
            {
                if (auto const parsed = TryParseFilePath(value))
                {
                    this->Output.GeneratedPath = *parsed;
                    return true;
                }
            }
            else if (name == "o:target")
            {
                if (auto const parsed = TryParseTargetKind(value))
                {
                    this->Output.Target = *parsed;
                    return true;
                }
            }
            else if (name == "o:name")
            {
                if (auto const parsed = TryParseString(value))
                {
                    this->Output.Name = *parsed;
                    return true;
                }
            }
            else if (name == "o:documentation")
            {
                if (auto const parsed = TryParseFilePath(value))
                {
                    this->Output.DocumentationPath = *parsed;
                    return true;
                }
            }
            else if (name == "i:reference")
            {
                if (auto const parsed = TryParseFilePath(value))
                {
                    this->Input.References.emplace_back(*value);
                    return true;
                }
            }
            else if (name == "c:architecture")
            {
                if (auto const parsed = TryParseTargetArchitecture(value))
                {
                    this->CodeGeneration.Architecture = *parsed;
                    return true;
                }
            }
            else if (name == "c:platform")
            {
                if (auto const parsed = TryParseTargetPlatform(value))
                {
                    this->CodeGeneration.Platform = *parsed;
                    return true;
                }
            }
            else if (name == "c:checked")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.Checked = parsed.value_or(true);
                return true;
            }
            else if (name == "c:debug")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.Debug = parsed.value_or(true);
                return true;
            }
            else if (name == "c:unsafe")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.Unsafe = parsed.value_or(true);
                return true;
            }
            else if (name == "c:optimize")
            {
                if (auto const parsed = TryParseOptimizationLevel(value))
                {
                    this->CodeGeneration.OptimizationLevel = *parsed;
                    return true;
                }
            }
            else if (name == "c:deterministic")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.Deterministic = parsed.value_or(true);
                return true;
            }
            else if (name == "c:coverage")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.Coverage = parsed.value_or(true);
                return true;
            }
            else if (name == "c:sanitize-address")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.SanitizeAddress = parsed.value_or(true);
                return true;
            }
            else if (name == "c:sanitize-thread")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.SanitizeThread = parsed.value_or(true);
                return true;
            }
            else if (name == "c:sanitize-memory")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.SanitizeMemory = parsed.value_or(true);
                return true;
            }
            else if (name == "c:sanitize-undefined")
            {
                auto const parsed = TryParseBool(value);
                this->CodeGeneration.SanitizeUndefined = parsed.value_or(true);
                return true;
            }
            else if (name == "w:level")
            {
                if (auto const parsed = TryParseInt32(value))
                {
                    this->ErrorsAndWarnings.WarningLevel = *parsed;
                    return true;
                }
            }
            else if (name == "w:error")
            {
                if (auto const parsed = TryParseUInt32(value))
                {
                    this->ErrorsAndWarnings.Warnings[*parsed] = WarningReport::Warning;
                    return true;
                }
            }
            else if (name == "w:warn")
            {
                if (auto const parsed = TryParseUInt32(value))
                {
                    this->ErrorsAndWarnings.Warnings[*parsed] = WarningReport::Warning;
                    return true;
                }
            }
            else if (name == "w:disable")
            {
                if (auto const parsed = TryParseUInt32(value))
                {
                    this->ErrorsAndWarnings.Warnings[*parsed] = WarningReport::Disabled;
                    return true;
                }
            }
            else if (name == "w:default")
            {
                if (auto const parsed = TryParseUInt32(value))
                {
                    this->ErrorsAndWarnings.Warnings.erase(*parsed);
                    return true;
                }
            }
            else if (name == "r:resource")
            {
                if (auto const parsed = TryParseFilePath(value))
                {
                    this->Resources.ResourcePaths.emplace_back(*parsed);
                    return true;
                }
            }
            else if (name == "a:analyze")
            {
                auto const parsed = TryParseBool(value);
                this->Analyze.Analyze = parsed.value_or(true);
                return true;
            }
            else if (name == "e:documentation")
            {
                auto const parsed = TryParseBool(value);
                this->Emit.Documentation = parsed.value_or(true);
                return true;
            }
            else if (name == "e:dependency")
            {
                auto const parsed = TryParseBool(value);
                this->Emit.Dependency = parsed.value_or(true);
                return true;
            }
            else if (name == "e:metadata")
            {
                auto const parsed = TryParseBool(value);
                this->Emit.Metadata = parsed.value_or(true);
                return true;
            }
            else if (name == "e:assembly-header")
            {
                auto const parsed = TryParseBool(value);
                this->Emit.AssemblyHeader = parsed.value_or(true);
                return true;
            }
            else if (name == "x:print-syntax-tree")
            {
                auto const parsed = TryParseBool(value);
                this->Experimental.PrintSyntaxTree = parsed.value_or(true);
                return true;
            }
            else if (name == "x:print-semantic-tree")
            {
                auto const parsed = TryParseBool(value);
                this->Experimental.PrintSemanticTree = parsed.value_or(true);
                return true;
            }

            return false;
        }

    private:
        static std::optional<TargetKind> TryParseTargetKind(std::optional<std::string_view> const& value)
        {
            if (value)
            {
                if (*value == "application")
                {
                    return TargetKind::Application;
                }
                else if (*value == "console")
                {
                    return TargetKind::ConsoleApplication;
                }
                else if (*value == "library")
                {
                    return TargetKind::Library;
                }
                else if (*value == "module")
                {
                    return TargetKind::Module;
                }
            }

            return {};
        }

        static std::optional<TargetArchitecture> TryParseTargetArchitecture(std::optional<std::string_view> const& value)
        {
            if (value)
            {
                if (*value == "x64")
                {
                    return TargetArchitecture::X64;
                }

                if (*value == "aarch64")
                {
                    return TargetArchitecture::AArch64;
                }

                if (*value == "riscv64")
                {
                    return TargetArchitecture::RiscV64;
                }
            }

            return {};
        }

        static std::optional<TargetPlatform> TryParseTargetPlatform(std::optional<std::string_view> const& value)
        {
            if (value)
            {
                if (*value == "linux")
                {
                    return TargetPlatform::Linux;
                }

                if (*value == "windows")
                {
                    return TargetPlatform::Windows;
                }

                if (*value == "macos")
                {
                    return TargetPlatform::MacOS;
                }

                if (*value == "android")
                {
                    return TargetPlatform::Android;
                }
            }

            return {};
        }

        static std ::optional<OptimizationLevel> TryParseOptimizationLevel(std::optional<std::string_view> const& value)
        {
            if (value)
            {
                if (*value == "none")
                {
                    return OptimizationLevel::None;
                }

                if (*value == "minimal")
                {
                    return OptimizationLevel::Minimal;
                }

                if (*value == "default")
                {
                    return OptimizationLevel::Default;
                }

                if (*value == "full")
                {
                    return OptimizationLevel::Full;
                }
            }

            return {};
        }
    };
}

int main(int argc, char* argv[])
{
    xxx::CompilerOptions options{};

    if (options.Parse(argc, argv))
    {
        if (options.Help)
        {
            options.PrintHelp();
            return EXIT_SUCCESS;
        }

        using namespace weave;
    
        auto const& files = options.Input.Sources;

        if (files.empty())
        {
            fmt::println(stderr, "No input files specified");
            fflush(stdout);
            fflush(stderr);
            return EXIT_FAILURE;
        }

        // if (files.size() > 1)
        //{
        //     fmt::println(stderr, "Too many files specified");
        //     return EXIT_FAILURE;
        // }

        auto parsing_timing = time::Instant::Now();
        if (auto file = filesystem::ReadTextFile(files.front()); file.has_value())
        {
            source::SourceText text{std::move(*file)};
            source::DiagnosticSink diagnostic{"<source>"};
            syntax::SyntaxFactory factory{};

            syntax::Parser parser{&diagnostic, &factory, text};

            if (options.Experimental.PrintSyntaxTree)
            {
                auto* root = parser.ParseSourceFile();
                {
                    SyntaxTreeStructurePrinter printer{text};
                    printer.Dispatch(root);
                }
                {
                    weave::syntax::Validate(root, &diagnostic);

                    ErrorReporter error{diagnostic};
                    error.Dispatch(root);
                    std::vector<std::string> diag{};
                    source::FormatDiagnostics(diag, text, diagnostic, 1000);

                    for (std::string const& item : diag)
                    {
                        fmt::println(stderr, "{}", item);
                    }
                }

                fflush(stdout);
                fflush(stderr);
                return EXIT_SUCCESS;
            }

            class TokenPrintingWalker : public syntax::SyntaxWalker
            {
            private:
                source::SourceText const& _text;

            public:
                TokenPrintingWalker(source::SourceText const& text)
                    : _text{text}
                {
                }

                void OnToken(syntax::SyntaxToken* token) override
                {
                    this->Dispatch(token->LeadingTrivia.GetNode());

                    // if ((token->Kind == syntax::SyntaxKind::OpenBraceToken) or (token->Kind == syntax::SyntaxKind::CloseBraceToken))
                    //{
                    //     fmt::print("{}", GetSpelling(token->Kind));
                    // }
                    // else
                    if (token->IsMissing())
                    {
                        fmt::print("/*missing:'{}'*/", weave::syntax::GetSpelling(token->Kind));
                    }
                    else
                    {
                        fmt::print("{}", _text.GetText(token->Source));
                    }

                    this->Dispatch(token->TrailingTrivia.GetNode());
                }

                void OnTrivia(syntax::SyntaxTrivia* trivia) override
                {
                    fmt::print("{}", _text.GetText(trivia->Source));
                }

                void OnUnexpectedNodesSyntax(syntax::UnexpectedNodesSyntax* node) override
                {
                    fmt::print("/*>>>");
                    // fmt::print("\u001b[41m");
                    SyntaxWalker::OnUnexpectedNodesSyntax(node);
                    fmt::print("<<<*/");
                    // fmt::print("\u001b[0m");
                }
            };

            class GX : public syntax::SyntaxWalker
            {
            private:
                void Indent()
                {
                    fmt::print("{:{}}", "", this->Depth * 4);
                }

            public:
                GX(source::SourceText& text)
                    : _text{text}
                {
                }
                source::SourceText& _text;

            public:
                void OnNamespaceDeclarationSyntax(syntax::NamespaceDeclarationSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnNamespaceDeclarationSyntax(node);
                }

                void OnStructDeclarationSyntax(syntax::StructDeclarationSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnStructDeclarationSyntax(node);
                }

                void OnConceptDeclarationSyntax(syntax::ConceptDeclarationSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnConceptDeclarationSyntax(node);
                }

                void OnExtendDeclarationSyntax(syntax::ExtendDeclarationSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnExtendDeclarationSyntax(node);
                }

                void OnFunctionDeclarationSyntax(syntax::FunctionDeclarationSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnFunctionDeclarationSyntax(node);
                }

                void OnDefault(syntax::SyntaxNode* node) override
                {
                    Indent();
                    fmt::println("{} {}", __func__, syntax::GetSpelling(node->Kind));
                    SyntaxWalker::OnDefault(node);
                }

                void OnSyntaxList(syntax::SyntaxList* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnSyntaxList(node);
                }

                void OnBlockStatementSyntax(syntax::BlockStatementSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnBlockStatementSyntax(node);
                }

                void OnParameterListSyntax(syntax::ParameterListSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnParameterListSyntax(node);
                }

                void OnParameterSyntax(syntax::ParameterSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnParameterSyntax(node);
                }

                void OnArgumentListSyntax(syntax::ArgumentListSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnArgumentListSyntax(node);
                }

                void OnEmptyStatementSyntax(syntax::EmptyStatementSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnEmptyStatementSyntax(node);
                }

                void OnExpressionStatementSyntax(syntax::ExpressionStatementSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnExpressionStatementSyntax(node);
                }

                void OnToken(syntax::SyntaxToken* token) override
                {
                    this->Dispatch(token->LeadingTrivia.GetNode());

                    Indent();
                    fmt::println("{} `{}`", __func__, GetName(token->Kind), _text.GetText(token->Source));

                    this->Dispatch(token->TrailingTrivia.GetNode());
                }

                void OnTrivia(weave::syntax::SyntaxTrivia* trivia) override
                {
                    Indent();
                    fmt::println("{}::LeadingTrivia `{}`", __func__, GetSpelling(trivia->Kind));
                }

                void OnIncompleteDeclarationSyntax(syntax::IncompleteDeclarationSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnIncompleteDeclarationSyntax(node);
                }

                // void OnArgumentSyntax(syntax::ArgumentSyntax* node) override
                //{
                //     Indent();
                //     fmt::println("{}", __func__);
                //     SyntaxWalker::OnArgumentSyntax(node);
                // }

                void OnArrowExpressionClauseSyntax(syntax::ArrowExpressionClauseSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnArrowExpressionClauseSyntax(node);
                }

                void OnAssignmentExpressionSyntax(syntax::AssignmentExpressionSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnAssignmentExpressionSyntax(node);
                }

                void OnElseClauseSyntax(syntax::ElseClauseSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnElseClauseSyntax(node);
                }

                void OnUnexpectedNodesSyntax(syntax::UnexpectedNodesSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnUnexpectedNodesSyntax(node);
                }

                // void OnQualifiedNameSyntax(syntax::QualifiedNameSyntax* node) override
                //{
                //     Indent();
                //     fmt::println("{} `.`", __func__);
                //
                //     SyntaxWalker::OnQualifiedNameSyntax(node);
                // }
            };

            auto cu2 = parser.ParseSourceFile();
            fmt::println("------");
            {
                ErrorReporter rr{diagnostic};
                rr.Dispatch(cu2);
            }
            /*fmt::println("-------");
            {
                TreeLinearizer printer{};
                printer.Dispatch(cu2);
                for (auto k : printer.GetKinds())
                {
                    fmt::println("{}", GetName(k));
                }
            }*/
            fmt::println("-------");
            {
                TokenPrintingWalker printer{text};
                printer.Dispatch(cu2);
            }
            fmt::println("-------");
            GX gx{text};
            gx.Dispatch(cu2);
            fmt::println("-------");

            std::vector<std::string> diag{};
            source::FormatDiagnostics(diag, text, diagnostic, 1000);

            for (std::string const& item : diag)
            {
                fmt::println(stderr, "{}", item);
            }

            factory.DebugDump();
        }
        else
        {
            fmt::println(stderr, "Failed to open file: {}", files.front());
        }

        fmt::println("parsing took: {}", parsing_timing.QueryElapsed());
    }
    else
    {
        fflush(stdout);
        fflush(stderr);
        return EXIT_FAILURE;
    }

#if defined(WIN32) && false
    HANDLE hProcess = GetCurrentProcess();
    PROCESS_MEMORY_COUNTERS pmc{sizeof(pmc)};

    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        fmt::println("PageFaultCount: {}", pmc.PageFaultCount);
        fmt::println("PeakWorkingSetSize: {} ({})", filesystem::FormatBinarySize(pmc.PeakWorkingSetSize), pmc.PeakWorkingSetSize);
        fmt::println("WorkingSetSize: {} ({})", filesystem::FormatBinarySize(pmc.WorkingSetSize), pmc.WorkingSetSize);
        fmt::println("QuotaPeakPagedPoolUsage: {} ({})", filesystem::FormatBinarySize(pmc.QuotaPeakPagedPoolUsage), pmc.QuotaPeakPagedPoolUsage);
        fmt::println("QuotaPagedPoolUsage: {} ({})", filesystem::FormatBinarySize(pmc.QuotaPagedPoolUsage), pmc.QuotaPagedPoolUsage);
        fmt::println("QuotaPeakNonPagedPoolUsage: {} ({})", filesystem::FormatBinarySize(pmc.QuotaPeakNonPagedPoolUsage), pmc.QuotaPeakNonPagedPoolUsage);
        fmt::println("QuotaNonPagedPoolUsage: {} ({})", filesystem::FormatBinarySize(pmc.QuotaNonPagedPoolUsage), pmc.QuotaNonPagedPoolUsage);
        fmt::println("PagefileUsage: {} ({})", filesystem::FormatBinarySize(pmc.PagefileUsage), pmc.PagefileUsage);
        fmt::println("PeakPagefileUsage: {} ({})", filesystem::FormatBinarySize(pmc.PeakPagefileUsage), pmc.PeakPagefileUsage);
    }
#endif

    fflush(stdout);

    return 0;
}
