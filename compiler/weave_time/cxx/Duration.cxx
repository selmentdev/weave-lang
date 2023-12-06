#include "weave/time/Duration.hxx"
#include "weave/BugCheck.hxx"

namespace weave::time
{
    Duration Duration::New(int64_t seconds, int32_t nanoseconds)
    {
        if (seconds > 0)
        {
            WEAVE_ASSERT(nanoseconds >= 0);
        }
        else if (seconds < 0)
        {
            WEAVE_ASSERT(nanoseconds <= 0);
        }
        else
        {
            WEAVE_ASSERT((-impl::NanosecondsInSecond < nanoseconds) and (nanoseconds < impl::NanosecondsInSecond));
        }

        return Duration{seconds, nanoseconds};
    }
}

namespace weave::time
{
    DurationMembers ToMembers(Duration const& self)
    {
        Duration copy{self};

        bool const negative = IsNegative(self);

        if (negative)
        {
            copy = Negate(copy);
        }

        int64_t d = copy.Seconds;
        int32_t const seconds = static_cast<int32_t>(d % 60);
        d /= 60;
        int32_t const minutes = static_cast<int32_t>(d % 60);
        d /= 60;
        int32_t const hours = static_cast<int32_t>(d % 24);
        d /= 24;
        int64_t const days = d;

        int32_t t = copy.Nanoseconds;

        int32_t const nanoseconds = t % 1'000;
        t /= 1'000;
        int32_t const microseconds = t % 1'000;
        t /= 1'000;
        int32_t const milliseconds = t % 1'000;

        return DurationMembers{
            .Days = days,
            .Hours = hours,
            .Minutes = minutes,
            .Seconds = seconds,
            .Milliseconds = milliseconds,
            .Microseconds = microseconds,
            .Nanoseconds = nanoseconds,
            .Negative = negative,
        };
    }

    std::optional<Duration> FromMembers(DurationMembers const& members)
    {
        WEAVE_ASSERT(members.Days >= 0);
        WEAVE_ASSERT(members.Hours >= 0);
        WEAVE_ASSERT(members.Minutes >= 0);
        WEAVE_ASSERT(members.Seconds >= 0);
        WEAVE_ASSERT(members.Milliseconds >= 0);
        WEAVE_ASSERT(members.Microseconds >= 0);

        int64_t const days = members.Days;
        int64_t const hours = (days * 24) + members.Hours;
        int64_t const minutes = (hours * 60) + members.Minutes;
        int64_t const seconds = (minutes * 60) + members.Seconds;

        int32_t const milliseconds = members.Milliseconds;
        int32_t const microseconds = (milliseconds * 1'000) + members.Microseconds;
        int32_t const nanoseconds = (microseconds * 1'000) + members.Nanoseconds;

        if (nanoseconds < impl::NanosecondsInSecond)
        {
            if (members.Negative)
            {
                return Duration{
                    .Seconds = -seconds,
                    .Nanoseconds = -nanoseconds,
                };
            }
            else
            {
                return Duration{
                    .Seconds = seconds,
                    .Nanoseconds = nanoseconds,
                };
            }
        }

        return std::nullopt;
    }

    bool ToString(std::string& result, DurationMembers const& value, std::string_view format)
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
                    // TODO: Enable this code sometime.
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
