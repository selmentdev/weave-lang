#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/bitwise/CompressedInteger.hxx"

TEST_CASE("Compressed Integers")
{
    using namespace weave::bitwise;

    std::array<uint8_t, 16> buffer;
    buffer.fill(0xFF);

    uint8_t* first = buffer.data();
    uint8_t* last = first+ buffer.size();

    SECTION("Unsigned")
    {
        {
            CHECK(EncodeUnsigned(uint64_t{0x03}, first, last) == 1);
            CHECK(buffer[0] == 0x03);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 1);
            CHECK(value == 0x03);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x7f}, first, last) == 1);
            CHECK(buffer[0] == 0x7f);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 1);
            CHECK(value == 0x7f);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x80}, first, last) == 2);
            CHECK(buffer[0] == 0x80);
            CHECK(buffer[1] == 0x80);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 2);
            CHECK(value == 0x80);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x2E57}, first, last) == 2);
            CHECK(buffer[0] == 0xAE);
            CHECK(buffer[1] == 0x57);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 2);
            CHECK(value == 0x2E57);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x3FFF}, first, last) == 2);
            CHECK(buffer[0] == 0xBF);
            CHECK(buffer[1] == 0xFF);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 2);
            CHECK(value == 0x3FFF);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x4000}, first, last) == 4);
            CHECK(buffer[0] == 0xC0);
            CHECK(buffer[1] == 0x00);
            CHECK(buffer[2] == 0x40);
            CHECK(buffer[3] == 0x00);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 4);
            CHECK(value == 0x4000);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x1FFFFFFF}, first, last) == 4);
            CHECK(buffer[0] == 0xDF);
            CHECK(buffer[1] == 0xFF);
            CHECK(buffer[2] == 0xFF);
            CHECK(buffer[3] == 0xFF);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 4);
            CHECK(value == 0x1FFFFFFF);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x20000000}, first, last) == 8);
            CHECK(buffer[0] == 0xE0);
            CHECK(buffer[1] == 0x00);
            CHECK(buffer[2] == 0x00);
            CHECK(buffer[3] == 0x00);
            CHECK(buffer[4] == 0x20);
            CHECK(buffer[5] == 0x00);
            CHECK(buffer[6] == 0x00);
            CHECK(buffer[7] == 0x00);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 8);
            CHECK(value == 0x20000000);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x0FFFFFFFFFFFFFFF}, first, last) == 8);
            CHECK(buffer[0] == 0xEF);
            CHECK(buffer[1] == 0xFF);
            CHECK(buffer[2] == 0xFF);
            CHECK(buffer[3] == 0xFF);
            CHECK(buffer[4] == 0xFF);
            CHECK(buffer[5] == 0xFF);
            CHECK(buffer[6] == 0xFF);
            CHECK(buffer[7] == 0xFF);

            uint64_t value{};
            CHECK(DecodeUnsigned(value, first, last) == 8);
            CHECK(value == 0x0FFFFFFFFFFFFFFF);
        }
        {
            CHECK(EncodeUnsigned(uint64_t{0x1000000000000000}, first, last) == 0);
        }
    }

    SECTION("Signed")
    {
        {
            CHECK(EncodeSigned(int64_t{3}, first, last) == 1);
            CHECK(buffer[0] == 0x06);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 1);
            CHECK(value == 3);
        }
        {
            CHECK(EncodeSigned(int64_t{-3}, first, last) == 1);
            CHECK(buffer[0] == 0x7B);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 1);
            CHECK(value == -3);
        }
        {
            CHECK(EncodeSigned(int64_t{64}, first, last) == 2);
            CHECK(buffer[0] == 0x80);
            CHECK(buffer[1] == 0x80);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 2);
            CHECK(value == 64);
        }
        {
            CHECK(EncodeSigned(int64_t{-64}, first, last) == 1);
            CHECK(buffer[0] == 0x01);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 1);
            CHECK(value == -64);
        }
        {
            CHECK(EncodeSigned(int64_t{8192}, first, last) == 4);
            CHECK(buffer[0] == 0xC0);
            CHECK(buffer[1] == 0x00);
            CHECK(buffer[2] == 0x40);
            CHECK(buffer[3] == 0x00);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 4);
            CHECK(value == 8192);
        }
        {
            CHECK(EncodeSigned(int64_t{-8192}, first, last) == 2);
            CHECK(buffer[0] == 0x80);
            CHECK(buffer[1] == 0x01);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 2);
            CHECK(value == -8192);
        }
        {
            CHECK(EncodeSigned(int64_t{268435455}, first, last) == 4);
            CHECK(buffer[0] == 0xDF);
            CHECK(buffer[1] == 0xFF);
            CHECK(buffer[2] == 0xFF);
            CHECK(buffer[3] == 0xFE);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 4);
            CHECK(value == 268435455);
        }
        {
            CHECK(EncodeSigned(int64_t{-268435456}, first, last) == 4);
            CHECK(buffer[0] == 0xC0);
            CHECK(buffer[1] == 0x00);
            CHECK(buffer[2] == 0x00);
            CHECK(buffer[3] == 0x01);

            int64_t value{};
            CHECK(DecodeSigned(value, first, last) == 4);
            CHECK(value == -268435456);
        }
    }
}
