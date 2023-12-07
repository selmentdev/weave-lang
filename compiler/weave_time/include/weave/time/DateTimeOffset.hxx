#pragma once
#include "weave/time/DateTime.hxx"

namespace weave::time
{
    struct DateTimeOffset
    {
        DateTime Local;
        Duration Bias;

        [[nodiscard]] static Duration GetCurrentTimeZoneBias();

        [[nodiscard]] static DateTimeOffset Now();

        [[nodiscard]] DateTime ToUtc() const
        {
            return DateTime{
                .Inner = this->Local.Inner + this->Bias,
                .Kind = DateTimeKind::Utc,
            };
        }

        [[nodiscard]] DateTime ToLocal() const
        {
            return DateTime{
                .Inner = this->Local.Inner,
                .Kind = DateTimeKind::Local,
            };
        }
    };
}


template <>
struct fmt::formatter<weave::time::DateTimeOffset> : fmt::formatter<weave::time::DateTime>
{
public:
    auto format(weave::time::DateTimeOffset const& value, auto& context)
    {
        auto out =  fmt::formatter<weave::time::DateTime>::format(value.Local, context);
        int64_t const minutes = value.Bias.Seconds / 60;
        return fmt::format_to(out, "{:+03}:{:02}", minutes / 60, minutes % 60);
    }
};
