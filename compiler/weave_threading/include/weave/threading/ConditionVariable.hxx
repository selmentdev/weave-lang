#pragma once
#include "weave/time/Duration.hxx"

namespace weave::threading::impl
{
    struct PlatformConditionVariable;

    struct NativeConditionVariable
    {
#if defined(WIN32)
        void* Native[1];
#else
#error "Not implemented"
#endif
    };
}

namespace weave::threading
{
    class CriticalSection;

    class ConditionVariable final
    {
    private:
        impl::NativeConditionVariable _native;

    private:
        impl::PlatformConditionVariable& AsPlatform();

    public:
        ConditionVariable();
        ~ConditionVariable();

        ConditionVariable(ConditionVariable const&) = delete;
        ConditionVariable(ConditionVariable&&) = delete;
        ConditionVariable& operator=(ConditionVariable const&) = delete;
        ConditionVariable& operator=(ConditionVariable&&) = delete;

    public:
        void Wait(CriticalSection& lock);
        bool TryWait(CriticalSection& lock, time::Duration const& timeout);
        void Notify();
        void NotifyAll();
    };
}
