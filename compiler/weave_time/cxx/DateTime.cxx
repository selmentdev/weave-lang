#include "weave/time/DateTime.hxx"
#include "weave/platform/Compiler.hxx"
#include "weave/BugCheck.hxx"

#include <optional>
#include <array>

#if defined(WIN32)
#include "weave/platform/windows/Time.hxx"
#endif

WEAVE_EXTERNAL_HEADERS_BEGIN

#if defined(__linux__)
#include "weave/platform/posix/Time.hxx"
#endif

namespace weave::time::impl
{
    inline constexpr int64_t DateTime_TicksInMicrosecond = 10;
    inline constexpr int64_t DateTime_TicksInMillisecond = DateTime_TicksInMicrosecond * 1'000;
    inline constexpr int64_t DateTime_TicksInSecond = DateTime_TicksInMillisecond * 1'000;
    inline constexpr int64_t DateTime_TicksInMinute = DateTime_TicksInSecond * 60;
    inline constexpr int64_t DateTime_TicksInHour = DateTime_TicksInMinute * 60;
    inline constexpr int64_t DateTime_TicksInDay = DateTime_TicksInHour * 24;
    inline constexpr int64_t DateTime_TicksMin = std::numeric_limits<int64_t>::min();
    inline constexpr int64_t DateTime_TicksMax = std::numeric_limits<int64_t>::max();
    inline constexpr int64_t DateTime_MinSeconds = DateTime_TicksMin / DateTime_TicksInSecond;
    inline constexpr int64_t DateTime_MaxSeconds = DateTime_TicksMax / DateTime_TicksInSecond;

    inline constexpr int64_t DaysInYear = 365;
    inline constexpr int64_t DaysIn4Years = (DaysInYear * 4) + 1;
    inline constexpr int64_t DaysIn100Years = (DaysIn4Years * 25) - 1;
    inline constexpr int64_t DaysIn400Years = (DaysIn100Years * 4) + 1;
    inline constexpr int64_t DaysTo1601 = DaysIn400Years * 4;
    inline constexpr int64_t DaysTo1899 = (DaysIn400Years * 4) + (DaysIn100Years * 3) - 367;
    inline constexpr int64_t DaysTo10000 = (DaysIn400Years * 25) - 366;

    inline constexpr std::array<int16_t, 13> DaysBeforeMonth365{{
        0,
        31,
        59,
        90,
        120,
        151,
        181,
        212,
        243,
        273,
        304,
        334,
        365,
    }};

    inline constexpr std::array<int16_t, 13> DaysBeforeMonth366{{
        0,
        31,
        60,
        91,
        121,
        152,
        182,
        213,
        244,
        274,
        305,
        335,
        366,
    }};

    constexpr bool IsLeapYear(int32_t year)
    {
        return (((year % 4) == 0) && ((year % 100) != 0)) || ((year % 400) == 0);
    }

    constexpr std::optional<int64_t> DateToTicks(int32_t year, int32_t month, int32_t day)
    {
        if ((year >= 1) && (year <= 9999) && (month >= 1) && (month <= 12))
        {
            bool const leap = IsLeapYear(year);
            auto const& days_to_month = (leap ? DaysBeforeMonth366 : DaysBeforeMonth365);

            int32_t const days_in_month = days_to_month[static_cast<size_t>(month)] - days_to_month[static_cast<size_t>(month - 1)];

            if ((day >= 1) && (day <= days_in_month))
            {
                int32_t const y = year - 1;
                int32_t const n = (y * 365) + (y / 4) - (y / 100) + (y / 400) + days_to_month[static_cast<size_t>(month - 1)] + day - 1;
                return n * DateTime_TicksInDay;
            }
        }

        return std::nullopt;
    }

}

