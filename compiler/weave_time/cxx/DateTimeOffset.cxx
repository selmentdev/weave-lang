#include "weave/time/DateTimeOffset.hxx"
#include "weave/platform/Compiler.hxx"
#include "weave/bugcheck/BugCheck.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#if defined(WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

WEAVE_EXTERNAL_HEADERS_END

namespace weave::time
{
    Duration DateTimeOffset::GetCurrentTimeZoneBias()
    {
#if defined(WIN32)

        DYNAMIC_TIME_ZONE_INFORMATION dtzi;

        int64_t seconds = 0;

        switch (GetDynamicTimeZoneInformation(&dtzi))
        {
        case TIME_ZONE_ID_INVALID:
            WEAVE_BUGCHECK("Cannot obtain timezone ID: {}", GetLastError());

        default:
        case TIME_ZONE_ID_UNKNOWN:
            break;

        case TIME_ZONE_ID_STANDARD:
            seconds += static_cast<int64_t>(dtzi.StandardBias) * 60;
            break;

        case TIME_ZONE_ID_DAYLIGHT:
            seconds += static_cast<int64_t>(dtzi.DaylightBias) * 60;
            break;
        }

        seconds += static_cast<int64_t>(dtzi.Bias) * 60;

        return Duration{
            .Seconds = seconds,
            .Nanoseconds = 0,
        };

#elif defined(__linux__)

        time_t seconds = 0;
        tm tmGMT{};
        tm tmLocal{}; 
        gmtime_r(&seconds, &tmGMT);
        localtime_r(&seconds, &tmLocal);

        return Duration{
            .Seconds = mktime(&tmGMT) - mktime(&tmLocal),
            .Nanoseconds = 0,
        };
#else
#error "Not implemented"
#endif
    }

    DateTimeOffset DateTimeOffset::Now()
    {
        return DateTimeOffset{
            .Local = DateTime::Now(),
            .Bias = DateTimeOffset::GetCurrentTimeZoneBias(),
        };
    }
}
