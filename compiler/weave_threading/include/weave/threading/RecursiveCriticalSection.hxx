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

    public:
        class Lock final
        {
        private:
            RecursiveCriticalSection& _lock;

        public:
            Lock(RecursiveCriticalSection& lock)
                : _lock(lock)
            {
                _lock.Enter();
            }

            ~Lock()
            {
                _lock.Leave();
            }

            Lock(Lock const&) = delete;
            Lock(Lock&&) = delete;
            Lock& operator=(Lock const&) = delete;
            Lock& operator=(Lock&&) = delete;
        };
    };
}
