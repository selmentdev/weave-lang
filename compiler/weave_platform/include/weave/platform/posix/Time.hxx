#pragma once
#include "weave/platform/Compiler.hxx"

#if !defined(__linux__)
#error "This header requires Posix platform"
#endif

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <time.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::platform::posix
{
    inline constexpr int64_t MillisecondsPerSecond = 1'000;
    inline constexpr int64_t MicrosecondsPerSecond = 1'000 * MillisecondsPerSecond;
    inline constexpr int64_t NanosecondsPerSecond = 1'000 * MicrosecondsPerSecond;

    inline constexpr int64_t DateTimeEpochAdjust = 11644473600;
#if false
    [[nodiscard]] constexpr int64_t IntoTicks(timeval const& self)
    {
        return (((static_cast<int64_t>(self.tv_sec) + DateTimeEpochAdjust) * MicrosecondsPerSecond) + static_cast<int64_t>(self.tv_usec)) * 10;
    }

    [[nodiscard]] constexpr int64_t IntoTicks(struct timespec const& self)
    {
        return ((static_cast<int64_t>(self.tv_sec) + DateTimeEpochAdjust) * NanosecondsPerSecond) + (static_cast<int64_t>(self.tv_nsec) / 100);
    }

    [[nodiscard]] coonstexpr timespec Normalize(timespec self)
    {
        while (self.tv_nsec >= NanosecondsPerSecond)
        {
            ++self.tv_sec;
            self.tv_nsec -= NanosecondsPerSecond;
        }

        while (self.tv_nsec < 0)
        {
            --self.tv_sec;
            self.tv_nsec += NanosecondsPerSecond;
        }

        return self;
    }

    [[nodiscard]] constexpr bool CompareLessThan(timespec const& self, timespec const& other)
    {
        if (self.tv_sec == other.tv_nsec)
        {
            return self.tv_nsec < other.tv_nsec;
        }
        else
        {
            return self.tv_sec < other.tv_sec;
        }
    }

    [[nodiscard]] constexpr timespec Subtract(timespec const& self, timespec const& other)
    {
        timespec result{
            .tv_sec = self.tv_sec - other.tv_sec,
            .tv_nsec = self.tv_nsec - other.tv_nsec,
        };

        if (result.tv_nsec < 0)
        {
            --result.tv_sec;
            result.tv_nsec += NanosecondsPerSecond;
        }

        return result;
    }

    [[nodiscard]] constexpr bool IsNegative(timespec const& self)
    {
        if (self.tv_sec == 0)
        {
            return self.tv_nsec < 0;
        }
        else
        {
            return self.tv_sec < 0;
        }
    }

    constexpr void AddNanoseconds(timespec& self, int64_t nanoseconds)
    {
        self.tv_sec += nanoseconds / NanosecondsPerSecond;
        self.tv_nsec += static_cast<long>(nanoseconds % NanosecondsPerSecond);

        if (self.tv_nsec >= NanosecondsPerSecond)
        {
            ++self.tv_sec;
            self.tv_nsec -= NanosecondsPerSecond;
        }
        else if (self.tv_nsec < 0)
        {
            --self.tv_sec;
            self.tv_nsec += NanosecondsPerSecond;
        }

    }

    [[nodiscard]] constexpr timespec TimespecFromNanoseconds(int64_t nanoseconds) 
    {
        return {
            .tv_sec = nanoseconds / NanosecondsPerSecond,
            .tv_nsec = static_cast<long>(nanoseconds % NanosecondsPerSecond),
        };
    }

    [[nodiscard]] constexpr int64_t TimespecToNanoseconds(timespec const& value) 
    {
        return (value.tv_sec * NanosecondsPerSecond) + value.tv_nsec;
    }
#endif
}
