#ifndef WEAVE_BUILTIN_UINT128_H
#define WEAVE_BUILTIN_UINT128_H

#include <stdint.h>
#include <stddef.h>

#if defined(_MSC_VER)
#define WEAVE_BUILTIN_UINT128_NATIVE false
#else
#define WEAVE_BUILTIN_UINT128_NATIVE true
#endif

typedef struct weave_builtin_uint128
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    unsigned __int128 value;
#else
    uint64_t lower;
    uint64_t upper;
#endif
} weave_builtin_uint128;

typedef struct weave_builtin_uint256
{
    weave_builtin_uint128 lower;
    weave_builtin_uint128 upper;
} weave_builtin_uint256;

typedef struct weave_builtin_int128
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    signed __int128 value;
#else
    uint64_t lower;
    int64_t upper;
#endif
} weave_builtin_int128;

inline uint64_t weave_builtin_uint64_bigmul(uint64_t left, uint64_t right, uint64_t* lower)
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

inline weave_builtin_uint128 weave_builtin_uint128_new(uint64_t upper, uint64_t lower)
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

inline uint64_t weave_builtin_uint128_lower(weave_builtin_uint128 value)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return static_cast<uint64_t>(value.value);
#else
    return value.lower;
#endif
}

inline uint64_t weave_builtin_uint128_upper(weave_builtin_uint128 value)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return static_cast<uint64_t>(value.value >> 64u);
#else
    return value.upper;
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_zero_upper(weave_builtin_uint128 value)
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

inline weave_builtin_uint128 weave_builtin_uint128_zero_lower(weave_builtin_uint128 value)
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

inline weave_builtin_uint128 weave_builtin_uint128_move_upper_to_lower(weave_builtin_uint128 value)
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

inline weave_builtin_uint128 weave_builtin_uint128_move_lower_to_upper(weave_builtin_uint128 value)
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

inline weave_builtin_uint128 weave_builtin_uint128_combine_lower(weave_builtin_uint128 lower, weave_builtin_uint128 upper)
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

inline weave_builtin_uint128 weave_builtin_uint128_combine_upper(weave_builtin_uint128 lower, weave_builtin_uint128 upper)
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

inline weave_builtin_uint128 weave_builtin_uint128_from_uint64(uint64_t value)
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

inline int weave_builtin_uint128_compare(weave_builtin_uint128 left, weave_builtin_uint128 right)
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

inline bool weave_builtin_uint128_cmplt(weave_builtin_uint128 left, weave_builtin_uint128 right)
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

inline uint64_t weave_builtin_uint64_adc(uint64_t left, uint64_t right, bool carry)
{
    return (left + right) + (carry ? 1 : 0);
}

inline weave_builtin_uint128 weave_builtin_uint128_adc(weave_builtin_uint128 left, weave_builtin_uint128 right, bool carry)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return {
        .value = left.value + right.value + (carry ? 1 : 0),
    };
#else
    uint64_t const lower = left.lower + right.lower;

    return {
        .lower = lower,
        .upper = weave_builtin_uint64_adc(left.upper, right.upper, carry),
    };
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_add(weave_builtin_uint128 left, weave_builtin_uint128 right)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return {
        .value = left.value + right.value,
    };
#else
    uint64_t const lower = left.lower + right.lower;

    return {
        .lower = lower,
        .upper = weave_builtin_uint64_adc(left.upper, right.upper, lower < left.lower),
    };
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_mul(weave_builtin_uint128 left, weave_builtin_uint128 right)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return {
        .value = left.value * right.value,
    };
#else
    uint64_t lower;
    uint64_t upper = weave_builtin_uint64_bigmul(left.lower, right.lower, &lower);
    upper += (left.lower * right.upper) + (left.upper * right.lower);
    return {.lower = lower, .upper = upper};
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_muladd(weave_builtin_uint128 a, weave_builtin_uint128 b, weave_builtin_uint128 c)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return {
        .value = (a.value * b.value) + c.value,
    };
#else
    return weave_builtin_uint128_add(weave_builtin_uint128_mul(a, b), c);
#endif
}

inline bool weave_builtin_uint128_checked_add(weave_builtin_uint128 left, weave_builtin_uint128 right, weave_builtin_uint128* result)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    result->value = left.value + right.value;
    return result->value < left.value;
#else
    *result = weave_builtin_uint128_add(left, right);
    return result->upper < left.upper;
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_not(weave_builtin_uint128 value)
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

inline weave_builtin_uint128 weave_builtin_uint128_and(weave_builtin_uint128 left, weave_builtin_uint128 right)
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

inline weave_builtin_uint128 weave_builtin_uint128_or(weave_builtin_uint128 left, weave_builtin_uint128 right)
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

inline weave_builtin_uint128 weave_builtin_uint128_xor(weave_builtin_uint128 left, weave_builtin_uint128 right)
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

inline weave_builtin_uint128 weave_builtin_uint128_shr(weave_builtin_uint128 value, size_t bits)
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
        return weave_builtin_uint128{
            .lower = lower,
            .upper = 0,
        };
    }

    uint64_t const lower = (value.lower >> shift) | (value.upper << (64 - shift));
    uint64_t const upper = value.upper >> shift;
    return weave_builtin_uint128{
        .lower = lower,
        .upper = upper,
    };
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_sar(weave_builtin_uint128 value, size_t bits)
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
        return weave_builtin_uint128{
            .lower = lower,
            .upper = upper,
        };
    }

    uint64_t const lower = (value.lower >> shift) | (value.upper << (64 - shift));
    uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value.upper) >> shift);
    return weave_builtin_uint128{
        .lower = lower,
        .upper = upper,
    };
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_shl(weave_builtin_uint128 value, size_t bits)
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
        return weave_builtin_uint128{
            .lower = 0,
            .upper = upper,
        };
    }

    uint64_t const lower = value.lower << shift;
    uint64_t const upper = (value.upper << shift) | (value.lower >> (64 - shift));
    return weave_builtin_uint128{
        .lower = lower,
        .upper = upper,
    };
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_rol(weave_builtin_uint128 value, size_t bits)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return {
        .value = (value.value << bits) | (value.value >> (128 - bits)),
    };
