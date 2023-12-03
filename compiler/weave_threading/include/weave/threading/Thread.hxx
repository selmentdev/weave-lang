#pragma once
#include <optional>
#include <functional>
#include <string_view>
#include <compare>

namespace weave::threading::impl
{
    struct PlatformThread;

    struct NativeThread final
    {
#if defined(WIN32)
        void* Native[2];
#elif defined(__linux__)
        void* Native[1];
#else
#error "Not implemented"
#endif
    };
}

namespace weave::threading
{
    class Runnable;

    enum class ThreadPriority
    {
        TimeCritical,
        Highest,
        AboveNormal,
        Normal,
        BelowNormal,
        Lower,
        Lowest,
    };

    struct ThreadStart final
    {
        std::optional<std::string_view> Name;
        std::optional<size_t> StackSize;
        std::optional<ThreadPriority> Priority;
        Runnable* Callback;
    };
}

namespace weave::threading
{
    struct ThreadId final
    {
        void* Native;

        [[nodiscard]] constexpr auto operator<=>(ThreadId const&) const = default;
    };

    ThreadId GetThisThreadId();

    class Thread final
    {
    private:
        impl::NativeThread _native;

    private:
        impl::PlatformThread& AsPlatform();
        impl::PlatformThread const& AsPlatform() const;

    public:
        Thread();
        explicit Thread(ThreadStart const& start);
        Thread(Thread&& other);
        Thread& operator=(Thread&& other);
        ~Thread();

        Thread(Thread const&) = delete;
        Thread& operator=(Thread const&) = delete;


    public:
        void Join();

        void Detach();

        [[nodiscard]] bool IsJoinable() const;

        [[nodiscard]] ThreadId GetId() const;
    };
}
