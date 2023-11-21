#pragma once
#include <cstdint>

namespace weave::time
{
    struct Duration final
    {
    public:
        inline static constexpr int64_t NanosecondsInMicrosecond = 1'000;
        inline static constexpr int64_t NanosecondsInMillisecond = 1'000 * NanosecondsInMicrosecond;
        inline static constexpr int64_t NanosecondsInSecond = 1'000 * NanosecondsInMillisecond;

    public:
        int64_t Value;

    public:
        [[nodiscard]] static constexpr Duration FromNanoseconds(int64_t value)
        {
            return {.Value = value};
        }

        [[nodiscard]] static constexpr Duration FromMicroseconds(int64_t value)
        {
            return {.Value = value * NanosecondsInMicrosecond};
        }

        [[nodiscard]] static constexpr Duration FromMilliseconds(int64_t value)
        {
            return {.Value = value * NanosecondsInMillisecond};
        }

        [[nodiscard]] static constexpr Duration FromSeconds(int64_t value)
        {
            return {.Value = value * NanosecondsInSecond};
        }

    public:
        [[nodiscard]] constexpr int64_t AsNanoseconds() const
        {
            return this->Value;
        }

        [[nodiscard]] constexpr int64_t AsMicroseconds() const
        {
            return this->Value / NanosecondsInMicrosecond;
        }

        [[nodiscard]] constexpr int64_t AsMilliseconds() const
        {
            return this->Value / NanosecondsInMillisecond;
        }

        [[nodiscard]] constexpr int64_t AsSeconds() const
        {
            return this->Value / NanosecondsInSecond;
        }

    public:
        [[nodiscard]] constexpr auto operator<=>(Duration const& other) const = default;
    };
}

namespace weave::time
{
    [[nodiscard]] constexpr Duration operator+(Duration const& self, Duration const& other)
    {
        return {.Value = self.Value + other.Value};
    }

    [[nodiscard]] constexpr Duration operator-(Duration const& self, Duration const& other)
    {
        return {.Value = self.Value - other.Value};
    }

    constexpr Duration& operator+=(Duration& self, Duration const& other)
    {
        self.Value += other.Value;
        return self;
    }

    constexpr Duration& operator-=(Duration& self, Duration const& other)
    {
        self.Value -= other.Value;
        return self;
    }
}
