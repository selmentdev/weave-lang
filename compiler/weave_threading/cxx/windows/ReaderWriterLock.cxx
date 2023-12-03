#include "weave/threading/ReaderWriterLock.hxx"

#include "Platform.hxx"

namespace weave::threading
{
    static_assert(sizeof(impl::NativeReaderWriterLock) >= sizeof(impl::PlatformReaderWriterLock));
    static_assert(alignof(impl::NativeReaderWriterLock) >= sizeof(impl::PlatformReaderWriterLock));

    impl::PlatformReaderWriterLock& ReaderWriterLock::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformReaderWriterLock*>(&this->_native);
    }

    ReaderWriterLock::ReaderWriterLock()
    {
        InitializeSRWLock(&this->AsPlatform().Native);
    }

    ReaderWriterLock::~ReaderWriterLock() = default;

    void ReaderWriterLock::EnterRead()
    {
        AcquireSRWLockExclusive(&this->AsPlatform().Native);
    }

    bool ReaderWriterLock::TryEnterRead()
    {
        return TryAcquireSRWLockExclusive(&this->AsPlatform().Native) != FALSE;
    }

    void ReaderWriterLock::LeaveRead()
    {
        ReleaseSRWLockExclusive(&this->AsPlatform().Native);
    }

    void ReaderWriterLock::EnterWrite()
    {
        AcquireSRWLockShared(&this->AsPlatform().Native);
    }

    bool ReaderWriterLock::TryEnterWrite()
    {
        return TryAcquireSRWLockShared(&this->AsPlatform().Native);
    }

    void ReaderWriterLock::LeaveWrite()
    {
        ReleaseSRWLockShared(&this->AsPlatform().Native);
    }
}
