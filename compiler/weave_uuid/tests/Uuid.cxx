#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/Uuid.hxx"

TEST_CASE("Uuid from bytes")
{
    using namespace weave::uuid;

    constexpr uint8_t bytes[]{
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17};

    SECTION("Smaller")
    {
        auto result = FromBytes(std::as_bytes(std::span{bytes, 10}));

        CHECK(result.Elements[0] == bytes[0]);
        CHECK(result.Elements[1] == bytes[1]);
        CHECK(result.Elements[2] == bytes[2]);
        CHECK(result.Elements[3] == bytes[3]);
        CHECK(result.Elements[4] == bytes[4]);
        CHECK(result.Elements[5] == bytes[5]);
        CHECK(result.Elements[6] == bytes[6]);
        CHECK(result.Elements[7] == bytes[7]);
        CHECK(result.Elements[8] == bytes[8]);
        CHECK(result.Elements[9] == bytes[9]);
        CHECK(result.Elements[10] == 0);
        CHECK(result.Elements[11] == 0);
        CHECK(result.Elements[12] == 0);
        CHECK(result.Elements[13] == 0);
        CHECK(result.Elements[14] == 0);
        CHECK(result.Elements[15] == 0);
    }

    SECTION("Exact")
    {
        auto result = FromBytes(std::as_bytes(std::span{bytes, 16}));

        CHECK(result.Elements[0] == bytes[0]);
        CHECK(result.Elements[1] == bytes[1]);
        CHECK(result.Elements[2] == bytes[2]);
        CHECK(result.Elements[3] == bytes[3]);
        CHECK(result.Elements[4] == bytes[4]);
        CHECK(result.Elements[5] == bytes[5]);
        CHECK(result.Elements[6] == bytes[6]);
        CHECK(result.Elements[7] == bytes[7]);
        CHECK(result.Elements[8] == bytes[8]);
        CHECK(result.Elements[9] == bytes[9]);
        CHECK(result.Elements[10] == bytes[10]);
        CHECK(result.Elements[11] == bytes[11]);
        CHECK(result.Elements[12] == bytes[12]);
        CHECK(result.Elements[13] == bytes[13]);
        CHECK(result.Elements[14] == bytes[14]);
        CHECK(result.Elements[15] == bytes[15]);
    }

    SECTION("Larger")
    {
        auto result = FromBytes(std::as_bytes(std::span{bytes, 24}));

        CHECK(result.Elements[0] == bytes[0]);
        CHECK(result.Elements[1] == bytes[1]);
        CHECK(result.Elements[2] == bytes[2]);
        CHECK(result.Elements[3] == bytes[3]);
        CHECK(result.Elements[4] == bytes[4]);
        CHECK(result.Elements[5] == bytes[5]);
        CHECK(result.Elements[6] == bytes[6]);
        CHECK(result.Elements[7] == bytes[7]);
        CHECK(result.Elements[8] == bytes[8]);
        CHECK(result.Elements[9] == bytes[9]);
        CHECK(result.Elements[10] == bytes[10]);
        CHECK(result.Elements[11] == bytes[11]);
        CHECK(result.Elements[12] == bytes[12]);
        CHECK(result.Elements[13] == bytes[13]);
        CHECK(result.Elements[14] == bytes[14]);
        CHECK(result.Elements[15] == bytes[15]);
    }
}

TEST_CASE("Uuid from namespace")
{
    using namespace weave::uuid;

    constexpr Uuid root{{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}};

    Uuid derived1 = FromNamespace(root, "derived1");

    CHECK(derived1 == Uuid{{0x9a, 0xa9, 0xa4, 0x5d, 0x1f, 0x61, 0x60, 0x3e, 0x7e, 0x2f, 0x79, 0x5a, 0xe1, 0xfe, 0xa9, 0xe9}});

    Uuid derived2 = FromNamespace(derived1, "derived2");

    CHECK(derived2 == Uuid{{0x78, 0xe8, 0xf3, 0x98, 0x0d, 0x2d, 0x1e, 0xaa, 0x20, 0xe1, 0x27, 0xb7, 0x6a, 0x4e, 0xee, 0x23}});
}
