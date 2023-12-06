#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/time/DateTime.hxx"

TEST_CASE("DateTime Tests")
{
    using namespace weave::time;

    DateTime const current = *FromMembers(DateTimeMembers{
        .Year = 2023,
        .Month = 11,
        .Day = 13,
        .Hour = 21,
        .Minute = 37,
        .Second = 42,
        .Millisecond = 69,
        .Microsecond = 420,
    });

    CHECK(current.Inner.Seconds == 63835508262);
    CHECK(current.Inner.Nanoseconds == 69420000);
    CHECK(current.Kind == DateTimeKind::Unknown);

    SECTION("DateTime + Duration")
    {
        DateTime const result = current + *FromMembers(DurationMembers{.Hours = 2, .Minutes = 1, .Seconds = 3, .Milliseconds = 7});
        CHECK(result.Inner.Seconds == 63835515525);
        CHECK(result.Inner.Nanoseconds == 76420000);
        CHECK(result.Kind == DateTimeKind::Unknown);

        DateTimeMembers const members = ToMembers(result);
        CHECK(members.Year == 2023);
        CHECK(members.Month == 11);
        CHECK(members.Day == 13);
        CHECK(members.Hour == 23);
        CHECK(members.Minute == 38);
        CHECK(members.Second == 45);
        CHECK(members.Millisecond == 76);
        CHECK(members.Microsecond == 420);
        CHECK(members.DayOfYear == 317);
        CHECK(members.DayOfWeek == 1);
    }

    SECTION("DateTime - Duration")
    {
        DateTime const result = current - *FromMembers(DurationMembers{.Hours = 2, .Minutes = 1, .Seconds = 3, .Milliseconds = 7});
        CHECK(result.Inner.Seconds == 63835500999);
        CHECK(result.Inner.Nanoseconds == 62420000);
        CHECK(result.Kind == DateTimeKind::Unknown);

        DateTimeMembers const members = ToMembers(result);
        CHECK(members.Year == 2023);
        CHECK(members.Month == 11);
        CHECK(members.Day == 13);
        CHECK(members.Hour == 19);
        CHECK(members.Minute == 36);
        CHECK(members.Second == 39);
        CHECK(members.Millisecond == 62);
        CHECK(members.Microsecond == 420);
        CHECK(members.DayOfYear == 317);
        CHECK(members.DayOfWeek == 1);
    }
}
