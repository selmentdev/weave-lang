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

#include <atomic>

namespace weave::tasking
{
    struct TaskHandle
    {
        void* Native;

        [[nodiscard]] constexpr auto operator<=>(TaskHandle const&) const = default;
    };
}

namespace weave::tasking::impl
{
    enum class TaskState
    {
        Initialized,
        Dispatched,
        Executing,
        Completed,
    };

    struct Task
    {
        std::atomic_size_t Waiters{0};
        TaskHandle Parent{};
        TaskState State{TaskState::Completed};
        threading::Runnable* Callback;

        void InitializeBarrier(TaskHandle parent)
        {
            WEAVE_ASSERT(this->Waiters == 0);
            WEAVE_ASSERT(this->State == TaskState::Completed);

            this->Waiters = 1;
            this->Callback = nullptr;
            this->Parent = parent;
            this->State = TaskState::Initialized;

            if (Task* task = static_cast<Task*>(parent.Native))
            {
                ++task->Waiters;
            }
        }

        void InitializeRunnable(threading::Runnable* runnable, TaskHandle parent) noexcept
        {
            WEAVE_ASSERT(this->Waiters == 0);
            WEAVE_ASSERT(this->State == TaskState::Completed);

            this->Waiters = 1;
            this->Callback = runnable;
            this->Parent = parent;
            this->State = TaskState::Initialized;

            if (Task* task = static_cast<Task*>(parent.Native))
            {
                ++task->Waiters;
            }
        }

        void Finish() noexcept
        {
            if (--this->Waiters == 0)
            {
                WEAVE_ASSERT(this->State == TaskState::Executing);
                this->State = TaskState::Completed;

                if (Task* task = static_cast<Task*>(this->Parent.Native))
                {
                    task->Finish();
                }
            }
        }

        void Dispatched() noexcept
        {
            WEAVE_ASSERT(this->State == TaskState::Initialized);
            this->State = TaskState::Dispatched;
        }

        void Execute() noexcept
        {
            WEAVE_ASSERT(this->State == TaskState::Dispatched);
            this->State = TaskState::Executing;

            if (threading::Runnable* const runnable = this->Callback; runnable != nullptr)
            {
                runnable->Run();
            }

            this->Finish();
        }

        bool IsFinished() const noexcept
        {
            return this->Waiters == 0;
        }
    };

    static_assert(std::is_trivially_destructible_v<Task>);
}

class Tazg : public weave::threading::Runnable
{
private:
    weave::threading::CriticalSection& _lock;
    int& _resource;

public:
    Tazg(weave::threading::CriticalSection& lock, int& resource)
        : _lock(lock)
        , _resource(resource)
    {
    }

protected:
    void Execute() override
    {
        fmt::println("Tazg::Run()");

        for (size_t i = 0; i < 1000000; ++i)
        {
            _lock.Enter();
            ++_resource;
            _lock.Leave();
        }
    }
};

namespace xxx
{

    using namespace weave;

    void Enumerate(filesystem::DirectoryEnumerator enumerator)
    {
        while (auto it = enumerator.Next())
        {
            if (*it)
            {
                fmt::println("{}: {}", std::to_underlying((*it)->Type), (*it)->Path);
            }
            else
            {
                fmt::println("error: {}", std::to_underlying(it->error()));
                break;
            }
        }
    }
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

        weave::errors::Handler handler{};
        session::CodeGeneratorOptions codegen{};
        session::ParseOptions(codegen, handler, matched->GetValues("codegen"));

        session::ExperimentalOptions experimental{};
        session::ParseOptions(experimental, handler, matched->GetValues("experimental"));

        session::EmitOptions emit{};
        session::ParseOptions(emit, handler, matched->GetValues("emit"));

        for (auto const& message : handler.GetMessages())
        {
            fmt::println(stderr, "codegen errors: {}", message.Value);
        }

        fmt::println("cmd.debug: {}", codegen.Debug);


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
                    fmt::println("{}: {}", v.Visit(node) , node->IdentifierToken->GetIdentifier());

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
                        fmt::println("{}", tokenizer::TokenKindTraits::GetName(tk->GetKind()));
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

    fmt::println("Elapsed: {}", started.QueryElapsed());
    fflush(stdout);    

    return 0;
}
