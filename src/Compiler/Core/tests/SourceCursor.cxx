#include <catch_amalgamated.hpp>

#include "Compiler.Core/SourceCursor.hxx"

TEST_CASE("Source Cursor")
{
    SECTION("Starts with")
    {
        std::string_view text{"A 21.37f32"};

        Weave::SourceCursor cursor{text};

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
