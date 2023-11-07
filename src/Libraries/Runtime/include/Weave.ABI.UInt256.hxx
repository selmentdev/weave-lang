#ifndef WEAVE_NUMERICS_UINT256_H
#define WEAVE_NUMERICS_UINT256_H

#include "Weave.ABI.UInt128.hxx"

namespace Weave::ABI
{
    struct UInt256 final
    {
        UInt128 lower;
        UInt128 upper;
    };
}

namespace Weave::ABI
{
    constexpr UInt256 UInt256_FromUInt128(UInt128 value)
    {
        return {
            .lower = value,
            .upper = {},
        };
    }

    constexpr UInt256 UInt256_Multiply(UInt256 left, UInt256 right)
    {
        UInt128 lower;
        UInt128 upper = UInt128_MultiplyHigh(left.lower, right.lower, lower);
        upper = UInt128_Add(upper, UInt128_Add(UInt128_Multiply(left.lower, right.upper), UInt128_Multiply(left.upper, right.lower)));
        return {
            .lower = lower,
            .upper = upper,
        };
    }

    constexpr UInt256 UInt256_Add(UInt256 left, UInt256 right)
    {
        UInt128 const lower = UInt128_Add(left.lower, right.lower);
        bool const carry = UInt128_CompareLessThan(lower, left.lower);
        UInt128 const upper = UInt128_AddWithCarry(left.upper, right.upper, carry);
        return {.lower = lower, .upper = upper,};
    }

    constexpr UInt256 UInt256_MultiplyAdd(UInt256 a, UInt256 b, UInt256 c)
    {
        return UInt256_Add(UInt256_Multiply(a, b), c);
    }

    constexpr UInt256 UInt256_MultiplyHigh(UInt256 left, UInt256 right, UInt256& lower)
    {
        UInt256 const ll = UInt256_FromUInt128(left.lower);
        UInt256 const lu = UInt256_FromUInt128(left.upper);
        UInt256 const rl = UInt256_FromUInt128(right.upper);
        UInt256 const ru = UInt256_FromUInt128(right.upper);

        UInt256 const m = UInt256_Multiply(ll, rl);
        UInt256 const t = UInt256_MultiplyAdd(lu, rl, UInt256_FromUInt128(m.upper));
        UInt256 const tl = UInt256_MultiplyAdd(ll, ru, UInt256_FromUInt128(t.lower));

        lower.lower = m.lower;
        lower.upper = tl.lower;

        return UInt256_Add(
            UInt256_MultiplyAdd(lu, ru, UInt256_FromUInt128(t.upper)),
            UInt256_FromUInt128(tl.upper));
    }
}

#endif // WEAVE_NUMERICS_UINT128_H
