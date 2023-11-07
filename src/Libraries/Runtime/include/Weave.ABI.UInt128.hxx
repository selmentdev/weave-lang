#ifndef WEAVE_NUMERICS_UINT128_H
#define WEAVE_NUMERICS_UINT128_H

#include <cstdint>
#include <cstddef>

#if defined(_MSC_VER)
#define WEAVE_BUILTIN_UINT128_NATIVE false
#else
#define WEAVE_BUILTIN_UINT128_NATIVE true
#endif

namespace Weave::ABI
{
    struct UInt128 final
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        unsigned __int128 value;
#else
        uint64_t lower;
        uint64_t upper;
#endif
    };
}

namespace Weave::ABI
{
    constexpr uint64_t UInt64_MultiplyHigh(uint64_t left, uint64_t right, uint64_t* lower)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        unsigned __int128 const r = static_cast<unsigned __int128>(left) * static_cast<unsigned __int128>(right);
        *lower = static_cast<uint64_t>(r);
        return static_cast<uint64_t>(r >> 64u);
#else
        uint64_t const ll = left & 0xFFFFFFFFu;
        uint64_t const lu = left >> 32u;
        uint64_t const rl = right & 0xFFFFFFFFu;
        uint64_t const ru = right >> 32u;
        uint64_t const ll_rl = ll * rl;
        uint64_t const ll_ru = ll * ru;
        uint64_t const lu_rl = lu * rl;
        uint64_t const lu_ru = lu * ru;
        uint64_t const m = (ll_rl >> 32u) + ll_ru + (lu_rl & 0xFFFFFFFFu);
        *lower = (ll_rl & 0xFFFFFFFFu) | (m << 32u);
        return (m >> 32u) + (lu_rl >> 32u) + lu_ru;
#endif
    }

    constexpr uint64_t UInt64_AddWithCarry(uint64_t left, uint64_t right, bool carry)
    {
        return (left + right) + (carry ? 1 : 0);
    }

    constexpr uint64_t UInt64_SubtractWithBorrow(uint64_t left, uint64_t right, bool borrow)
    {
        return (left - right) - (borrow ? 1 : 0);
    }
}

namespace Weave::ABI
{
    constexpr UInt128 UInt128_New(uint64_t upper, uint64_t lower)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = (static_cast<unsigned __int128>(upper) << 64) | static_cast<unsigned __int128>(lower),
        };
#else
        return {
            .lower = lower,
            .upper = upper,
        };
#endif
    }

    constexpr uint64_t UInt128_Lower(UInt128 value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return static_cast<uint64_t>(value.value);
#else
        return value.lower;
#endif
    }

    constexpr uint64_t UInt128_Upper(UInt128 value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return static_cast<uint64_t>(value.value >> 64u);
#else
        return value.upper;
#endif
    }

    constexpr UInt128 UInt128_ZeroUpper(UInt128 value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = static_cast<unsigned __int128>(value.value) & 0xFFFFFFFFFFFFFFFFu,
        };
#else
        return {
            .lower = value.lower,
            .upper = 0,
        };
#endif
    }

    constexpr UInt128 UInt128_ZeroLower(UInt128 value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = static_cast<unsigned __int128>(value.value) & ~0xFFFFFFFFFFFFFFFFu,
        };
#else
        return {
            .lower = 0,
            .upper = value.upper,
        };
#endif
    }

    constexpr UInt128 UInt128_MoveUpperToLower(UInt128 value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = static_cast<unsigned __int128>(value.value) >> 64u,
        };
#else
        return {
            .lower = value.upper,
            .upper = 0,
        };
#endif
    }

    constexpr UInt128 UInt128_MoveLowerToUpper(UInt128 value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = static_cast<unsigned __int128>(value.value) << 64u,
        };
#else
        return {
            .lower = 0,
            .upper = value.lower,
        };
#endif
    }

    constexpr UInt128 UInt128_CombineLower(UInt128 lower, UInt128 upper)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = (static_cast<unsigned __int128>(upper.value) << 64u) | (static_cast<unsigned __int128>(lower.value) & 0xFFFFFFFFFFFFFFFFu),
        };
