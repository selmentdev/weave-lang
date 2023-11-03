#pragma once
#include <cstddef>
#include <cstdint>

namespace Weave::Builtin
{
    struct UInt128 final
    {
        uint64_t Low;
        uint64_t High;
    };

    constexpr UInt128 UMul64x64To128(uint64_t x, uint64_t y)
    {
        uint64_t const xl = x & 0xFFFFFFFFu;
        uint64_t const xh = x >> 32u;
        uint64_t const yl = y & 0xFFFFFFFFu;
        uint64_t const yh = y >> 32u;
        uint64_t const ll = xl * yl;
        uint64_t const lh = xl * yh;
        uint64_t const hl = xh * yl;
        uint64_t const hh = xh * yh;
        uint64_t const m = (ll >> 32u) + lh + (hl & 0xFFFFFFFFu);
        uint64_t const l = (ll & 0xFFFFFFFFu) | (m << 32u);
        uint64_t const h = (m >> 32u) + (hl >> 32u) + hh;
        return {l, h};
    }

    constexpr UInt128 Multiply(UInt128 x, UInt128 y)
    {
        UInt128 result = UMul64x64To128(x.Low, y.Low);
        result.High += (x.Low * y.High) + (x.High * y.Low);
        return result;
    }
}
