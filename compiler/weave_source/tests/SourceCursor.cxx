#include "weave/platform/compiler.hxx"

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

        REQUIRE_FALSE(cursor.is_end());
        REQUIRE(cursor.is_valid());
        REQUIRE(cursor.first(U'A'));
        REQUIRE(cursor.first(U' '));
        REQUIRE_FALSE(cursor.starts_with(U"37f32"));
        REQUIRE(cursor.first(U'2'));
        REQUIRE(cursor.first(U'1'));
        REQUIRE(cursor.first(U'.'));
        REQUIRE(cursor.starts_with(U"37f32"));

        REQUIRE(cursor.is_end());
        REQUIRE_FALSE(cursor.is_valid());
    }
}
