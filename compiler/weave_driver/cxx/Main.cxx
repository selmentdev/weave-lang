#include <fmt/format.h>
#include <fmt/chrono.h>


#include <charconv>
#include <cstdlib>
#include <map>
#include "weave/core/String.hxx"
#include "weave/commandline/CommandLineParser.hxx"
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

    class CompilerOptions
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

        void Apply(weave::commandline::ArgumentParseResult const& arguments)
        {
            this->Help = arguments.Contains("-help");
            this->Verbose = arguments.Contains("-verbose");
            this->Version = arguments.Contains("-version");
            this->NoLogo = arguments.Contains("-nologo");

            if (auto const parsed = weave::commandline::TryParseFilePath(arguments.GetValue("-o:output")))
            {
                this->Output.OutputPath = *parsed;
            }

            if (auto const parsed = weave::commandline::TryParseFilePath(arguments.GetValue("-o:immediate")))
            {
                this->Output.ImmediatePath = *parsed;
            }

            if (auto const parsed = weave::commandline::TryParseFilePath(arguments.GetValue("-o:generated")))
            {
                this->Output.GeneratedPath = *parsed;
            }

            if (auto const parsed = TryParseTargetKind(arguments.GetValue("-o:target")))
            {
                this->Output.Target = *parsed;
            }

            if (auto const parsed = arguments.GetValue("-o:name"))
            {
                this->Output.Name = *parsed;
            }

            if (auto const parsed = weave::commandline::TryParseFilePath(arguments.GetValue("-o:documentation")))
            {
                this->Output.DocumentationPath = *parsed;
            }

            if (auto const parsed = weave::commandline::TryParseFilePath(arguments.GetValue("-i:reference")))
            {
                this->Input.References.emplace_back(*parsed);
            }

            if (auto const parsed = TryParseTargetArchitecture(arguments.GetValue("-c:architecture")))
            {
                this->CodeGeneration.Architecture = *parsed;
            }

            if (auto const parsed = TryParseTargetPlatform(arguments.GetValue("-c:platform")))
            {
                this->CodeGeneration.Platform = *parsed;
            }

            this->CodeGeneration.Checked = arguments.Contains("-c:checked");
            this->CodeGeneration.Debug = arguments.Contains("-c:debug");
            this->CodeGeneration.Unsafe = arguments.Contains("-c:unsafe");
            if (auto const parsed = TryParseOptimizationLevel(arguments.GetValue("-c:optimize")))
            {
                this->CodeGeneration.OptimizationLevel = *parsed;
            }
            this->CodeGeneration.Deterministic = arguments.Contains("-c:deterministic");
            this->CodeGeneration.Coverage = arguments.Contains("-c:coverage");
            this->CodeGeneration.SanitizeAddress = arguments.Contains("-c:sanitize-address");
            this->CodeGeneration.SanitizeThread = arguments.Contains("-c:sanitize-thread");
            this->CodeGeneration.SanitizeMemory = arguments.Contains("-c:sanitize-memory");
            this->CodeGeneration.SanitizeUndefined = arguments.Contains("-c:sanitize-undefined");
            if (auto const parsed = weave::commandline::TryParseInt32(arguments.GetValue("-w:level")))
            {
                this->ErrorsAndWarnings.WarningLevel = *parsed;
            }
            if (auto const parsed = weave::commandline::TryParseUInt32(arguments.GetValue("-w:error")))
            {
                this->ErrorsAndWarnings.Warnings[*parsed] = WarningReport::Warning;
            }
            if (auto const parsed = weave::commandline::TryParseUInt32(arguments.GetValue("-w:warning")))
            {
                this->ErrorsAndWarnings.Warnings[*parsed] = WarningReport::Warning;
            }
            if (auto const parsed = weave::commandline::TryParseUInt32(arguments.GetValue("-w:disable")))
            {
                this->ErrorsAndWarnings.Warnings[*parsed] = WarningReport::Disabled;
            }
            if (auto const parsed = weave::commandline::TryParseUInt32(arguments.GetValue("-w:default")))
            {
                this->ErrorsAndWarnings.Warnings.erase(*parsed);
            }
            if (auto const parsed = weave::commandline::TryParseFilePath(arguments.GetValue("-r:resource")))
            {
                this->Resources.ResourcePaths.emplace_back(*parsed);
            }
            this->Analyze.Analyze = arguments.Contains("-a:analyze");
            this->Emit.Documentation = arguments.Contains("-e:documentation");
            this->Emit.Dependency = arguments.Contains("-e:dependency");
            this->Emit.Metadata = arguments.Contains("-e:metadata");
            this->Emit.AssemblyHeader = arguments.Contains("-e:assembly-header");
            this->Experimental.PrintSyntaxTree = arguments.Contains("-x:print-syntax-tree");
            this->Experimental.PrintSemanticTree = arguments.Contains("-x:print-semantic-tree");

            for (auto const& path : arguments.GetPositional())
            {
                this->Input.Sources.emplace_back(path);
            }
        }

    private:
        static std::optional<TargetKind>
        TryParseTargetKind(std::optional<std::string_view> const& value)
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
    std::string_view const appname = weave::filesystem::path::GetFilenameWithoutExtension(argv[0]);

    weave::commandline::ArgumentEnumerator enumerator{argc, argv};

    weave::commandline::ArgumentParser argumentParser{};

    argumentParser.AddOption("-help", "Prints this help message");
    argumentParser.AddOption("-version", "Prints version information");
    argumentParser.AddOption("-nologo", "Suppresses the logo banner");
    argumentParser.AddOption("-verbose", "Use verbose output");


    argumentParser.AddOption("-o:output",            "The path to the output files", "path");
    argumentParser.AddOption("-o:immediate",         "The path to the immediate files", "path");
    argumentParser.AddOption("-o:generated",         "The path to the generated files", "path");
    argumentParser.AddOption("-o:target",           "The target kind", "value");
    argumentParser.AddOption("-o:name",             "The name of the module", "value");
    argumentParser.AddOption("-o:documentation",            "The path to the generated documentation files");

    argumentParser.AddOption("-i:reference", "The input path to module reference");

    argumentParser.AddOption("-c:architecture",     "The target architecture", "value");
    argumentParser.AddOption("-c:platform",         "The target platform", "value");
    argumentParser.AddOption("-c:architecture",             "The target architecture");
    argumentParser.AddOption("-c:platform",                 "The target platform");
    argumentParser.AddOption("-c:checked",                  "Enable overflow checking");
    argumentParser.AddOption("-c:debug",                    "Enable debugging information");
    argumentParser.AddOption("-c:unsafe",                   "Allow unsafe code");
    argumentParser.AddOption("-c:optimize",         "Set optimization level", "value");
    argumentParser.AddOption("-c:deterministic",            "Produce deterministic output");
    argumentParser.AddOption("-c:coverage",                 "Enable code coverage");
    argumentParser.AddOption("-c:sanitize-address",         "Enable address sanitizer");
    argumentParser.AddOption("-c:sanitize-thread",          "Enable thread sanitizer");
    argumentParser.AddOption("-c:sanitize-memory",          "Enable memory sanitizer");
    argumentParser.AddOption("-c:sanitize-undefined",       "Enable undefined behavior sanitizer");

    argumentParser.AddOption("-w:level",                    "Set warning level");
    argumentParser.AddOption("-w:error",               "Treats a specific warning as an error", "id");
    argumentParser.AddOption("-w:warning",                "Treats a specific warning as a warning", "id");
    argumentParser.AddOption("-w:disable",             "Disables a specific warning", "id");
    argumentParser.AddOption("-w:default",             "Resets a specific warning to its default level", "id");

    argumentParser.AddOption("-r:resource",          "The input path to resource file", "path");

    argumentParser.AddOption("-a:analyze",                  "Enable static analysis");

    argumentParser.AddOption("-e:documentation",            "Emit documentation");
    argumentParser.AddOption("-e:dependency",               "Emit dependency");
    argumentParser.AddOption("-e:metadata",                 "Emit metadata");
    argumentParser.AddOption("-e:assembly-header",          "Emit assembly header");

    argumentParser.AddOption("-x:print-syntax-tree",        "Print syntax tree");
    argumentParser.AddOption("-x:print-semantic-tree",      "Print semantic tree");

    xxx::CompilerOptions options{};

    if (auto result = argumentParser.Parse(enumerator))
    {
        options.Apply(result.value());

        if (options.Help)
        {
            argumentParser.PrintUsage(appname);
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
