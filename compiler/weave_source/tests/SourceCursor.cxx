#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/source/SourceCursor.hxx"

TEST_CASE("Source Cursor")
{
    SECTION("Starts with")
    {
        std::string_view text{"A 21.37f32"};

        weave::source::SourceCursor cursor{text};

        REQUIRE_FALSE(cursor.IsEnd());
        REQUIRE(cursor.IsValid());
        REQUIRE(cursor.First(U'A'));
        REQUIRE(cursor.First(U' '));
        REQUIRE_FALSE(cursor.StartsWith(U"37f32"));
        REQUIRE(cursor.First(U'2'));
        REQUIRE(cursor.First(U'1'));
        REQUIRE(cursor.First(U'.'));
        REQUIRE(cursor.StartsWith(U"37f32"));

        REQUIRE(cursor.IsEnd());
        REQUIRE_FALSE(cursor.IsValid());
    }
}
