#pragma once
#include <cstdint>

namespace weave::hash
{
    constexpr uint64_t Mix64(uint64_t x)
    {
        constexpr uint64_t m = 0xe9846af9b1a615d;

        x ^= x >> 32;
        x *= m;
        x ^= x >> 32;
        x *= m;
        x ^= x >> 28;

        return x;
    }

    constexpr uint32_t Mix32(uint32_t x)
    {
        constexpr uint32_t m1 = 0x21f0aaad;
        constexpr uint32_t m2 = 0x735a2d97;

        x ^= x >> 16;
        x *= m1;
        x ^= x >> 15;
        x *= m2;
        x ^= x >> 15;

        return x;
    }

    constexpr void Combine(uint64_t& seed, uint64_t value)
    {
        seed = Mix64(seed + 0x9e3779b9 + value);
    }

    constexpr void Combine(uint32_t& seed, uint32_t value)
    {
        seed = Mix32(seed + 0x9e3779b9 + value);
    }
}
