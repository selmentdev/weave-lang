#include <fmt/format.h>
#include <fmt/chrono.h>


#include <charconv>
#include <cstdlib>

#include "weave/Session.hxx"
#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/tokenizer/Token.hxx"
#include "weave/filesystem/FileSystem.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/tokenizer/Tokenizer.hxx"
#include "weave/tokenizer/TokenizerContext.hxx"
#include "weave/time/Instant.hxx"
#include "weave/Session.hxx"
#include "weave/platform/StringBuffer.hxx"

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
    {
        fmt::println("Zero: {}", weave::time::Duration{INT64_MAX});
        fmt::println("Min: {}", weave::time::DateTime{{0}});
        fmt::println("Max: {}", weave::time::DateTime{{std::numeric_limits<int64_t>::max()}});
        fmt::println("Now:    {}", weave::time::DateTime::Now());
        fmt::println("UtcNow: {}", weave::time::DateTime::UtcNow());
        fmt::println("Now:    {}", weave::time::DateTimeOffset::Now());
        fmt::println("Now:    {}", weave::time::DateTimeOffset::Now().ToLocal());
        fmt::println("Now:    {}", weave::time::DateTimeOffset::Now().ToUtc());
        fmt::println("Started: {:t}", weave::time::DateTime::Now());
        fmt::println("Started: {:D}", weave::time::DateTime::Now());
        fmt::println("Started: {:T}", weave::time::DateTime::Now());
        fmt::println("Started: {:F}", weave::time::DateTime::Now());
        fmt::println("Started: {}", weave::time::DateTime::Now());
    }
    {
#if defined(WIN32)
        weave::filesystem::DirectoryEnumerator enumerator{R"(d:\repos\weave-lang)"};
#else
        weave::filesystem::DirectoryEnumerator enumerator{"/home/selmentdev/repos/weave-lang"};
#endif

        for (size_t i = 0; i < 3; ++i)
        {
            if (auto it = enumerator.Next(); it.has_value() and it->has_value())
            {
                fmt::println("{}: {}", std::to_underlying((*it)->Type), (*it)->Path);
            }
            else
            {
                fmt::println("error: {}", std::to_underlying(it->error()));
                break;
            }
        }

        xxx::Enumerate(std::move(enumerator));
    }
    int resource = 0;
    {
        weave::threading::CriticalSection cs{};

        Tazg t1{cs, resource};
        weave::threading::Thread thread1{weave::threading::ThreadStart{.Callback = &t1}};

        Tazg t2{cs, resource};
        weave::threading::Thread thread2{weave::threading::ThreadStart{.Callback = &t1}};

        for (size_t i = 0; i < 1000000; ++i)
        {
            cs.Enter();
            ++resource;
            cs.Leave();
        }
    }

    fmt::println("resource: {}", resource);

#if defined(WIN32)
    {
        using namespace weave;
        if (auto handle = filesystem::FileHandle::Create("d:/profile.json", filesystem::FileMode::CreateAlways, filesystem::FileAccess::ReadWrite, {}))
        {
            filesystem::FileWriter writer{*handle};

            profiler::Profiler profiler{};
            {
                profiler::EventScope e{profiler, "Timeline", "Duration 1"};
                profiler.Event("General", "Event 1");
                threading::Sleep(time::Duration::FromMilliseconds(100));
                profiler::EventScope b{profiler, "Timeline", "Duration 2 (inner)"};
                threading::Sleep(time::Duration::FromMilliseconds(150));
                profiler.Event("General", "Event 2");
                threading::YieldThread(threading::YieldTarget::AnyThreadOnAnyProcessor);
                threading::YieldThread(threading::YieldTarget::AnyThreadOnSameProcessor);
                threading::YieldThread(threading::YieldTarget::SameOrHigherPriorityOnAnyProcessor);
                threading::Sleep(time::Duration::FromMilliseconds(150));

                profiler.Event("General", "Event 4");
                threading::Sleep(time::Duration::FromMilliseconds(200));
                profiler.Event("General", "Event 3");
                threading::Sleep(time::Duration::FromMilliseconds(200));
            }
            profiler.Serialize(writer);
        }
    }
#endif

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

    if (auto matched = builder.Parse(argc, argv); matched.has_value())
    {
        if (matched->HasFlag("help"))
        {
            fmt::println("Usage: {} [OPTIONS]", argv[0]);
            fmt::println("");

            for (auto const& option : builder.GetOptions())
            {
                fmt::println("{}", commandline::FormatOption(option));
                fmt::println("\t{}", option.Description);
                fmt::println("");
            }
            return EXIT_SUCCESS;
        }

        weave::errors::Handler handler{};
        auto cmd = weave::session::CodeGeneratorOptions::FromCommandLine(handler, *matched);

        for (auto const& message : handler.GetMessages())
        {
            fmt::println(stderr, "codegen errors: {}", message.Value);
        }

        fmt::println("cmd.debug: {}", cmd.Debug);


        auto const& result = matched.value();

        session::CodeGeneratorOptions codegen = session::CodeGeneratorOptions::FromCommandLine(handler, *matched);
        codegen.DebugPrint();

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


        if (auto file = filesystem::ReadTextFile(files.front()); file.has_value())
        {
            std::vector<tokenizer::Token*> tokens{};
            source::DiagnosticSink diagnostic{"<source>"};
            source::SourceText text{std::move(*file)};
            tokenizer::TokenizerContext context{};
            tokenizer::Tokenizer lexer{diagnostic, text, tokenizer::TriviaMode::All};

            time::Instant const started = time::Instant::Now();

            while (lexer.Lex())
            {
                for (auto const& t : lexer.GetLeadingTrivia())
                {
                    source::LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("ll: {}, {}:{}-{}:{}: {}",
                        tokenizer::TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }
                {
                    source::LineSpan const loc = text.GetLineSpan(lexer.GetSpan());
                    fmt::println("kk: {} ('{}'): '{}' '{}' '{}', {}:{}-{}:{}: {}",
                        tokenizer::TokenKindTraits::GetName(lexer.GetKind()),
                        tokenizer::TokenKindTraits::GetSpelling(lexer.GetKind()),
                        lexer.GetPrefix(), lexer.GetValue(), lexer.GetSuffix(),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        lexer.GetSpan().End.Offset - lexer.GetSpan().Start.Offset);
                }
                for (auto const& t : lexer.GetTrailingTrivia())
                {
                    source::LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("tt: {}, {}:{}-{}:{}: {}",
                        tokenizer::TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }

                if (lexer.GetKind() == tokenizer::TokenKind::EndOfFile)
                {
                    break;
                }
            }

            time::Duration const timeout = started.QueryElapsed();
            context.Strings.Dump();

            std::vector<std::string> diag{};
            source::FormatDiagnostics(diag, text, diagnostic, 1000);

            for (std::string const& item : diag)
            {
                fmt::println(stderr, "{}", item);
            }

            size_t allocated{};
            size_t reserved{};
            context.QueryMemoryUsage(allocated, reserved);
            context.DumpMemoryUsage();

            fmt::println("lexed in {} ms", timeout.ToMilliseconds());
            fmt::println("allocated: {}, reserved: {}", allocated, reserved);
        }
        else
        {
            fmt::println(stderr, "Failed to open file: {}", files.front());
        }
    }
    else
    {
        fmt::println(stderr, "{}", matched.error());
        return EXIT_FAILURE;
    }

    return 0;
}
