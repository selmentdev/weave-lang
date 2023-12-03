#pragma once
#include "weave/time/Duration.hxx"

namespace weave::threading::impl
{
    struct PlatformSemaphore;

    struct NativeSemaphore final
    {
#if defined(WIN32)
        void* Native[1];
#elif defined(__linux__)
        void* Native[4];
#else
#error "Not implemented"
#endif
    };
}

namespace weave::threading
{
    class Semaphore final
    {
    private:
        impl::NativeSemaphore _native;

    private:
        impl::PlatformSemaphore& AsPlatform();

    public:
        explicit Semaphore(int32_t count);
        ~Semaphore();

        Semaphore(Semaphore const&) = delete;
        Semaphore(Semaphore&&) = delete;
        Semaphore& operator=(Semaphore const&) = delete;
        Semaphore& operator=(Semaphore&&) = delete;

    public:
        void Wait();
        bool TryWait(time::Duration const& timeout);
        bool TryWait();
        void Release();
    };
}
