#include "Weave.Core/Platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END


#include "Weave.Core/Cryptography/Sha256.hxx"

TEST_CASE("Cryptography Sha256")
{
    using namespace Weave::Cryptography;

    Sha256Context context{};

    SECTION("Emtpy")
    {
        Sha256Initialize(context);
        auto hash = Sha256Final(context);
        constexpr uint8_t expected[]{
            0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
            0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};

        for (size_t i = 0; i < hash.size(); ++i)
        {
            REQUIRE(hash[i] == expected[i]);
        }
    }

    SECTION("foobar")
    {
        Sha256Initialize(context);
        Sha256Update(context, reinterpret_cast<std::byte const*>("foobar"), 6);
        auto hash = Sha256Final(context);
        constexpr uint8_t expected[]{
            0xc3, 0xab, 0x8f, 0xf1, 0x37, 0x20, 0xe8, 0xad, 0x90, 0x47, 0xdd, 0x39, 0x46, 0x6b, 0x3c, 0x89,
            0x74, 0xe5, 0x92, 0xc2, 0xfa, 0x38, 0x3d, 0x4a, 0x39, 0x60, 0x71, 0x4c, 0xae, 0xf0, 0xc4, 0xf2};

        for (size_t i = 0; i < hash.size(); ++i)
        {
            REQUIRE(hash[i] == expected[i]);
        }
    }
}
