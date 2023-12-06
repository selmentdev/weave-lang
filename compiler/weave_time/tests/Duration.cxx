#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/time/Duration.hxx"

TEST_CASE("Duration Tests")
{
    using namespace weave::time;

    SECTION("From Members")
    {
        DurationMembers const members{
            .Days = 1,
            .Hours = 2,
            .Minutes = 3,
            .Seconds = 4,
            .Milliseconds = 999,
            .Microseconds = 888,
            .Nanoseconds = 777,
        };

        std::optional<Duration> const result = FromMembers(members);
        REQUIRE(result.has_value());
        CHECK(result->Seconds == 93784);
        CHECK(result->Nanoseconds == 999888777);
    }

    SECTION("To Members")
    {
        Duration const source{
            .Seconds = 93784,
            .Nanoseconds = 999888777,
        };

        DurationMembers const result = ToMembers(source);

        CHECK(result.Negative == false);

        CHECK(result.Days == 1);
        CHECK(result.Hours == 2);
        CHECK(result.Minutes == 3);
        CHECK(result.Seconds == 4);
        CHECK(result.Milliseconds == 999);
        CHECK(result.Microseconds == 888);
        CHECK(result.Nanoseconds == 777);
    }

    SECTION("Arithmetic")
    {
        SECTION("Negation")
        {
            DurationMembers const members{
                .Days = 1,
                .Hours = 2,
                .Minutes = 3,
                .Seconds = 4,
                .Milliseconds = 999,
                .Microseconds = 888,
                .Nanoseconds = 777,
            };

            std::optional<Duration> const value = FromMembers(members);
            REQUIRE(value);
            CHECK(value->Seconds == 93784);
            CHECK(value->Nanoseconds == 999888777);

            Duration const negative = Negate(*value);
            CHECK(negative.Seconds == -(value->Seconds + 1));
            CHECK(negative.Nanoseconds == (impl::NanosecondsInSecond - value->Nanoseconds));

            {
                Duration const positive = Negate(negative);
                CHECK(positive.Seconds == value->Seconds);
                CHECK(positive.Nanoseconds == value->Nanoseconds);
            }

            DurationMembers const negativeResult = ToMembers(negative);

            CHECK(negativeResult.Negative == true);

            CHECK(negativeResult.Days == 1);
            CHECK(negativeResult.Hours == 2);
            CHECK(negativeResult.Minutes == 3);
            CHECK(negativeResult.Seconds == 4);
            CHECK(negativeResult.Milliseconds == 999);
            CHECK(negativeResult.Microseconds == 888);
            CHECK(negativeResult.Nanoseconds == 777);
        }


        SECTION("Simple")
        {
            Duration const a = *FromMembers(DurationMembers{.Seconds = 2, .Milliseconds = 900});
            Duration const b = *FromMembers(DurationMembers{.Seconds = 1, .Milliseconds = 800});

            SECTION("(b - a) + a")
            {
                Duration const result = (b - a) + a;
                CHECK(result == b);
            }

            SECTION("(a - b) + b")
            {
                Duration const result = (a - b) + b;
                CHECK(result == a);
            }

            SECTION("(b + a) - a")
            {
                Duration const result = (b + a) - a;
                CHECK(result == b);
            }

            SECTION("(a + b) - b")
            {
                Duration const result = (a + b) - b;
                CHECK(result == a);
            }

            SECTION("a - b")
            {
                Duration const result = a - b;
                CHECK(result.Seconds == 1);
                CHECK(result.Nanoseconds == 100'000'000);
            }

            SECTION("b - a")
            {
                Duration const result = b - a;
                CHECK(result.Seconds == -2);
                CHECK(result.Nanoseconds == 900'000'000);
            }

            SECTION("a - (-b)")
            {
                Duration const result = a - (-b);
                CHECK(result.Seconds == 4);
                CHECK(result.Nanoseconds == 700'000'000);
            }

            SECTION("b - (-a)")
            {
                Duration const result = b - (-a);
                CHECK(result.Seconds == 4);
                CHECK(result.Nanoseconds == 700'000'000);
            }

            SECTION("a + b")
            {
                Duration const result = a + b;
                CHECK(result.Seconds == 4);
                CHECK(result.Nanoseconds == 700'000'000);
            }

            SECTION("b + a")
            {
                Duration const result = b + a;
                CHECK(result.Seconds == 4);
                CHECK(result.Nanoseconds == 700'000'000);
            }

            SECTION("a + (-b)")
            {
                Duration const result = a + (-b);
                CHECK(result.Seconds == 1);
                CHECK(result.Nanoseconds == 100'000'000);
            }

            SECTION("b + (-a)")
            {
                Duration const result = b + (-a);
                CHECK(result.Seconds == -2);
                CHECK(result.Nanoseconds == 900'000'000);
            }
        }
    }

    SECTION("Total values")
    {
        SECTION("From Milliseconds")
        {
            Duration const positive = Duration::FromMilliseconds(89'123);
            CHECK(positive.Seconds == 89);
            CHECK(positive.Nanoseconds == 123'000'000);

            Duration const negative = Duration::FromMilliseconds(-89'123);
            CHECK(negative.Seconds == -90);
            CHECK(negative.Nanoseconds == 877'000'000);
        }

        SECTION("From Microseconds")
        {
            Duration const positive = Duration::FromMicroseconds(89'123'456);
            CHECK(positive.Seconds == 89);
            CHECK(positive.Nanoseconds == 123'456'000);

            Duration const negative = Duration::FromMicroseconds(-89'123'456);
            CHECK(negative.Seconds == -90);
            CHECK(negative.Nanoseconds == 876'544'000);
        }

        SECTION("From Nanoseconds")
        {
            Duration const positive = Duration::FromNanoseconds(89'123'456'789);
            CHECK(positive.Seconds == 89);
            CHECK(positive.Nanoseconds == 123'456'789);

            Duration const negative = Duration::FromNanoseconds(-89'123'456'789);
            CHECK(negative.Seconds == -90);
            CHECK(negative.Nanoseconds == 876'543'211
);
        }
    }
}
