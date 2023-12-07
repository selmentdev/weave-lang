#pragma once

namespace weave::threading::impl
{
    struct PlatformRecursiveCriticalSection;

    struct NativeRecursiveCriticalSection
    {
#if defined(WIN32)
        void* Native[5];
#elif defined(__linux__)
        void* Native[6];
#else
#error "Not implemented"
#endif
    };
}

namespace weave::threading
{
    class ConditionVariable;

    class RecursiveCriticalSection final
    {
        friend class ConditionVariable;

    private:
        impl::NativeRecursiveCriticalSection _native;

    private:
        impl::PlatformRecursiveCriticalSection& AsPlatform();

    public:
        RecursiveCriticalSection();
        ~RecursiveCriticalSection();

        RecursiveCriticalSection(RecursiveCriticalSection const&) = delete;
        RecursiveCriticalSection(RecursiveCriticalSection&&) = delete;
        RecursiveCriticalSection& operator=(RecursiveCriticalSection const&) = delete;
        RecursiveCriticalSection& operator=(RecursiveCriticalSection&&) = delete;

    public:
        void Enter();
        bool TryEnter();
        void Leave();
    };
}
