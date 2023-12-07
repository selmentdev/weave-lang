#pragma once
#include "weave/platform/Compiler.hxx"
#include "weave/time/Duration.hxx"
#include "weave/bugcheck/BugCheck.hxx"

#include <limits>

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <pthread.h>
#include <semaphore.h>
#include <time.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::threading::impl
{
    struct PlatformCriticalSection final
    {
        pthread_mutex_t Native;
    };

    struct PlatformRecursiveCriticalSection final
    {
        pthread_mutex_t Native;
    };

    struct PlatformReaderWriterLock final
    {
        pthread_rwlock_t Native;
    };

    struct PlatformSemaphore final
    {
        sem_t Native;
    };

    struct PlatformConditionVariable final
    {
        pthread_cond_t Native;
    };

    struct PlatformThread final
    {
        pthread_t Native;
    };
}

namespace weave::threading::impl
{
    constexpr void Normalize(timespec& self)
    {
        self.tv_sec += (self.tv_nsec / time::impl::NanosecondsInSecond);

        if ((self.tv_nsec %= time::impl::NanosecondsInSecond) < 0)
        {
            self.tv_nsec += time::impl::NanosecondsInSecond;
            --self.tv_sec;
        }
    }

    constexpr void AddDuration(timespec& self, time::Duration const& value) noexcept
    {
        int64_t const nanoseconds = value.ToNanoseconds();

        self.tv_sec += nanoseconds / time::impl::NanosecondsInSecond;
        self.tv_nsec += nanoseconds % time::impl::NanosecondsInSecond;

        Normalize(self);
    }

    inline void ValidateTimeoutDuration(timespec& self, time::Duration const& value) noexcept
    {
        if (value.Seconds < 0)
        {
            WEAVE_BUGCHECK("Duration timeout out of range");
        }

        AddDuration(self, value);
    }

    inline void ValidateTimeoutDuration(timespec& self, int32_t milliseconds) noexcept
    {
        ValidateTimeoutDuration(self, time::Duration::FromMilliseconds(milliseconds));
    }
}
