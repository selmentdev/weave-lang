#pragma once

namespace weave::threading::impl
{
    struct PlatformReaderWriterLock;

    struct NativeReaderWriterLock
    {
#if defined(WIN32)
        void* Native[1];
#elif defined(__linux__)
        void* Native[7];
#else
#error "Not implemented"
#endif
    };
}

namespace weave::threading
{
    class ReaderWriterLock final
    {
    private:
        impl::NativeReaderWriterLock _native;

    private:
        impl::PlatformReaderWriterLock& AsPlatform();

    public:
        ReaderWriterLock();
        ~ReaderWriterLock();

        ReaderWriterLock(ReaderWriterLock const&) = delete;
        ReaderWriterLock(ReaderWriterLock&&) = delete;
        ReaderWriterLock& operator=(ReaderWriterLock const&) = delete;
        ReaderWriterLock& operator=(ReaderWriterLock&&) = delete;

    public:
        void EnterRead();
        bool TryEnterRead();
        void LeaveRead();

        void EnterWrite();
        bool TryEnterWrite();
        void LeaveWrite();
    };
    
    class LockRead final
    {
    private:
        ReaderWriterLock& _lock;

    public:
        LockRead(ReaderWriterLock& lock)
            : _lock(lock)
        {
            _lock.EnterRead();
        }

        ~LockRead()
        {
            _lock.LeaveRead();
        }

        LockRead(LockRead const&) = delete;
        LockRead(LockRead&&) = delete;
        LockRead& operator=(LockRead const&) = delete;
        LockRead& operator=(LockRead&&) = delete;
    };

    class LockWrite final
    {
    private:
        ReaderWriterLock& _lock;

    public:
        LockWrite(ReaderWriterLock& lock)
            : _lock(lock)
        {
            _lock.EnterWrite();
        }

        ~LockWrite()
        {
            _lock.LeaveWrite();
        }

        LockWrite(LockWrite const&) = delete;
        LockWrite(LockWrite&&) = delete;
        LockWrite& operator=(LockWrite const&) = delete;
        LockWrite& operator=(LockWrite&&) = delete;
    };
}
