#include "weave/platform/Compiler.hxx"
#include "weave/BugCheck.hxx"
#include "weave/time/Instant.hxx"
#include "weave/time/Duration.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#if defined(WIN32)

#define NOMINMAX
#include <Windows.h>

#elif defined(__linux__)

#include <sys/mman.h>

#else
#error Not implemented
#endif

WEAVE_EXTERNAL_HEADERS_END

namespace weave::time::impl
{
    static int64_t QueryInstantFrequency()
    {
        static std::atomic<int64_t> cache{};

        int64_t result = cache.load(std::memory_order::relaxed);

        if (result != 0)
        {
            return result;
        }

        // Implementation detail:
        //  QPC Frequency is stored in KUSER_SHARED_DATA.
        //  This function just performs read from that struct on all known platforms.
        LARGE_INTEGER value;
        [[maybe_unused]] BOOL const success = QueryPerformanceFrequency(&value);
        WEAVE_ASSERT(success != FALSE);

        result = std::bit_cast<int64_t>(value);
        cache.store(result, std::memory_order::relaxed);
        return result;
    }    
}

namespace weave::time
{
    Instant Instant::Now()
    {
#if defined(WIN32)
        static constexpr int64_t CommonQpcFrequency = 10'000'000;

        LARGE_INTEGER counter;
        [[maybe_unused]] BOOL const qpcResult = QueryPerformanceCounter(&counter);
        WEAVE_ASSERT(qpcResult != FALSE);

        int64_t const value = std::bit_cast<int64_t>(counter);

        if (int64_t const frequency = impl::QueryInstantFrequency(); frequency == CommonQpcFrequency)
        {
            return Instant{
                .Inner = {
                    .Seconds = value / CommonQpcFrequency,
                    .Nanoseconds = static_cast<int32_t>((value % CommonQpcFrequency) * 100),
                },
            };
        }
        else
        {
            int64_t const nanosecond_conversion = impl::NanosecondsInSecond / frequency;
            return Instant{
                .Inner = {
                    .Seconds = value / frequency,
                    .Nanoseconds = static_cast<int32_t>((value % frequency) * nanosecond_conversion),
                },
            };
        }

#elif defined(__linux__)

#if defined(HAVE_CLOCK_GETTIME_NSEC_NP)

        return {.Value = clock_gettime_nsec_np(CLOCK_UPTIME_RAW)};

#else

        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_MONOTONIC, &ts);
        WEAVE_ASSERT(result == 0);

        return {.Value = (static_cast<int64_t>(ts.tv_sec) * Duration::NanosecondsInSecond) + static_cast<int64_t>(ts.tv_nsec)};

#endif

#else
#error Not implemented
#endif
    }
}
