#pragma once
#include <cstdint>
#include <compare>
#include <optional>
#include <string>
#include <string_view>
#include <fmt/format.h>

namespace weave::time::impl
{
    inline constexpr int64_t NanosecondsInSecond = 1'000'000'000;
    inline constexpr int64_t MicrosecondsInSecond = 1'000'000;
    inline constexpr int64_t MillisecondsInSecond = 1'000;    
}

namespace weave::time
{
    struct DurationMembers final
    {
        int64_t Days;
        int32_t Hours;
        int32_t Minutes;
        int32_t Seconds;
        int32_t Milliseconds;
        int32_t Microseconds;
        int32_t Nanoseconds;
        bool Negative;
    };
}

namespace weave::time
{
    struct Duration
    {
        int64_t Seconds;
        int32_t Nanoseconds;

        [[nodiscard]] friend constexpr auto operator<=>(Duration const& self, Duration const& other) = default;

        [[nodiscard]] static Duration New(int64_t seconds, int32_t nanoseconds);

        [[nodiscard]] constexpr int64_t ToNanoseconds() const
        {
            return (this->Seconds * impl::NanosecondsInSecond) + this->Nanoseconds;
        }

        [[nodiscard]] constexpr int64_t ToMicroseconds() const
        {
            return (this->Seconds * impl::MicrosecondsInSecond) + (this->Nanoseconds / 1'000);
        }

        [[nodiscard]] constexpr int64_t ToMilliseconds() const
        {
            return (this->Seconds * impl::MillisecondsInSecond) + (this->Nanoseconds / 1'000'000);
        }

        [[nodiscard]] static constexpr Duration FromMilliseconds(int64_t value);

        [[nodiscard]] static constexpr Duration FromMicroseconds(int64_t value);

        [[nodiscard]] static constexpr Duration FromNanoseconds(int64_t value);
    };
}

namespace weave::time
{
    constexpr void Normalize(Duration& self)
    {
        self.Seconds += (self.Nanoseconds / impl::NanosecondsInSecond);

        if ((self.Nanoseconds %= impl::NanosecondsInSecond) < 0)
        {
            self.Nanoseconds += impl::NanosecondsInSecond;
            --self.Seconds;
        }
    }

    constexpr Duration Duration::FromMilliseconds(int64_t value)
    {
        Duration result{
            .Seconds = value / 1'000,
            .Nanoseconds = static_cast<int32_t>((value % 1'000) * 1'000'000),
        };

        Normalize(result);
        return result;
    }

    constexpr Duration Duration::FromMicroseconds(int64_t value)
    {
        Duration result{
            .Seconds = value / 1'000'000,
            .Nanoseconds = static_cast<int32_t>((value % 1'000'000) * 1'000),
        };

        Normalize(result);
        return result;
    }

    constexpr Duration Duration::FromNanoseconds(int64_t value)
    {
        Duration result{
            .Seconds = value / 1'000'000'000,
            .Nanoseconds = static_cast<int32_t>(value % 1'000'000'000),
        };

        Normalize(result);
        return result;
    }

    [[nodiscard]] DurationMembers ToMembers(Duration const& self);

    [[nodiscard]] std::optional<Duration> FromMembers(DurationMembers const& members);

    [[nodiscard]] bool ToString(std::string& result, DurationMembers const& value, std::string_view format);

    [[nodiscard]] constexpr Duration Add(Duration const& self, Duration const& other)
    {
        Duration result = {
            .Seconds = self.Seconds + other.Seconds,
            .Nanoseconds = self.Nanoseconds + other.Nanoseconds,
        };

        Normalize(result);

        return result;
    }

    [[nodiscard]] constexpr Duration Subtract(Duration const& self, Duration const& other)
    {
        Duration result{
            .Seconds = self.Seconds - other.Seconds,
            .Nanoseconds = self.Nanoseconds - other.Nanoseconds,
        };

        Normalize(result);

        return result;
    }

    [[nodiscard]] constexpr bool IsNegative(Duration const& self)
    {
        if (self.Seconds == 0)
        {
            return self.Nanoseconds < 0;
        }

        return self.Seconds < 0;
    }

    [[nodiscard]] constexpr Duration Negate(Duration const& self)
    {
        Duration result{
            .Seconds = -self.Seconds,
            .Nanoseconds = -self.Nanoseconds,
        };

        Normalize(result);

        return result;
    }

    [[nodiscard]] constexpr Duration operator+(Duration const& self, Duration const& other)
    {
        return Add(self, other);
    }

    [[nodiscard]] constexpr Duration operator-(Duration const& self, Duration const& other)
    {
        return Subtract(self, other);
    }

    [[nodiscard]] constexpr Duration operator-(Duration const& self)
    {
        return Negate(self);
    }

    constexpr Duration& operator+=(Duration& self, Duration const& other)
    {
        self = Add(self, other);
        return self;
    }

    constexpr Duration& operator-=(Duration& self, Duration const& other)
    {
        self = Subtract(self, other);
        return self;
    }
    
}

template <>
struct fmt::formatter<weave::time::DurationMembers>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(weave::time::DurationMembers const& value, auto& context)
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
struct fmt::formatter<weave::time::Duration> : fmt::formatter<weave::time::DurationMembers>
{
    constexpr auto format(weave::time::Duration const& value, auto& context)
    {
        return fmt::formatter<weave::time::DurationMembers>::format(weave::time::ToMembers(value), context);
    }
};