#else
    return weave_builtin_uint128_or(
        weave_builtin_uint128_shl(value, bits),
        weave_builtin_uint128_shr(value, 128 - bits));
#endif
}

inline weave_builtin_uint128 weave_builtin_uint128_ror(weave_builtin_uint128 value, size_t bits)
{
#if WEAVE_BUILTIN_UINT128_NATIVE
    return {
        .value = (value.value >> bits) | (value.value << (128 - bits)),
    };
#else
    return weave_builtin_uint128_or(
        weave_builtin_uint128_shr(value, bits),
        weave_builtin_uint128_shl(value, 128 - bits));
#endif
}


inline weave_builtin_uint128 weave_builtin_uint128_bigmul(weave_builtin_uint128 left, weave_builtin_uint128 right, weave_builtin_uint128* lower)
{
    weave_builtin_uint128 const ll = weave_builtin_uint128_zero_upper(left);
    weave_builtin_uint128 const lu = weave_builtin_uint128_move_upper_to_lower(left);
    weave_builtin_uint128 const rl = weave_builtin_uint128_zero_upper(right);
    weave_builtin_uint128 const ru = weave_builtin_uint128_move_upper_to_lower(right);

    weave_builtin_uint128 const m = weave_builtin_uint128_mul(ll, rl);
    weave_builtin_uint128 const t = weave_builtin_uint128_muladd(lu, rl, weave_builtin_uint128_move_upper_to_lower(m));
    weave_builtin_uint128 const tl = weave_builtin_uint128_muladd(ll, ru, weave_builtin_uint128_zero_upper(t));

    *lower = weave_builtin_uint128_combine_lower(m, tl);
    //lower->lower = m.lower;
    //lower->upper = tl.lower;

    return weave_builtin_uint128_add(
        weave_builtin_uint128_muladd(lu, ru, weave_builtin_uint128_move_upper_to_lower(t)),
        weave_builtin_uint128_move_upper_to_lower(tl));
}

inline weave_builtin_uint256 weave_builtin_uint256_from_uint128(weave_builtin_uint128 value)
{
    return {.lower = value, .upper = {}};
}

inline weave_builtin_uint256 weave_builtin_uint256_mul(weave_builtin_uint256 left, weave_builtin_uint256 right)
{
    weave_builtin_uint128 lower;
    weave_builtin_uint128 upper = weave_builtin_uint128_bigmul(left.lower, right.lower, &lower);
    upper = weave_builtin_uint128_add(upper,
        weave_builtin_uint128_add(
            weave_builtin_uint128_mul(left.lower, right.upper),
            weave_builtin_uint128_mul(left.upper, right.lower)));
    return {.lower = lower, .upper = upper};
}

inline weave_builtin_uint256 weave_builtin_uint256_add(weave_builtin_uint256 left, weave_builtin_uint256 right)
{
    weave_builtin_uint128 const lower = weave_builtin_uint128_add(left.lower, right.lower);
    uint64_t const overflow = weave_builtin_uint128_cmplt(lower, left.lower) ? 1 : 0;
    weave_builtin_uint128 const upper = weave_builtin_uint128_add(weave_builtin_uint128_add(left.upper, right.upper), weave_builtin_uint128_from_uint64(overflow));
    return {.lower = lower, .upper = upper};
}

inline weave_builtin_uint256 weave_builtin_uint256_muladd(weave_builtin_uint256 a, weave_builtin_uint256 b, weave_builtin_uint256 c)
{
    return weave_builtin_uint256_add(weave_builtin_uint256_mul(a, b), c);
}

inline weave_builtin_uint256 weave_builtin_uint256_bigmul(weave_builtin_uint256 left, weave_builtin_uint256 right, weave_builtin_uint256* lower)
{
    weave_builtin_uint256 const ll = weave_builtin_uint256_from_uint128(left.lower);
    weave_builtin_uint256 const lu = weave_builtin_uint256_from_uint128(left.upper);
    weave_builtin_uint256 const rl = weave_builtin_uint256_from_uint128(right.upper);
    weave_builtin_uint256 const ru = weave_builtin_uint256_from_uint128(right.upper);

    weave_builtin_uint256 const m = weave_builtin_uint256_mul(ll, rl);
    weave_builtin_uint256 const t = weave_builtin_uint256_muladd(lu, rl, weave_builtin_uint256_from_uint128(m.upper));
    weave_builtin_uint256 const tl = weave_builtin_uint256_muladd(ll, ru, weave_builtin_uint256_from_uint128(t.lower));

    lower->lower = m.lower;
    lower->upper = tl.lower;

    return weave_builtin_uint256_add(
        weave_builtin_uint256_muladd(lu, ru, weave_builtin_uint256_from_uint128(t.upper)),
        weave_builtin_uint256_from_uint128(tl.upper));
}

#endif // WEAVE_BUILTIN_UINT128_H
