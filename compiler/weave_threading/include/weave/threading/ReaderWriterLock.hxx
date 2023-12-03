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
}
