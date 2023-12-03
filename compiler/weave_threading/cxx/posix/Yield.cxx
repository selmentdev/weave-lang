#include "weave/threading/Yield.hxx"

#include "Platform.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#if defined(__x86_64__)
#include <xmmintrin.h>
#endif

#include <sched.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::threading
{
    void YieldThread()
    {
        sched_yield();
    }

    void YieldThread(YieldTarget target)
    {
        sched_yield();
        (void)target;
    }

    void Sleep(time::Duration const& timeout)
    {
        usleep(static_cast<useconds_t>(timeout.AsMicroseconds()));
    }

    void Pause()
    {
#if defined(_M_AMD64) || defined(__x86_64__)
        _mm_pause();
        _mm_pause();
        _mm_pause();
        _mm_pause();
#elif defined(_M_ARM64)
        __wfe();
#elif defined(__aarch64__)
        __asm__ __volatile__("wfe");
#else
#error "Not implemented"
#endif
    }
}