#else
        return {
            .lower = lower.lower,
            .upper = upper.lower,
        };
#endif
    }

    constexpr UInt128 UInt128_CombineUpper(UInt128 lower, UInt128 upper)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        unsigned __int128 lower_upper = static_cast<unsigned __int128>(lower.value) >> 64u;
        unsigned __int128 upper_upper = static_cast<unsigned __int128>(upper.value) >> 64u;
        return {
            .value = (upper_upper << 64u) | (lower_upper & 0xFFFFFFFFFFFFFFFFu),
        };
#else
        return {
            .lower = lower.upper,
            .upper = upper.upper,
        };
#endif
    }

    constexpr UInt128 UInt128_FromUInt64(uint64_t value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = static_cast<unsigned __int128>(value),
        };
#else
        return {
            .lower = value,
            .upper = 0,
        };
#endif
    }

    constexpr int UInt128_Compare(UInt128 left, UInt128 right)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        if (left.value < right.value)
        {
            return -1;
        }
        else if (left.value > right.value)
        {
            return 1;
        }

        return 0;
#else
        if (left.upper != right.upper)
        {
            return (left.upper < right.upper) ? -1 : 1;
        }

        if (left.lower != right.lower)
        {
            return (left.lower < right.lower) ? -1 : 1;
        }

        return 0;
#endif
    }

    constexpr bool UInt128_CompareLessThan(UInt128 left, UInt128 right)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return left.value < right.value;
#else
        if (left.upper != right.upper)
        {
            return left.upper < right.upper;
        }

        return left.lower < right.lower;
#endif
    }

    constexpr UInt128 UInt128_AddWithCarry(UInt128 left, UInt128 right, bool carry)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = left.value + right.value + (carry ? 1 : 0),
        };
#else
        uint64_t const lower = left.lower + right.lower;

        return {
            .lower = lower,
            .upper = UInt64_AddWithCarry(left.upper, right.upper, carry),
        };
#endif
    }

    constexpr UInt128 UInt128_Add(UInt128 left, UInt128 right)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = left.value + right.value,
        };
#else
        uint64_t const lower = left.lower + right.lower;

        return {
            .lower = lower,
            .upper = UInt64_AddWithCarry(left.upper, right.upper, lower < left.lower),
        };
#endif
    }

    constexpr UInt128 UInt128_Multiply(UInt128 left, UInt128 right)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = left.value * right.value,
        };
#else
        uint64_t lower;
        uint64_t upper = UInt64_MultiplyHigh(left.lower, right.lower, &lower);
        upper += (left.lower * right.upper) + (left.upper * right.lower);
        return {.lower = lower, .upper = upper};
#endif
    }

    constexpr UInt128 UInt128_MultiplyAdd(UInt128 a, UInt128 b, UInt128 c)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = (a.value * b.value) + c.value,
        };
#else
        return UInt128_Add(UInt128_Multiply(a, b), c);
#endif
    }

    constexpr bool UInt128_CheckedAdd(UInt128 left, UInt128 right, UInt128& result)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        result->value = left.value + right.value;
        return result->value < left.value;
#else
        result = UInt128_Add(left, right);
        return result.upper < left.upper;
#endif
    }

    constexpr UInt128 UInt128_BitNot(UInt128 value)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = ~value.value,
        };
#else
        return {
            .lower = ~value.lower,
            .upper = ~value.upper,
        };
#endif
    }

    constexpr UInt128 UInt128_BitAnd(UInt128 left, UInt128 right)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = left.value & right.value,
        };
#else
        return {
            .lower = left.lower & right.lower,
            .upper = left.upper & right.upper,
        };
#endif
    }

    constexpr UInt128 UInt128_BitOr(UInt128 left, UInt128 right)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = left.value | right.value,
        };
#else
        return {
            .lower = left.lower | right.lower,
            .upper = left.upper | right.upper,
        };
#endif
    }

    constexpr UInt128 UInt128_BitXor(UInt128 left, UInt128 right)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = left.value ^ right.value,
        };
