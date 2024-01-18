#include <fmt/format.h>
#include <fmt/chrono.h>


#include <charconv>
#include <cstdlib>

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
#include "weave/syntax/Parser.hxx"
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

int main(int argc, const char* argv[])
{
    using namespace weave;

    commandline::CommandLineBuilder builder{};

    builder.Multiple(
        "codegen",
        "c",
        "Code generator options",
        "<name=value>");

    builder.Multiple(
        "emit",
        "e",
        "List of types of output to emit",
        "items");

    builder.Multiple(
        "experimental",
        "x",
        "Experimental options",
        "name[=value]");

    builder.Flag(
        "help",
        "h",
        "Prints help");

    builder.Flag(
        "verbose",
        "v",
        "Use verbose output");

    // Skip the first argument - it's the path to executable.
    if (auto matched = builder.Parse(argc - 1, argv + 1); matched.has_value())
    {
        if (matched->HasFlag("help"))
        {
            fmt::println("Usage: {} [OPTIONS]", argv[0]);
            fmt::println("");

            for (auto const& option : builder.GetOptions())
            {
                fmt::println("{}", commandline::FormatOption(option));
                fmt::println("    {}", option.Description);
                fmt::println("");
            }
            return EXIT_SUCCESS;
        }

        errors::Handler handler{};
        driver::Driver driver{};

        session::ParseOptions(driver.CodeGenerator, handler, matched->GetValues("codegen"));
        session::ParseOptions(driver.Experimental, handler, matched->GetValues("experimental"));
        session::ParseOptions(driver.Emit, handler, matched->GetValues("emit"));

        for (auto const& message : handler.GetMessages())
        {
            fmt::println(stderr, "{}", message.Value);
        }

        if (handler.HasErrors())
        {
            fmt::println(stderr, "aborting due to previous errors");
            return EXIT_FAILURE;
        }

        if (matched->HasFlag("verbose"))
        {
            driver.CodeGenerator.Dump();
            driver.Experimental.Dump();
            driver.Emit.Dump();
        }


        auto const& result = matched.value();

        auto const& files = result.GetPositional();

        fmt::println("---");

        for (auto const& f : files)
        {
            fmt::println("file: {}", f);
        }

        fmt::println("___");

        if (files.empty())
        {
            fmt::println(stderr, "No input files specified");
            return EXIT_FAILURE;
        }

        if (files.size() > 1)
        {
            fmt::println(stderr, "Too many files specified");
            return EXIT_FAILURE;
        }

        auto parsing_timing = time::Instant::Now();
        if (auto file = filesystem::ReadTextFile(files.front()); file.has_value())
        {
            source::SourceText text{std::move(*file)};
            source::DiagnosticSink diagnostic{"<source>"};
            syntax::SyntaxFactory factory{};

            syntax::Parser pp{&diagnostic, &factory, text};

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
                    for (auto tt : token->GetLeadingTrivia())
                    {
                        fmt::print("{}", _text.GetText(tt.Source));
                    }

                    if ((token->Kind == syntax::SyntaxKind::OpenBraceToken) or (token->Kind == syntax::SyntaxKind::CloseBraceToken))
                    {
                        fmt::print("{}", syntax::SyntaxKindTraits::GetSpelling(token->Kind));
                    }
                    else
                    {
                        fmt::print("{}", _text.GetText(token->Source));
                    }

                    for (auto tt : token->GetTrailingTrivia())
                    {
                        fmt::print("{}", _text.GetText(tt.Source));
                    }
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
                void OnCompilationUnitSyntax(syntax::CompilationUnitSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnCompilationUnitSyntax(node);
                }

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

                void OnDefault(syntax::SyntaxNode* node) override
                {
                    Indent();
                    fmt::println("{} {}", __func__, syntax::SyntaxKindTraits::GetSpelling(node->Kind));
                    SyntaxWalker::OnDefault(node);
                }

                void OnSyntaxList(syntax::SyntaxList* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnSyntaxList(node);
                }

                void OnToken(syntax::SyntaxToken* token) override
                {
#if false
                    for (auto t : token->GetLeadingTrivia())
                    {
                        Indent();
                        fmt::println("{}::LeadingTrivia `{}`", __func__, _text.GetText(t.Source));
                    }

                    Indent();
                    fmt::println("{} `{}`", __func__, syntax::SyntaxKindTraits::GetName(token->Kind), _text.GetText(token->Source));

                    for (auto t : token->GetLeadingTrivia())
                    {
                        Indent();
                        fmt::println("{}::TrailingTrivia `{}`", __func__, _text.GetText(t.Source));
                    }
#else
                    Indent();
                    fmt::println("{} `{}` `{}`",
                        __func__,
                        _text.GetText(token->Source),
                        weave::syntax::SyntaxKindTraits::GetSpelling(token->Kind));
#endif
                }

                void OnIncompleteDeclarationSyntax(syntax::IncompleteDeclarationSyntax* node) override
                {
                    Indent();
                    fmt::println("{}", __func__);
                    SyntaxWalker::OnIncompleteDeclarationSyntax(node);
                }

                void OnIdentifierNameSyntax(syntax::IdentifierNameSyntax* node) override
                {
                    syntax::IdentifierSyntaxToken* id = node->Identifier->TryCast<syntax::IdentifierSyntaxToken>();

                    Indent();
                    fmt::println("{} `{}`", __func__, id->Identifier);
                    // SyntaxWalker::OnIdentifierNameSyntax(node);
                }

                // void OnQualifiedNameSyntax(syntax::QualifiedNameSyntax* node) override
                //{
                //     Indent();
                //     fmt::println("{} `.`", __func__);
                //
                //     SyntaxWalker::OnQualifiedNameSyntax(node);
                // }
            };

            auto cu2 = pp.Parse();
            fmt::println("-------");
            {
                TreeLinearizer printer{};
                printer.Dispatch(cu2);
                for (auto k : printer.GetKinds())
                {
                    fmt::println("{}", syntax::SyntaxKindTraits::GetName(k));
                }
            }
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
        }
        else
        {
            fmt::println(stderr, "Failed to open file: {}", files.front());
        }

        fmt::println("parsing took: {}", parsing_timing.QueryElapsed());
    }
    else
    {
        fmt::println(stderr, "{}", matched.error());
        return EXIT_FAILURE;
    }

#if defined(WIN32)
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
