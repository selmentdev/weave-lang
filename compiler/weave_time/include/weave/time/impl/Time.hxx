#pragma once

#if defined(WIN32)

#include "weave/platform/windows/PlatformHeaders.hxx"

#elif defined(__linux__)
WEAVE_EXTERNAL_HEADERS_BEGIN
#include <sys/time.h>
WEAVE_EXTERNAL_HEADERS_END
#else
#error "Not implemented"
#endif

#if defined(WIN32)

namespace weave::time::impl
{
    inline constexpr int64_t DateTimeEpochAdjust = 504911231999999999;
    inline constexpr int64_t TicksPerSecond = NanosecondsInSecond / 100;

    constexpr DateTime FromNative(FILETIME const& ft, DateTimeKind kind)
    {
        int64_t const ticks = std::bit_cast<int64_t>(ft) + impl::DateTimeEpochAdjust;

        return DateTime{
            .Inner = {
                .Seconds = (ticks / impl::TicksPerSecond),
                .Nanoseconds = (ticks % impl::TicksPerSecond) * 100,
            },
            .Kind = kind,
        };
    }
}

#endif

#if defined(__linux__)
namespace weave::time::impl
{
    inline constexpr int64_t DateTimeEpochAdjust = 62135596800;

    constexpr DateTime FromNative(struct timespec const& ts, DateTimeKind kind)
    {
        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec + DateTimeEpochAdjust,
                .Nanoseconds = ts.tv_nsec,
            },
            .Kind = kind,
        };
    }

    constexpr DateTime FromNative(struct timespec const& ts, int64_t bias, DateTimeKind kind)
    {
        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + DateTimeEpochAdjust,
                .Nanoseconds = ts.tv_nsec,
            },
            .Kind = kind,
        };
    }
}
#endif
