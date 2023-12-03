#include "weave/threading/ConditionVariable.hxx"
#include "weave/threading/CriticalSection.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeConditionVariable) >= sizeof(impl::PlatformConditionVariable));
    static_assert(alignof(impl::NativeConditionVariable) >= alignof(impl::PlatformConditionVariable));

    impl::PlatformConditionVariable& ConditionVariable::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformConditionVariable*>(&this->_native);
    }

    ConditionVariable::ConditionVariable()
    {
        InitializeConditionVariable(
            &this->AsPlatform().Native);
    }

    ConditionVariable::~ConditionVariable() = default;

    void ConditionVariable::Wait(CriticalSection& lock)
    {
        SleepConditionVariableSRW(
            &this->AsPlatform().Native,
            &lock.AsPlatform().Native,
            INFINITE, 0);
    }

    bool ConditionVariable::TryWait(CriticalSection& lock, time::Duration const& timeout)
    {
        return SleepConditionVariableSRW(
            &this->AsPlatform().Native,
            &lock.AsPlatform().Native,
            impl::ValidateTimeoutDuration(timeout),
            0) != FALSE;
    }

    void ConditionVariable::Notify()
    {
        WakeConditionVariable(&this->AsPlatform().Native);
    }

    void ConditionVariable::NotifyAll()
    {
        WakeAllConditionVariable(&this->AsPlatform().Native);
    }
}
