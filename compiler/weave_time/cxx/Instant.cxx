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

namespace weave::time
{
    Instant Instant::Now()
    {
#if defined(WIN32)

        static constexpr int64_t QpcFrequency = 10'000'000;

        LARGE_INTEGER counter;
        [[maybe_unused]] BOOL const qpcResult = QueryPerformanceCounter(&counter);
        WEAVE_ASSERT(qpcResult != FALSE);

        // Implementation detail:
        //  QPC Frequency is stored in KUSER_SHARED_DATA.
        //  This function just performs read from that struct on all known platforms.
        LARGE_INTEGER frequency;
        [[maybe_unused]] BOOL const qpfResult = QueryPerformanceFrequency(&frequency);
        WEAVE_ASSERT(qpfResult != FALSE);

        if (frequency.QuadPart == QpcFrequency)
        {
            // 10 MHz frequency is common for known PCs.
            // This way we optimize this part without division.
            return {.Value = counter.QuadPart * (Duration::NanosecondsInSecond / QpcFrequency)};
        }

        // This platform uses some uncommon QPC frequency.
        return {.Value = (counter.QuadPart / frequency.QuadPart)};

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
