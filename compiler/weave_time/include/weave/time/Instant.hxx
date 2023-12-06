#pragma once
#include "weave/time/Duration.hxx"

namespace weave::time
{
    struct Instant
    {
        Duration Inner;

        [[nodiscard]] friend constexpr auto operator<=>(Instant const& self, Instant const& other) = default;

        [[nodiscard]] static Instant Now();

        [[nodiscard]] Duration QueryElapsed() const
        {
            Instant const current = Now();
            return Duration{current.Inner - this->Inner};
        }

        [[nodiscard]] Duration SinceEpoch() const
        {
            return this->Inner;
        }

        [[nodiscard]] Duration DurationSince(Instant const& earlier) const
        {
            return Duration{this->Inner - earlier.Inner};
        }
    };

    [[nodiscard]] constexpr Instant operator+(Instant const& self, Duration const& other)
    {
        return Instant{self.Inner + other};
    }

    [[nodiscard]] constexpr Instant operator-(Instant const& self, Duration const& other)
    {
        return Instant{self.Inner - other};
    }

    [[nodiscard]] constexpr Duration operator-(Instant const& self, Instant const& other)
    {
        return self.Inner - other.Inner;
    }

    constexpr Instant& operator+=(Instant& self, Duration const& other)
    {
        self.Inner += other;
        return self;
    }

    constexpr Instant& operator-=(Instant& self, Duration const& other)
    {
        self.Inner -= other;
        return self;
    }
}
