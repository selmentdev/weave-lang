#pragma once
#include "weave/time/Duration.hxx"

namespace weave::time
{
    struct Instant final
    {
        int64_t Value;

        [[nodiscard]] static Instant Now();

        [[nodiscard]] Duration QueryElapsed() const
        {
            Instant const current = Now();
            return Duration{current.Value - this->Value};
        }

        [[nodiscard]] Duration SinceEpoch() const
        {
            return Duration{this->Value};
        }

        [[nodiscard]] constexpr auto operator<=>(Instant const& other) const = default;
    };
}


namespace weave::time
{
    [[nodiscard]] constexpr Instant operator+(Instant const& self, Duration const& other)
    {
        return {.Value = self.Value + other.Value};
    }

    [[nodiscard]] constexpr Instant operator-(Instant const& self, Duration const& other)
    {
        return {.Value = self.Value - other.Value};
    }

    [[nodiscard]] constexpr Duration operator-(Instant const& self, Instant const& other)
    {
        return {.Value = self.Value - other.Value};
    }

    constexpr Instant& operator+=(Instant& self, Duration const& other)
    {
        self.Value += other.Value;
        return self;
    }

    constexpr Instant& operator-=(Instant& self, Duration const& other)
    {
        self.Value -= other.Value;
        return self;
    }
}