namespace weave::time
{
    DateTime FromMembers(DateTimeMembers const& members)
    {
        std::optional<int64_t> const ticks_in_date_part = impl::DateToTicks(members.Year, members.Month, members.Day);
        WEAVE_ASSERT(ticks_in_date_part);

        int64_t const full_hours = members.Hour;
        int64_t const full_minutes = (full_hours * 60) + members.Minute;
        int64_t const full_seconds = (full_minutes * 60) + members.Second;
        int64_t const full_milliseconds = (full_seconds * 1'000) + members.Millisecond;
        int64_t const full_microseconds = (full_milliseconds * 1'000) + members.Microsecond;
        int64_t const ticks_in_time_part = full_microseconds * impl::DateTime_TicksInMicrosecond;

        return {.Ticks = ticks_in_date_part.value_or(0) + ticks_in_time_part};
    }

    DateTimeMembers ToMembers(DateTime value)
    {
        DateTimeMembers result; // NOLINT(cppcoreguidelines-pro-type-member-init)

        int32_t n = static_cast<int32_t>(value.Ticks / impl::DateTime_TicksInDay);

        result.DayOfWeek = (n + 1) % 7;

        int32_t const y400 = static_cast<int32_t>(n / impl::DaysIn400Years);

        n -= static_cast<int32_t>(y400 * impl::DaysIn400Years);

        int32_t y100 = static_cast<int32_t>(n / impl::DaysIn100Years);

        if (y100 == 4)
        {
            y100 = 3;
        }

        n -= static_cast<int32_t>(y100 * impl::DaysIn100Years);

        int32_t const y4 = static_cast<int32_t>(n / impl::DaysIn4Years);

        n -= static_cast<int32_t>(y4 * impl::DaysIn4Years);

        int32_t y1 = static_cast<int32_t>(n / impl::DaysInYear);

        if (y1 == 4)
        {
            y1 = 3;
        }

        result.Year = (y400 * 400) + (y100 * 100) + (y4 * 4) + y1 + 1;

        n -= static_cast<int32_t>(y1 * impl::DaysInYear);

        result.DayOfYear = n + 1;

        bool const leap = (y1 == 3 && (y4 != 24 || y100 == 3));

        auto const& daysToMonth = (leap ? impl::DaysBeforeMonth366 : impl::DaysBeforeMonth365);

        int32_t m = (n >> 5) + 1;

        while (n >= daysToMonth[static_cast<size_t>(m)])
        {
            ++m;
        }

        result.Month = m;

        result.Day = n - daysToMonth[static_cast<size_t>(m - 1)] + 1;

        int64_t t = value.Ticks / impl::DateTime_TicksInMicrosecond;

        result.Microsecond = static_cast<int32_t>(t % 1'000);
        t /= 1'000;

        result.Millisecond = static_cast<int32_t>(t % 1'000);
        t /= 1'000;

        result.Second = static_cast<int32_t>(t % 60);
        t /= 60;

        result.Minute = static_cast<int32_t>(t % 60);
        t /= 60;

        result.Hour = static_cast<int32_t>(t % 24);

        return result;
    }

    TimeSpan FromMembers(TimeSpanMembers const& members)
    {
        int64_t const full_hours = members.Hours;
        int64_t const full_minutes = (full_hours * 60) + members.Minutes;
        int64_t const full_seconds = (full_minutes * 60) + members.Seconds;
        int64_t const full_milliseconds = (full_seconds * 1'000) + members.Milliseconds;
        int64_t const full_microseconds = (full_milliseconds * 1'000) + members.Microseconds;
        int64_t const full_ticks_in_time = full_microseconds * impl::DateTime_TicksInMicrosecond;

        int64_t const full_ticks_in_days = impl::DateTime_TicksInDay * members.Days;
        int64_t biased_ticks = full_ticks_in_time + full_ticks_in_days;

        [[maybe_unused]] bool overflow{};

        if (members.Days > 0)
        {
            if (full_ticks_in_time < 0)
            {
                overflow = (full_ticks_in_time > biased_ticks);
            }
            else
            {
                overflow = (biased_ticks < 0);
            }
        }
        else if (members.Days < 0)
        {
            if (full_ticks_in_time <= 0)
            {
                overflow = (biased_ticks > 0);
            }
            else
            {
                overflow = (full_ticks_in_time > biased_ticks);
            }
        }

        WEAVE_ASSERT(!overflow);


        if (members.Negative)
        {
            biased_ticks = -biased_ticks;
        }

        return {.Ticks = biased_ticks};
    }

    TimeSpanMembers ToMembers(TimeSpan value)
    {
        int64_t const ticks = std::abs(value.Ticks);

        int32_t const days = static_cast<int32_t>(ticks / impl::DateTime_TicksInDay);

        return TimeSpanMembers{
            .Negative = value.Ticks < 0,
            .Days = days,
            .Hours = static_cast<int32_t>((ticks % impl::DateTime_TicksInDay) / impl::DateTime_TicksInHour),
            .Minutes = static_cast<int32_t>((ticks % impl::DateTime_TicksInHour) / impl::DateTime_TicksInMinute),
            .Seconds = static_cast<int32_t>((ticks % impl::DateTime_TicksInMinute) / impl::DateTime_TicksInSecond),
            .Milliseconds = static_cast<int32_t>((ticks % impl::DateTime_TicksInSecond) / impl::DateTime_TicksInMillisecond),
            .Microseconds = static_cast<int32_t>((ticks % impl::DateTime_TicksInMillisecond) / impl::DateTime_TicksInMicrosecond),
        };
    }

    DateTime DateTime::UtcNow()
    {
#if defined(WIN32)
        FILETIME ft{};
        GetSystemTimeAsFileTime(&ft);
        return DateTime{.Ticks = platform::windows::IntoTicks(ft)};
#elif defined(__linux__)
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return DateTime{.Ticks = platform::posix::IntoTicks(ts)};
#else
#error "Not implemented"
#endif
    }

    DateTime DateTime::Now()
    {
#if defined(WIN32)
        SYSTEMTIME st;
        GetLocalTime(&st);

        FILETIME ft;
        SystemTimeToFileTime(&st, &ft);

        return DateTime{.Ticks = platform::windows::IntoTicks(ft)};
#elif defined(__linux__)
#error "Not implemented"
#else
#error "Not implemented"
#endif
    }

    DateTime ToLocal(DateTime value)
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

        return DateTime{.Ticks = value.Ticks - (seconds * impl::DateTime_TicksInSecond)};
#elif defined(__linux__)
        return value;
#error "Not implemented"
#else
#error "Not implemented"
#endif
    }
}


namespace weave::time
{
    bool ToString(std::string& result, DateTimeMembers const& value, std::string_view format)
    {
        bool const morning = value.Hour < 12;

        bool percent = false;

        fmt::memory_buffer buffer{};
        auto out = std::back_inserter(buffer);

        for (char const ch : format)
        {
            if (ch == '%')
            {
                if (percent)
                {
                    (*out++) = '%';
                    percent = false;
                }
                else
                {
                    percent = true;
                    continue;
                }
            }

            if (percent)
            {
                percent = false;

                switch (ch)
                {
                case 'a':
                    {
                        const char* ampm = morning ? "am" : "pm";

                        out = std::copy(ampm, ampm + 2, out);
                        break;
                    }

                case 'A':
                    {
                        const char* ampm = morning ? "AM" : "PM";

                        out = std::copy(ampm, ampm + 2, out);
                        break;
                    }

                case 'd':
                    {
                        out = fmt::format_to(out, "{:02}", value.Day);
                        break;
                    }

                case 'D':
                    {
                        out = fmt::format_to(out, "{:03}", value.DayOfYear);
                        break;
                    }

                case 'm':
                    {
                        out = fmt::format_to(out, "{:02}", value.Month);
                        break;
                    }

                case 'y':
                    {
                        out = fmt::format_to(out, "{:02}", value.Year % 100);
                        break;
                    }

                case 'Y':
                    {
                        out = fmt::format_to(out, "{}", value.Year);
                        break;
                    }

                case 'h':
                    {
                        out = fmt::format_to(out, "{:02}", value.Hour % 12);
                        break;
                    }
                case 'H':
                    {
                        out = fmt::format_to(out, "{:02}", value.Hour);
                        break;
                    }
                case 'M':
                    {
                        out = fmt::format_to(out, "{:02}", value.Minute);
                        break;
                    }
                case 'S':
                    {
                        out = fmt::format_to(out, "{:02}", value.Second);
                        break;
                    }
                case 's':
                    {
                        out = fmt::format_to(out, "{:03}", value.Millisecond);
                        break;
                    }
                default:
                    {
                        WEAVE_ASSERT(false, "Unknown format specified: {:c}", ch);
                        (*out++) = ch;
                        break;
                    }
                }
            }
            else
            {
                (*out++) = ch;
            }
        }

        WEAVE_ASSERT(percent == false, "Unterminated string. Percent isgn without format specifier");

        result.assign(buffer.data(), buffer.size());
        return true;
    }

    bool ToString(std::string& result, TimeSpanMembers const& value, std::string_view format)
    {
        bool percent = false;

        fmt::memory_buffer buffer{};
        auto out = std::back_inserter(buffer);


        for (const char ch : format)
        {
            if (ch == '%')
            {
                percent = true;
                continue;
            }

            if (percent)
            {
                percent = false;

                switch (ch)
                {
                case 'n':
                    {
                        if (value.Negative)
                        {
                            (*out++) = '-';
                        }
                        break;
                    }
                case 'N':
                    {
                        (*out++) = value.Negative ? '-' : '+';
                        break;
                    }
                case 'd':
                    {
                        out = fmt::format_to(out, "{}", value.Days);
                        break;
                    }
                case 'h':
                    {
                        out = fmt::format_to(out, "{:02d}", value.Hours);
                        break;
                    }
                case 'm':
                    {
                        out = fmt::format_to(out, "{:02d}", value.Minutes);
                        break;
                    }
                case 's':
                    {
                        out = fmt::format_to(out, "{:02d}", value.Seconds);
                        break;
                    }
                case 'f':
                    {
                        out = fmt::format_to(out, "{:03d}", value.Milliseconds);
                        break;
                    }
#if false
                case 'D':
                    {
                        out = fmt::format_to(out, "{}", std::abs(value.GetTotalDays()));
                        break;
                    }
                case 'H':
                    {
                        out = fmt::format_to(out, "{:02d}", std::abs(value.GetTotalHours()));
                        break;
                    }
                case 'M':
                    {
                        out = fmt::format_to(out, "{:02d}", std::abs(value.GetTotalMinutes()));
                        break;
                    }
                case 'S':
                    {
                        out = fmt::format_to(out, "{:02d}", std::abs(value.GetTotalSeconds()));
                        break;
                    }
                case 'F':
                    {
                        out = fmt::format_to(out, "{:03d}", std::abs(value.GetTotalMilliseconds()));
                        break;
                    }
#endif
                default:
                    {
                        WEAVE_ASSERT(false, "Unknown format specified: {:c}", ch);

                        (*out++) = ch;
                        break;
                    }
                }
            }
            else
            {
                buffer.push_back(ch);
            }
        }

        WEAVE_ASSERT(percent == false, "Unterminated string. Percent isgn without format specifier");

        result.assign(buffer.data(), buffer.size());
        return true;
    }
}
