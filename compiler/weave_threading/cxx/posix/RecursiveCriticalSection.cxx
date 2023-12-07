#include "weave/threading/RecursiveCriticalSection.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeRecursiveCriticalSection) >= sizeof(impl::PlatformRecursiveCriticalSection));
    static_assert(alignof(impl::NativeRecursiveCriticalSection) >= alignof(impl::PlatformRecursiveCriticalSection));

    inline impl::PlatformRecursiveCriticalSection& RecursiveCriticalSection::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformRecursiveCriticalSection*>(&this->_native);
    }

    RecursiveCriticalSection::RecursiveCriticalSection()
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

    RecursiveCriticalSection::~RecursiveCriticalSection()
    {
        if (int const rc = pthread_mutex_destroy(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutex_destroy (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void RecursiveCriticalSection::Enter()
    {
        if (int const rc = pthread_mutex_lock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutex_lock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool RecursiveCriticalSection::TryEnter()
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

    void RecursiveCriticalSection::Leave()
    {
        if (int const rc = pthread_mutex_unlock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_mutex_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
