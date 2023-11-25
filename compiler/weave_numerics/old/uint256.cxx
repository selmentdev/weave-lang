#include <stdint.h>
#include <stddef.h>

#define weave_inline __attribute__((__always_inline__)) inline constexpr

typedef struct weave_builtin_uint128
{
    uint64_t lower;
    uint64_t upper;
} weave_builtin_uint128;

typedef struct weave_builtin_uint256
{
    weave_builtin_uint128 lower;
    weave_builtin_uint128 upper;
} weave_builtin_uint256;

typedef struct weave_builtin_int128
{
    uint64_t lower;
    int64_t upper;
} weave_builtin_int128;

weave_inline uint64_t weave_builtin_uint64_bigmul(uint64_t left, uint64_t right, uint64_t* lower)
{
    uint64_t const ll = static_cast<uint32_t>(left);
    uint64_t const lu = left >> 32u;
    uint64_t const rl = static_cast<uint32_t>(right);
    uint64_t const ru = right >> 32u;
    uint64_t const ll_rl = ll * rl;
    uint64_t const ll_ru = ll * ru;
    uint64_t const lu_rl = lu * rl;
    uint64_t const lu_ru = lu * ru;
    uint64_t const m = (ll_rl >> 32u) + ll_ru + static_cast<uint32_t>(lu_rl);
    *lower = static_cast<uint32_t>(ll_rl) | (m << 32u);
    return (m >> 32u) + (lu_rl >> 32u) + lu_ru;
}

weave_inline weave_builtin_uint128 weave_builtin_uint128_new(uint64_t upper, uint64_t lower)
{
    return {.lower = lower, .upper = upper};
}

weave_inline weave_builtin_uint128 weave_builtin_uint128_from_uint64(uint64_t value)
{
    return {.lower = value, .upper = 0};
}

weave_inline int weave_builtin_uint128_compare(weave_builtin_uint128 left, weave_builtin_uint128 right)
{
    if (left.upper != right.upper)
    {
        return (left.upper < right.upper) ? -1 : 1;
    }

    if (left.lower != right.lower)
    {
        return (left.lower < right.lower) ? -1 : 1;     
    }

    return 0;
}

weave_inline bool weave_builtin_uint128_cmplt(weave_builtin_uint128 left, weave_builtin_uint128 right)
{
    if (left.upper != right.upper)
    {
        return left.upper < right.upper;
    }

    return left.lower < right.lower;
}

weave_inline weave_builtin_uint128 weave_builtin_uint128_add(weave_builtin_uint128 left, weave_builtin_uint128 right)
{
    uint64_t const lower = left.lower + right.lower;

    return {
        .lower = lower,
        .upper = left.upper + right.upper + ((lower < left.lower) ? 1 : 0),
    };
}

weave_inline weave_builtin_uint128 weave_builtin_uint128_mul(weave_builtin_uint128 left, weave_builtin_uint128 right)
{
    uint64_t lower;
    uint64_t upper = weave_builtin_uint64_bigmul(left.lower, right.lower, &lower);
    upper += (left.lower * right.upper) + (left.upper * right.lower);
    return {.lower = lower, .upper = upper};
}

weave_inline weave_builtin_uint128 weave_builtin_uint128_muladd(weave_builtin_uint128 a, weave_builtin_uint128 b, weave_builtin_uint128 c)
{
    return weave_builtin_uint128_add(weave_builtin_uint128_mul(a, b), c);
}

weave_inline bool weave_builtin_uint128_checked_add(weave_builtin_uint128 left, weave_builtin_uint128 right, weave_builtin_uint128* result)
{
    *result = weave_builtin_uint128_add(left, right);
    return result->upper < left.upper;
}

weave_inline weave_builtin_uint128 weave_builtin_uint128_bigmul(weave_builtin_uint128 left, weave_builtin_uint128 right, weave_builtin_uint128* lower)
{
    weave_builtin_uint128 const ll = weave_builtin_uint128_from_uint64(left.lower);
    weave_builtin_uint128 const lu = weave_builtin_uint128_from_uint64(left.upper);
    weave_builtin_uint128 const rl = weave_builtin_uint128_from_uint64(right.lower);
    weave_builtin_uint128 const ru = weave_builtin_uint128_from_uint64(right.upper);

    weave_builtin_uint128 const m = weave_builtin_uint128_mul(ll, rl);
    weave_builtin_uint128 const t = weave_builtin_uint128_muladd(lu, rl, weave_builtin_uint128_from_uint64(m.upper));
    weave_builtin_uint128 const tl = weave_builtin_uint128_muladd(ll, ru, weave_builtin_uint128_from_uint64(t.lower));

    lower->lower = m.lower;
    lower->upper = tl.lower;

    return weave_builtin_uint128_add(
        weave_builtin_uint128_muladd(lu, ru, weave_builtin_uint128_from_uint64(t.upper)),
        weave_builtin_uint128_from_uint64(tl.upper));
}

weave_inline weave_builtin_uint256 weave_builtin_uint256_from_uint128(weave_builtin_uint128 value)
{
    return {.lower = value, .upper = {}};
}

weave_inline weave_builtin_uint256 weave_builtin_uint256_mul(weave_builtin_uint256 left, weave_builtin_uint256 right)
{
    weave_builtin_uint128 lower;
    weave_builtin_uint128 upper = weave_builtin_uint128_bigmul(left.lower, right.lower, &lower);
    upper = weave_builtin_uint128_add(upper,
        weave_builtin_uint128_add(
            weave_builtin_uint128_mul(left.lower, right.upper),
            weave_builtin_uint128_mul(left.upper, right.lower)));
    return {.lower = lower, .upper = upper};
}

weave_inline weave_builtin_uint256 weave_builtin_uint256_add(weave_builtin_uint256 left, weave_builtin_uint256 right)
{
    weave_builtin_uint128 const lower = weave_builtin_uint128_add(left.lower, right.lower);
    uint64_t const overflow = weave_builtin_uint128_cmplt(lower, left.lower) ? 1 : 0;
    weave_builtin_uint128 const upper = weave_builtin_uint128_add(weave_builtin_uint128_add(left.upper, right.upper), weave_builtin_uint128_from_uint64(overflow));
    return {.lower = lower, .upper = upper};
}

weave_inline weave_builtin_uint256 weave_builtin_uint256_muladd(weave_builtin_uint256 a, weave_builtin_uint256 b, weave_builtin_uint256 c)
{
    return weave_builtin_uint256_add(weave_builtin_uint256_mul(a, b), c);
}

weave_inline weave_builtin_uint256 weave_builtin_uint256_bigmul(weave_builtin_uint256 left, weave_builtin_uint256 right, weave_builtin_uint256* lower)
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


weave_inline weave_builtin_int128 weave_builtin_int128_new(int64_t upper, uint64_t lower)
{
    return {.lower = lower, .upper = upper};
}

__attribute__((__flatten__))
weave_builtin_uint256 foo(weave_builtin_uint256 a, weave_builtin_uint256 b)
{
    return weave_builtin_uint256_add(a, b);
}
