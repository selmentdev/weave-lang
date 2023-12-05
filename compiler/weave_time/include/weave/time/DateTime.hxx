#pragma once
#include <cstdint>
#include <compare>
#include <fmt/format.h>

namespace weave::time
{
    enum class DateTimeFormat
    {
        Date,
        Time,
        DateTime,
        File,
        TimeStamp,
    };

    struct DateTimeMembers final
    {
        int32_t Year;
        int32_t Month;
        int32_t Day;
        int32_t Hour;
        int32_t Minute;
        int32_t Second;
        int32_t Millisecond;
        int32_t Microsecond;
        int32_t DayOfYear;
        int32_t DayOfWeek;
    };

    struct TimeSpanMembers final
    {
        bool Negative;
        int32_t Days;
        int32_t Hours;
        int32_t Minutes;
        int32_t Seconds;
        int32_t Milliseconds;
        int32_t Microseconds;
    };
}

namespace weave::time
{
    struct DateTime final
    {
        int64_t Ticks;

        [[nodiscard]] constexpr auto operator<=>(DateTime const&) const = default;

        [[nodiscard]] static DateTime Now();

        [[nodiscard]] static DateTime UtcNow();
    };

    struct TimeSpan final
    {
        int64_t Ticks;

        [[nodiscard]] constexpr auto operator<=>(TimeSpan const&) const = default;
    };

    [[nodiscard]] DateTime FromMembers(DateTimeMembers const& members);

    [[nodiscard]] DateTimeMembers ToMembers(DateTime value);

    [[nodiscard]] TimeSpan FromMembers(TimeSpanMembers const& members);

    [[nodiscard]] TimeSpanMembers ToMembers(TimeSpan value);

    [[nodiscard]] bool ToString(std::string& result, DateTimeMembers const& value, std::string_view format);

    [[nodiscard]] bool ToString(std::string& result, TimeSpanMembers const& value, std::string_view format);
}

namespace weave::time
{
    [[nodiscard]] constexpr DateTime operator+(DateTime const& self, TimeSpan const& other)
    {
        return DateTime{self.Ticks + other.Ticks};
    }

    [[nodiscard]] constexpr DateTime operator-(DateTime const& self, TimeSpan const& other)
    {
        return {.Ticks = self.Ticks - other.Ticks};
    }

    [[nodiscard]] constexpr TimeSpan operator-(DateTime const& self, DateTime const& other)
    {
        return {.Ticks = self.Ticks - other.Ticks};
    }

    [[nodiscard]] constexpr TimeSpan operator+(TimeSpan const& self, TimeSpan const& other)
    {
        return {.Ticks = self.Ticks + other.Ticks};
    }

    [[nodiscard]] constexpr TimeSpan operator-(TimeSpan const& self, TimeSpan const& other)
    {
        return {.Ticks = self.Ticks - other.Ticks};
    }

    [[nodiscard]] constexpr DateTime& operator+=(DateTime& self, TimeSpan const& other)
    {
        self.Ticks += other.Ticks;
        return self;
    }

    [[nodiscard]] constexpr DateTime& operator-=(DateTime& self, TimeSpan const& other)
    {
        self.Ticks -= other.Ticks;
        return self;
    }

    [[nodiscard]] constexpr TimeSpan& operator+=(TimeSpan& self, TimeSpan const& other)
    {
        self.Ticks += other.Ticks;
        return self;
    }

    [[nodiscard]] constexpr TimeSpan& operator-=(TimeSpan& self, TimeSpan const& other)
    {
        self.Ticks -= other.Ticks;
        return self;
    }
}

template <>
struct fmt::formatter<weave::time::TimeSpanMembers>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(weave::time::TimeSpanMembers const& value, auto& context)
    {
        auto out = context.out();

        if (value.Negative)
        {
            (*out++) = '-';
        }

        if (value.Days != 0)
        {
            out = fmt::format_to(out, "{}.", value.Days);
        }

        out = fmt::format_to(out, "{:02}:{:02}:{:02}.{:03}",
            value.Hours,
            value.Minutes,
            value.Seconds,
            value.Milliseconds);

        return out;
    }
};

template <>
struct fmt::formatter<weave::time::TimeSpan> : fmt::formatter<weave::time::TimeSpanMembers>
{
    constexpr auto format(weave::time::TimeSpan const& value, auto& context)
    {
        return fmt::formatter<weave::time::TimeSpanMembers>::format(weave::time::ToMembers(value), context);
    }
};

template <>
struct fmt::formatter<weave::time::DateTimeMembers>
{
private:
    weave::time::DateTimeFormat m_Format = weave::time::DateTimeFormat::DateTime;

public:
    constexpr auto parse(auto& context)
    {
        auto it = context.begin();

        if (it != context.end())
        {
            switch (*it)
            {
            case 'd':
                {
                    ++it;
                    this->m_Format = weave::time::DateTimeFormat::Date;
                    break;
                }

            case 't':
                {
                    ++it;
                    this->m_Format = weave::time::DateTimeFormat::Time;
                    break;
                }

            case 'D':
                {
                    ++it;
                    this->m_Format = weave::time::DateTimeFormat::DateTime;
                    break;
                }

            case 'T':
                {
                    ++it;
                    this->m_Format = weave::time::DateTimeFormat::TimeStamp;
                    break;
                }

            case 'F':
                {
                    ++it;
                    this->m_Format = weave::time::DateTimeFormat::File;
                    break;
                }

            default:
                {
                    break;
                }
            }
        }

        return it;
    }

    constexpr auto format(weave::time::DateTimeMembers const& value, auto& context)
    {
        auto out = context.out();

        switch (this->m_Format)
        {
        case weave::time::DateTimeFormat::Date:
            {
                out = fmt::format_to(out, "{:04}-{:02}-{:02}",
                    value.Year,
                    value.Month,
                    value.Day);
                break;
            }

        case weave::time::DateTimeFormat::Time:
            {
                out = fmt::format_to(out, "{:02}:{:02}:{:02}",
                    value.Hour,
                    value.Minute,
                    value.Second);
                break;
            }

        case weave::time::DateTimeFormat::DateTime:
            {
                out = fmt::format_to(out, "{:04}-{:02}-{:02} {:02}:{:02}:{:02}",
                    value.Year,
                    value.Month,
                    value.Day,
                    value.Hour,
                    value.Minute,
                    value.Second);
                break;
            }

        case weave::time::DateTimeFormat::File:
            {
                out = fmt::format_to(out, "{:04}_{:02}_{:02}_{:02}_{:02}_{:02}_{:03}",
                    value.Year,
                    value.Month,
                    value.Day,
                    value.Hour,
                    value.Minute,
                    value.Second,
                    value.Millisecond);
                break;
            }

        case weave::time::DateTimeFormat::TimeStamp:
            {
                out = fmt::format_to(out, "{:04}.{:02}.{:02}-{:02}.{:02}.{:02}.{:03}",
                    value.Year,
                    value.Month,
                    value.Day,
                    value.Hour,
                    value.Minute,
                    value.Second,
                    value.Millisecond);
                break;
            }
        }

        return out;
    }
};

template <>
struct fmt::formatter<weave::time::DateTime> : fmt::formatter<weave::time::DateTimeMembers>
{
public:
    auto format(weave::time::DateTime const& value, auto& context)
    {
        return fmt::formatter<weave::time::DateTimeMembers>::format(weave::time::ToMembers(value), context);
    }
};
