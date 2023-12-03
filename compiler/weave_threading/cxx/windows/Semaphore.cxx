#include "weave/threading/Semaphore.hxx"

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
        this->AsPlatform().Native = CreateSemaphoreW(
            nullptr,
            count,
            INT32_MAX,
            nullptr);
    }

    Semaphore::~Semaphore()
    {
        CloseHandle(
            this->AsPlatform().Native);
    }


    void Semaphore::Wait()
    {
        WaitForSingleObject(
            this->AsPlatform().Native,
            INFINITE);
    }

    bool Semaphore::TryWait(time::Duration const& timeout)
    {
        return WaitForSingleObject(
                   this->AsPlatform().Native,
                   impl::ValidateTimeoutDuration(timeout)) != WAIT_TIMEOUT;
    }

    bool Semaphore::TryWait()
    {
        return WaitForSingleObject(
                   this->AsPlatform().Native,
                   0) != WAIT_TIMEOUT;
    }

    void Semaphore::Release()
    {
        ReleaseSemaphore(
            this->AsPlatform().Native,
            1,
            nullptr);
    };
}
