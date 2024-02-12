#pragma once

namespace weave::threading::impl
{
    struct PlatformCriticalSection;

    struct NativeCriticalSection
    {
#if defined(WIN32)
        void* Native[1];
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

    class CriticalSection final
    {
        friend class ConditionVariable;

    private:
        impl::NativeCriticalSection _native;

    private:
        impl::PlatformCriticalSection& AsPlatform();

    public:
        CriticalSection();
        ~CriticalSection();

        CriticalSection(CriticalSection const&) = delete;
        CriticalSection(CriticalSection&&) = delete;
        CriticalSection& operator=(CriticalSection const&) = delete;
        CriticalSection& operator=(CriticalSection&&) = delete;

    public:
        void Enter();
        bool TryEnter();
        void Leave();

    public:
        class Lock final
        {
        private:
            CriticalSection& _lock;

        public:
            Lock(CriticalSection& lock)
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
