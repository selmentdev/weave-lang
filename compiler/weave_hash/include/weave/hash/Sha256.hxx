#pragma once
#include <cstdint>
#include <array>
#include <string_view>

namespace weave::hash
{
    struct Sha256 final
    {
        std::array<std::byte, 64> buffer;
        std::array<uint32_t, 8> state;
        uint64_t count;
        size_t size;
    };

    void Sha256Initialize(Sha256& context);

    void Sha256Update(Sha256& context, std::byte const* buffer, size_t length);

    auto Sha256Finalize(Sha256& context) -> std::array<uint8_t, 32>;

    auto Sha256FromBuffer(std::byte const* buffer, size_t length) -> std::array<uint8_t, 32>;

    auto Sha256FromString(std::string_view value) -> std::array<uint8_t, 32>;
}
