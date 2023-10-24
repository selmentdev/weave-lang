#pragma once
#include <cstdint>
#include <array>
#include <bit>
#include "Compiler.Core/Bits.hxx"

namespace Weave::Cryptography
{
    struct Sha256Context
    {
        std::array<uint8_t, 64> Buffer;
        std::array<uint32_t, 8> State;
        uint64_t Count;
        size_t Size;
    };

    using Sha256Digest = std::array<uint8_t, 32>;

    void Sha256Initialize(Sha256Context& context);

    void Sha256Update(Sha256Context& context, std::byte const* buffer, size_t length);

    Sha256Digest Sha256Final(Sha256Context& context);
}
