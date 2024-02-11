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

            fflush(stdout);
            fflush(stderr);
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
            fflush(stdout);
            fflush(stderr);
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

            if (driver.Experimental.Format == session::PrintFormat::AST)
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

                void OnIdentifierNameSyntax(syntax::IdentifierNameSyntax* node) override
                {
                    syntax::IdentifierSyntaxToken* id = node->Identifier->TryCast<syntax::IdentifierSyntaxToken>();

                    Indent();
                    fmt::println("{} `{}`", __func__, id->Identifier);
                    // SyntaxWalker::OnIdentifierNameSyntax(node);
                }

                //void OnArgumentSyntax(syntax::ArgumentSyntax* node) override
                //{
                //    Indent();
                //    fmt::println("{}", __func__);
                //    SyntaxWalker::OnArgumentSyntax(node);
                //}

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
        fmt::println(stderr, "{}", matched.error());
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
