#include <fmt/format.h>
#include <fmt/chrono.h>


#include <charconv>
#include <cstdlib>

#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/tokenizer/Token.hxx"
#include "weave/filesystem/FileSystem.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/tokenizer/Tokenizer.hxx"
#include "weave/tokenizer/TokenizerContext.hxx"
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

#include "weave/syntax2/Lexer.hxx"

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

            syntax2::Lexer ll{diagnostic, text, syntax2::LexerTriviaMode::All};

            syntax2::TokenInfo ti{};
            while (ll.Lex(ti))
            {
                if (ti.Kind == syntax2::SyntaxKind::EndOfFile)
                {
                    break;
                }

                auto ps = text.GetLineSpan(ti.Source);
                fmt::println(
                    "LL2: {} {} {} `{}` `{}` `{}` `{}:{}-{}:{}`",
                    syntax2::SyntaxKindTraits::GetCategoryName(ti.Kind),
                    syntax2::SyntaxKindTraits::GetName(ti.Kind),
                    syntax2::SyntaxKindTraits::GetSpelling(ti.Kind),
                    text.GetText(ti.Source),
                    ti.Value,
                    ti.Suffix,
                    ps.Start.Line,
                    ps.Start.Column,
                    ps.End.Line,
                    ps.End.Column
                    );
            }

            syntax::ParserContext context{};
            syntax::Parser parser{diagnostic, text, context};
            [[maybe_unused]] syntax::CompilationUnitDeclaration* cu = parser.Parse();

            WEAVE_ASSERT(cu != nullptr);

            struct V final : weave::syntax::SyntaxVisitor<V, std::string>
            {
                std::string VisitDefault(syntax::SyntaxNode* node)
                {
                    return std::string{syntax::SyntaxKindTraits::GetSpelling(node->Kind())};
                }

                std::string VisitCompilationUnitDeclaration(weave::syntax::CompilationUnitDeclaration* node) override
                {
                    SyntaxVisitor::VisitCompilationUnitDeclaration(node);

                    return "CompilationUnit (overridden)";
                }
            };

            struct W final : weave::syntax::SyntaxWalker
            {
                V v{};

                void PrintIndent()
                {
                    fmt::print("{}", std::string(this->Depth * 2, ' '));
                }

                void VisitCompilationUnitDeclaration(weave::syntax::CompilationUnitDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitCompilationUnitDeclaration(node);
                }

                void VisitStructDeclaration(syntax::StructDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitStructDeclaration(node);
                }

                void VisitExtendDeclaration(syntax::ExtendDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitExtendDeclaration(node);
                }

                void VisitConceptDeclaration(syntax::ConceptDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitConceptDeclaration(node);
                }

                void VisitFunctionDeclaration(syntax::FunctionDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitFunctionDeclaration(node);
                }

                void VisitFieldDeclaration(syntax::FieldDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitFieldDeclaration(node);
                }

                void VisitConstantDeclaration(syntax::ConstantDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitConstantDeclaration(node);
                }

                void VisitNamespaceDeclaration(syntax::NamespaceDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitNamespaceDeclaration(node);
                }

                void VisitUsingStatement(syntax::UsingStatement* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitUsingStatement(node);
                }

                void VisitQualifiedNameExpression(syntax::QualifiedNameExpression* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitQualifiedNameExpression(node);
                }

                void VisitSimpleNameExpression(syntax::SimpleNameExpression* node) override
                {
                    PrintIndent();
                    fmt::println("{}: {}", v.Visit(node), node->IdentifierToken->GetIdentifier());

                    SyntaxWalker::VisitSimpleNameExpression(node);
                }

                void VisitGenericNameExpression(syntax::GenericNameExpression* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    SyntaxWalker::VisitGenericNameExpression(node);
                }

                void VisitIdentifierNameExpression(syntax::IdentifierNameExpression* node) override
                {
                    PrintIndent();
                    fmt::println("{}: {}", v.Visit(node), node->IdentifierToken->GetIdentifier());

                    SyntaxWalker::VisitIdentifierNameExpression(node);
                }

                void VisitIncompleteMemberDeclaration(syntax::IncompleteMemberDeclaration* node) override
                {
                    PrintIndent();
                    fmt::println("{}", v.Visit(node));

                    ++Depth;

                    for (auto tk : node->Tokens)
                    {
                        PrintIndent();

                        if (auto id = tk->TryCast<tokenizer::IdentifierToken>())
                        {
                            fmt::println("{}", id->GetIdentifier());
                        }
                        else if (auto str = tk->TryCast<tokenizer::StringLiteralToken>())
                        {
                            fmt::println("\"{}\"", str->GetValue());
                        }
                        else if (auto chr = tk->TryCast<tokenizer::CharacterLiteralToken>())
                        {
                            fmt::println("{}", static_cast<uint32_t>(chr->GetValue()));
                        }
                        else if (auto num = tk->TryCast<tokenizer::IntegerLiteralToken>())
                        {
                            fmt::println("{}", num->GetValue());
                        }
                        else if (auto flt = tk->TryCast<tokenizer::FloatLiteralToken>())
                        {
                            fmt::println("{}", flt->GetValue());
                        }
                        else
                        {
                            fmt::println("{}", tokenizer::TokenKindTraits::GetName(tk->GetKind()));
                        }
                    }

                    --Depth;

                    SyntaxWalker::VisitIncompleteMemberDeclaration(node);
                }
            };
            W w{};
            w.Visit(cu);

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
        fmt::println("PageFaultCount: {}",                  pmc.PageFaultCount);
        fmt::println("PeakWorkingSetSize: {} ({})",         filesystem::FormatBinarySize(pmc.PeakWorkingSetSize), pmc.PeakWorkingSetSize);
        fmt::println("WorkingSetSize: {} ({})",             filesystem::FormatBinarySize(pmc.WorkingSetSize), pmc.WorkingSetSize);
        fmt::println("QuotaPeakPagedPoolUsage: {} ({})",    filesystem::FormatBinarySize(pmc.QuotaPeakPagedPoolUsage), pmc.QuotaPeakPagedPoolUsage);
        fmt::println("QuotaPagedPoolUsage: {} ({})",        filesystem::FormatBinarySize(pmc.QuotaPagedPoolUsage), pmc.QuotaPagedPoolUsage);
        fmt::println("QuotaPeakNonPagedPoolUsage: {} ({})", filesystem::FormatBinarySize(pmc.QuotaPeakNonPagedPoolUsage), pmc.QuotaPeakNonPagedPoolUsage);
        fmt::println("QuotaNonPagedPoolUsage: {} ({})",     filesystem::FormatBinarySize(pmc.QuotaNonPagedPoolUsage), pmc.QuotaNonPagedPoolUsage);
        fmt::println("PagefileUsage: {} ({})",              filesystem::FormatBinarySize(pmc.PagefileUsage), pmc.PagefileUsage);
        fmt::println("PeakPagefileUsage: {} ({})",          filesystem::FormatBinarySize(pmc.PeakPagefileUsage), pmc.PeakPagefileUsage);
    }
#endif

    fflush(stdout);

    return 0;
}
