#include "weave/threading/Yield.hxx"

#include "Platform.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <timeapi.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::threading
{
    void YieldThread()
    {
        if (SwitchToThread() == FALSE)
        {
            SleepEx(0, FALSE);
        }
    }

    void YieldThread(YieldTarget target)
    {
        switch (target)
        {
        case YieldTarget::None:
            {
                break;
            }

        case YieldTarget::AnyThreadOnAnyProcessor:
            {
                timeBeginPeriod(1);
                SleepEx(1, FALSE);
                timeEndPeriod(1);
                break;
            }
        case YieldTarget::SameOrHigherPriorityOnAnyProcessor:
            {
                SleepEx(0, FALSE);
                break;
            }

        case YieldTarget::AnyThreadOnSameProcessor:
            {
                SwitchToThread();
                break;
            }

        default: // NOLINT(clang-diagnostic-covered-switch-default)
            {
                std::unreachable();
            }
        }
    }

    void Sleep(time::Duration const& timeout)
    {
        SleepEx(impl::ValidateTimeoutDuration(timeout), TRUE);
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
