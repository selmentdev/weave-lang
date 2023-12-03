#include "weave/threading/ReaderWriterLock.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeReaderWriterLock) >= sizeof(impl::PlatformReaderWriterLock));
    static_assert(alignof(impl::NativeReaderWriterLock) >= alignof(impl::PlatformReaderWriterLock));

    impl::PlatformReaderWriterLock& ReaderWriterLock::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformReaderWriterLock*>(&this->_native);
    }

    ReaderWriterLock::ReaderWriterLock()
    {
        if (int const rc = pthread_rwlock_init(&this->AsPlatform().Native, nullptr); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_rwlock_init (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    ReaderWriterLock::~ReaderWriterLock() = default;

    void ReaderWriterLock::EnterRead()
    {
        if (int const rc = pthread_rwlock_rdlock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_rwlock_rdlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnterRead()
    {
        if (int const rc = pthread_rwlock_tryrdlock(&this->AsPlatform().Native); rc == 0)
        {
            return true;
        }
        else if (rc == EBUSY)
        {
            return false;
        }
        else
        {
            WEAVE_BUGCHECK("pthread_rwlock_tryrdlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ReaderWriterLock::LeaveRead()
    {
        if (int const rc = pthread_rwlock_unlock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ReaderWriterLock::EnterWrite()
    {
        if (int const rc = pthread_rwlock_wrlock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_rwlock_wrlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    bool ReaderWriterLock::TryEnterWrite()
    {
        if (int const rc = pthread_rwlock_trywrlock(&this->AsPlatform().Native); rc == 0)
        {
            return true;
        }
        else if (rc == EBUSY)
        {
            return false;
        }
        else
        {
            WEAVE_BUGCHECK("pthread_rwlock_trywrlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }

    void ReaderWriterLock::LeaveWrite()
    {
        if (int const rc  = pthread_rwlock_unlock(&this->AsPlatform().Native); rc != 0)
        {
            WEAVE_BUGCHECK("pthread_rwlock_unlock (rc: {}, `{}`)", rc, strerror(rc));
        }
    }
}
