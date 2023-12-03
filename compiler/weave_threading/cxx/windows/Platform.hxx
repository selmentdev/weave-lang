#pragma once
#include "weave/platform/Compiler.hxx"
#include "weave/time/Duration.hxx"
#include "weave/BugCheck.hxx"

#include <limits>

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::threading::impl
{
    struct PlatformCriticalSection final
    {
        SRWLOCK Native;
    };

    struct PlatformReaderWriterLock final
    {
        SRWLOCK Native;
    };

    struct PlatformSemaphore final
    {
        HANDLE Native;
    };

    struct PlatformConditionVariable final
    {
        CONDITION_VARIABLE Native;
    };

    struct PlatformThread final
    {
        HANDLE Handle;
        DWORD Id;
    };
}

namespace weave::threading::impl
{
    inline DWORD ValidateTimeoutDuration(time::Duration const& value)
    {
        int64_t const milliseconds = value.AsMilliseconds();

        WEAVE_ASSERT((milliseconds >= 0) and (milliseconds <= std::numeric_limits<int32_t>::max()));

        return static_cast<DWORD>(milliseconds);
    }
}
