#include "weave/time/DateTime.hxx"
#include "weave/bugcheck/Assert.hxx"
#include "weave/time/DateTimeOffset.hxx"

#include <array>

WEAVE_EXTERNAL_HEADERS_BEGIN

#if defined(WIN32)
#define NOMINMAX
#include <Windows.h>
#endif

#if defined(__linux__)
#include <sys/time.h>
#endif

WEAVE_EXTERNAL_HEADERS_END

namespace weave::time::impl
{
#if defined(WIN32)
    inline constexpr int64_t DateTimeEpochAdjust = 504911231999999999;
#elif defined(__linux__)
    inline constexpr int64_t DateTimeEpochAdjust = 62135596800;
#else
#error "Not implemented"
#endif

    inline constexpr int64_t TicksPerSecond = NanosecondsInSecond / 100;

    inline constexpr int64_t SecondsInMinute = 60;
    inline constexpr int64_t SecondsInHour = 60 * SecondsInMinute;
    inline constexpr int64_t SecondsInDay = 24 * SecondsInHour;

    inline constexpr int64_t DaysInYear = 365;
    inline constexpr int64_t DaysIn4Years = (DaysInYear * 4) + 1;
    inline constexpr int64_t DaysIn100Years = (DaysIn4Years * 25) - 1;
    inline constexpr int64_t DaysIn400Years = (DaysIn100Years * 4) + 1;

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

    constexpr std::optional<Duration> DateToSeconds(int32_t year, int32_t month, int32_t day)
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
                return Duration{
                    .Seconds = n * SecondsInDay,
                    .Nanoseconds = 0,
                };
            }
        }

        return std::nullopt;
    }
}

namespace weave::time
{
    DateTime DateTime::Now()
    {
#if defined(WIN32)
        SYSTEMTIME st;
        GetLocalTime(&st);

        FILETIME ft;
        SystemTimeToFileTime(&st, &ft);

        int64_t const ticks = std::bit_cast<int64_t>(ft) + impl::DateTimeEpochAdjust;

        return DateTime{
            .Inner = {
                .Seconds = (ticks / impl::TicksPerSecond),
                .Nanoseconds = (ticks % impl::TicksPerSecond) * 100,
            },
            .Kind = DateTimeKind::Local,
        };
#elif defined(__linux__)
        struct timespec ts;

        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        WEAVE_ASSERT(result == 0);

        int64_t const bias = DateTimeOffset::GetCurrentTimeZoneBias().Seconds;

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec - bias + impl::DateTimeEpochAdjust,
                .Nanoseconds = ts.tv_nsec,
            },
            .Kind = DateTimeKind::Local,
        };
#else
#error "Not implemented"
#endif
    }

    DateTime DateTime::UtcNow()
    {
#if defined(WIN32)
        FILETIME ft;
        GetSystemTimePreciseAsFileTime(&ft);

        int64_t const ticks = std::bit_cast<int64_t>(ft) + impl::DateTimeEpochAdjust;

        return DateTime{
            .Inner = {
                .Seconds = (ticks / impl::TicksPerSecond),
                .Nanoseconds = (ticks % impl::TicksPerSecond) * 100,
            },
            .Kind = DateTimeKind::Utc,
        };
#elif defined(__linux__)
        struct timespec ts;

        // Get local time
        [[maybe_unused]] int const result = clock_gettime(CLOCK_REALTIME, &ts);
        WEAVE_ASSERT(result == 0);

        return DateTime{
            .Inner = {
                .Seconds = ts.tv_sec + impl::DateTimeEpochAdjust,
                .Nanoseconds = ts.tv_nsec,
            },
            .Kind = DateTimeKind::Utc,
        };
#else
#error "Not implemented"
#endif
    }

    DateTimeMembers ToMembers(DateTime const& self)
    {
        DateTimeMembers result; // NOLINT(cppcoreguidelines-pro-type-member-init)

        int32_t n = static_cast<int32_t>(self.Inner.Seconds / impl::SecondsInDay);

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

        // Time
        int64_t ns = self.Inner.Nanoseconds / 1000;

        result.Microsecond = static_cast<int32_t>(ns % 1'000);
        ns /= 1'000;
        result.Millisecond = static_cast<int32_t>(ns % 1'000);

        // Sub-second time
        int64_t t = self.Inner.Seconds;

        result.Second = static_cast<int32_t>(t % 60);
        t /= 60;

        result.Minute = static_cast<int32_t>(t % 60);
        t /= 60;

        result.Hour = static_cast<int32_t>(t % 24);

        return result;
    }

    std::optional<DateTime> FromMembers(DateTimeMembers const& members)
    {
        if (std::optional<Duration> const datePart = impl::DateToSeconds(members.Year, members.Month, members.Day))
        {
            DateTime result{
                .Inner = *datePart,
                .Kind = DateTimeKind::Unknown,
            };

            int64_t const hours = members.Hour;
            int64_t const minutes = (hours * 60) + members.Minute;
            int64_t const seconds = (minutes * 60) + members.Second;

            result.Inner.Seconds += seconds;

            int32_t const milliseconds = members.Millisecond;
            int32_t const microseconds = (milliseconds * 1'000) + members.Microsecond;
            int32_t const nanoseconds = (microseconds * 1'000);

            result.Inner.Nanoseconds = nanoseconds;

            if (nanoseconds < 1'000'000'000)
            {
                return result;
            }
        }

        return std::nullopt;
    }

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
}