#else
        return {
            .lower = left.lower ^ right.lower,
            .upper = left.upper ^ right.upper,
        };
#endif
    }

    constexpr UInt128 UInt128_ShiftRightZeroExtend(UInt128 value, size_t bits)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = static_cast<unsigned __int128>(value.value) >> bits,
        };
#else
        size_t const shift = bits & 0x7F;

        if (shift == 0)
        {
            return value;
        }

        if (shift >= 64)
        {
            uint64_t const lower = value.upper >> (shift - 64);
            return UInt128{
                .lower = lower,
                .upper = 0,
            };
        }

        uint64_t const lower = (value.lower >> shift) | (value.upper << (64 - shift));
        uint64_t const upper = value.upper >> shift;
        return UInt128{
            .lower = lower,
            .upper = upper,
        };
#endif
    }

    constexpr UInt128 UInt128_ShiftRightSignExtend(UInt128 value, size_t bits)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = static_cast<unsigned __int128>(static_cast<signed __int128>(value.value) >> bits),
        };
#else
        size_t const shift = bits & 0x7F;

        if (shift == 0)
        {
            return value;
        }

        if (shift >= 64)
        {
            uint64_t const lower = static_cast<uint64_t>(static_cast<int64_t>(value.upper) >> (shift - 64));
            uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value.upper) >> 63);
            return UInt128{
                .lower = lower,
                .upper = upper,
            };
        }

        uint64_t const lower = (value.lower >> shift) | (value.upper << (64 - shift));
        uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value.upper) >> shift);
        return UInt128{
            .lower = lower,
            .upper = upper,
        };
#endif
    }

    constexpr UInt128 UInt128_ShiftLeft(UInt128 value, size_t bits)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = value.value << bits,
        };
#else
        size_t const shift = bits & 0x7F;

        if (shift == 0)
        {
            return value;
        }

        if (shift >= 64)
        {
            uint64_t const upper = value.lower << (shift - 64);
            return UInt128{
                .lower = 0,
                .upper = upper,
            };
        }

        uint64_t const lower = value.lower << shift;
        uint64_t const upper = (value.upper << shift) | (value.lower >> (64 - shift));
        return UInt128{
            .lower = lower,
            .upper = upper,
        };
#endif
    }

    constexpr UInt128 UInt128_RotateLeft(UInt128 value, size_t bits)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = (value.value << bits) | (value.value >> (128 - bits)),
        };
#else
        return UInt128_BitOr(
            UInt128_ShiftLeft(value, bits),
            UInt128_ShiftRightZeroExtend(value, 128 - bits));
#endif
    }

    constexpr UInt128 UInt128_RotateRight(UInt128 value, size_t bits)
    {
#if WEAVE_BUILTIN_UINT128_NATIVE
        return {
            .value = (value.value >> bits) | (value.value << (128 - bits)),
        };
#else
        return UInt128_BitOr(
            UInt128_ShiftRightZeroExtend(value, bits),
            UInt128_ShiftLeft(value, 128 - bits));
#endif
    }

    constexpr UInt128 UInt128_MultiplyHigh(UInt128 left, UInt128 right, UInt128& lower)
    {
        UInt128 const ll = UInt128_ZeroUpper(left);
        UInt128 const lu = UInt128_MoveUpperToLower(left);
        UInt128 const rl = UInt128_ZeroUpper(right);
        UInt128 const ru = UInt128_MoveUpperToLower(right);

        UInt128 const m = UInt128_Multiply(ll, rl);
        UInt128 const t = UInt128_MultiplyAdd(lu, rl, UInt128_MoveUpperToLower(m));
        UInt128 const tl = UInt128_MultiplyAdd(ll, ru, UInt128_ZeroUpper(t));

        lower = UInt128_CombineLower(m, tl);

        return UInt128_Add(
            UInt128_MultiplyAdd(lu, ru, UInt128_MoveUpperToLower(t)),
            UInt128_MoveUpperToLower(tl));
    }
}

#endif // WEAVE_NUMERICS_UINT128_H
