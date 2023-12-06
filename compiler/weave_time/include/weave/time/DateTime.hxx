#pragma once
#include "weave/time/Duration.hxx"

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

    enum class DateTimeKind : uint32_t
    {
        Unknown,
        Local,
        Utc,
    };

    struct DateTime
    {
        Duration Inner;
        DateTimeKind Kind;

        [[nodiscard]] friend constexpr auto operator<=>(DateTime const& self, DateTime const& other) = default;

        [[nodiscard]] static DateTime Now();

        [[nodiscard]] static DateTime UtcNow();
    };

    [[nodiscard]] DateTimeMembers ToMembers(DateTime const& self);

    [[nodiscard]] std::optional<DateTime> FromMembers(DateTimeMembers const& members);

    [[nodiscard]] bool ToString(std::string& result, DateTimeMembers const& value, std::string_view format);

    [[nodiscard]] constexpr DateTime operator+(DateTime const& self, Duration const& other)
    {
        return DateTime{self.Inner + other, self.Kind};
    }

    [[nodiscard]] constexpr DateTime operator-(DateTime const& self, Duration const& other)
    {
        return DateTime{self.Inner - other, self.Kind};
    }

    [[nodiscard]] constexpr Duration operator-(DateTime const& self, DateTime const& other)
    {
        return self.Inner - other.Inner;
    }

    [[nodiscard]] constexpr DateTime& operator+=(DateTime& self, Duration const& other)
    {
        self.Inner += other;
        return self;
    }

    [[nodiscard]] constexpr DateTime& operator-=(DateTime& self, Duration const& other)
    {
        self.Inner -= other;
        return self;
    }
}

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
