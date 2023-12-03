#include "weave/threading/CriticalSection.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeCriticalSection) >= sizeof(impl::PlatformCriticalSection));
    static_assert(alignof(impl::NativeCriticalSection) >= alignof(impl::PlatformCriticalSection));

    inline impl::PlatformCriticalSection& CriticalSection::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformCriticalSection*>(&this->_native);
    }

    CriticalSection::CriticalSection()
    {
        pthread_mutexattr_t attr{};

        if (int const rc = pthread_mutexattr_init(&attr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutexattr_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutexattr_settype (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutex_init(&this->AsPlatform().Native, &attr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutex_init (rc: {}, `{}`)", rc, strerror(rc));
        }

        if (int const rc = pthread_mutexattr_destroy(&attr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutexattr_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    CriticalSection::~CriticalSection()
    {
        if (int const rc = pthread_mutex_destroy(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutex_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void CriticalSection::Enter()
    {
        if (int const rc = pthread_mutex_lock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutex_lock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool CriticalSection::TryEnter()
    {
        if (int const rc = pthread_mutex_trylock(&this->AsPlatform().Native); rc != 0)
        {
            if (rc == EBUSY)
            {
                return false;
            }
            else
            {
                WEAVE_BUGCHECK("pthread_mutex_trylock (rc: {}, `{}`)", rc, strerror(rc));
            }
        }

        return true;
    }

    void CriticalSection::Leave()
    {
        if (int const rc = pthread_mutex_unlock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutex_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
