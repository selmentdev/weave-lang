#pragma once
#include "weave/time/DateTime.hxx"

namespace weave::time
{
    struct DateTimeOffset
    {
        DateTime Local;
        Duration Bias;

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
