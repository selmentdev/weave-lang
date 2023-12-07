#include "weave/threading/Semaphore.hxx"
#include "weave/bugcheck/BugCheck.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeSemaphore) >= sizeof(impl::PlatformSemaphore));
    static_assert(alignof(impl::NativeSemaphore) >= alignof(impl::PlatformSemaphore));

    inline impl::PlatformSemaphore& Semaphore::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformSemaphore*>(&this->_native);
    }

    Semaphore::Semaphore(int32_t count)
    {
        if (sem_init(&this->AsPlatform().Native, 0, static_cast<unsigned int>(count)) != 0)
        {
            WEAVE_BUGCHECK("sem_init (rc: {}, `{}`)", errno, strerror(errno));
        }
    }

    Semaphore::~Semaphore()
    {
        if (sem_destroy(&this->AsPlatform().Native) != 0)
        {
            WEAVE_BUGCHECK("sem_destroy (rc: {}, `{}`)", errno, strerror(errno));
        }
    }


    void Semaphore::Wait()
    {
        while (true)
        {
            int const r = sem_wait(&this->AsPlatform().Native);

            if (r != 0)
            {
                if (errno == EINTR)
                {
                    continue;
                }
                else
                {
                    WEAVE_BUGCHECK("sem_wait (rc: {}, `{}`)", errno, strerror(errno));
                }
            }
            else
            {
                break;
            }
        }
    }

    bool Semaphore::TryWait(time::Duration const& timeout)
    {
        timespec ts{};
        clock_gettime(CLOCK_REALTIME, &ts);
        impl::ValidateTimeoutDuration(ts, timeout);

        while (true)
        {
            int const r = sem_timedwait(&this->AsPlatform().Native, &ts);

            if (r != 0)
            {
                int const error = errno;

                if (error == EINTR)
                {
                    continue;
                }
                else if (error == ETIMEDOUT)
                {
                    return false;
                }
                else
                {
                    WEAVE_BUGCHECK("sem_timedwait (rc: {}, `{}`)", error, strerror(error));
                }
            }
            else
            {
                return true;
            }
        }
    }

    bool Semaphore::TryWait()
    {
        int const r = sem_trywait(&this->AsPlatform().Native);

        if (r == 0)
        {
            return true;
        }
        else
        {
            int const error = errno;

            if (error == EAGAIN || error == EINTR)
            {
                return false;
            }
            else
            {
                WEAVE_BUGCHECK("sem_trywait (rc: {}, `{}`)", error, strerror(error));
            }
        }
    }

    void Semaphore::Release()
    {
        if (sem_post(&this->AsPlatform().Native) != 0)
        {
            WEAVE_BUGCHECK("sem_trywait (rc: {}, `{}`)", errno, strerror(errno));
        }
    };
}
