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
        if (int const rc = pthread_cond_init(&this->AsPlatform().Native, nullptr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_cond_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    ConditionVariable::~ConditionVariable()
    {
        if (int const rc = pthread_cond_destroy(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_cond_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ConditionVariable::Wait(CriticalSection& lock)
    {
        if (int const rc = pthread_cond_wait(&this->AsPlatform().Native, &lock.AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_cond_wait (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ConditionVariable::TryWait(CriticalSection& lock, time::Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        impl::ValidateTimeoutDuration(ts, timeout);

        while (true)
        {
            int const rc = pthread_cond_timedwait(
                   &this->AsPlatform().Native,
                   &lock.AsPlatform().Native,
                   &ts);

            if (rc == 0)
            {
                break;
            }
            else if (rc == ETIMEDOUT)
            {
                return false;
            }
            else
            {
                WEAVE_BUGCHECK("pthread_cond_timedwait (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        return true;
    }

    void ConditionVariable::Notify()
    {
        if (int const rc = pthread_cond_signal(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ConditionVariable::NotifyAll()
    {
        if (int const rc = pthread_cond_broadcast(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_cond_signal (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
