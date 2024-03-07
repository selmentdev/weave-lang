#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/hash/Aes.hxx"

TEST_CASE("Cryptography Aes")
{
    using namespace weave::hash;

    SECTION("Plain key")
    {
        std::array<uint8_t, 32> key{};
        key.fill(0xFF);

        AESContext context{};

        Initialize(context, key.data(), key.size());

        std::array<uint8_t, 16> input{};
        input.fill(0xFF);

        std::array<uint8_t, 16> encoded{};
        encoded.fill(0);

        Encrypt(context, input.data(), encoded.data());

        CHECK(encoded[0] == 0xd5);
        CHECK(encoded[1] == 0xf9);
        CHECK(encoded[2] == 0x3d);
        CHECK(encoded[3] == 0x6d);
        CHECK(encoded[4] == 0x33);
        CHECK(encoded[5] == 0x11);
        CHECK(encoded[6] == 0xcb);
        CHECK(encoded[7] == 0x30);
        CHECK(encoded[8] == 0x9f);
        CHECK(encoded[9] == 0x23);
        CHECK(encoded[10] == 0x62);
        CHECK(encoded[11] == 0x1b);
        CHECK(encoded[12] == 0x02);
        CHECK(encoded[13] == 0xfb);
        CHECK(encoded[14] == 0xd5);
        CHECK(encoded[15] == 0xe2);

        std::array<uint8_t, 16> decoded{};
        decoded.fill(0xFF);

        Decrypt(context, encoded.data(), decoded.data());

        CHECK(input == decoded);
    }

    SECTION("Complicated key")
    {
        std::array<uint8_t, 32> key{
            {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                0x00, 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70,
                0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0}};

        AESContext context{};

        Initialize(context, key.data(), key.size());

        std::array<uint8_t, 16> input{};
        input.fill(0xFF);

        std::array<uint8_t, 16> encoded{};
        encoded.fill(0);

        Encrypt(context, input.data(), encoded.data());

        CHECK(encoded[0] == 0x64);
        CHECK(encoded[1] == 0x5f);
        CHECK(encoded[2] == 0xc5);
        CHECK(encoded[3] == 0x11);
        CHECK(encoded[4] == 0x17);
        CHECK(encoded[5] == 0xb7);
        CHECK(encoded[6] == 0x99);
        CHECK(encoded[7] == 0x52);
        CHECK(encoded[8] == 0xe3);
        CHECK(encoded[9] == 0x0a);
        CHECK(encoded[10] == 0x1e);
        CHECK(encoded[11] == 0x86);
        CHECK(encoded[12] == 0x16);
        CHECK(encoded[13] == 0xca);
        CHECK(encoded[14] == 0xe9);
        CHECK(encoded[15] == 0xd5);

        std::array<uint8_t, 16> decoded{};
        decoded.fill(0xFF);

        Decrypt(context, encoded.data(), decoded.data());

        CHECK(input == decoded);
    }
}
