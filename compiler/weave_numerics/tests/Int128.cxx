#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>
#include <fmt/format.h>

WEAVE_EXTERNAL_HEADERS_END


#include "weave/numerics/Int128.hxx"

namespace Catch
{
    template <>
    struct StringMaker<weave::numerics::Int128>
    {
        static std::string convert(weave::numerics::Int128 const& value)
        {
            return fmt::format("0x{:016x}, 0x{:016x}", value.GetUpper(), value.GetLower());
        }
    };
}

TEST_CASE("Numerics / Int128 / Comparison")
{
    using namespace weave::numerics;

    Int128 const n = Int128::Make(-2137);
    Int128 const z = Int128::Make(0);
    Int128 const p = Int128::Make(1337);

    SECTION("<")
    {
        CHECK_FALSE(n < n);
        CHECK(n < z);
        CHECK(n < p);
        CHECK_FALSE(z < n);
        CHECK_FALSE(z < z);
        CHECK(z < p);
        CHECK_FALSE(p < n);
        CHECK_FALSE(p < z);
        CHECK_FALSE(p < p);
    }

    SECTION("<=")
    {
        CHECK(n <= n);
        CHECK(n <= z);
        CHECK(n <= p);
        CHECK_FALSE(z <= n);
        CHECK(z <= z);
        CHECK(z <= p);
        CHECK_FALSE(p <= n);
        CHECK_FALSE(p <= z);
        CHECK(p <= p);
    }

    SECTION(">")
    {
        CHECK_FALSE(n > n);
        CHECK_FALSE(n > z);
        CHECK_FALSE(n > p);
        CHECK(z > n);
        CHECK_FALSE(z > z);
        CHECK_FALSE(z > p);
        CHECK(p > n);
        CHECK(p > z);
        CHECK_FALSE(p > p);
    }

    SECTION(">=")
    {
        CHECK(n >= n);
        CHECK_FALSE(n >= z);
        CHECK_FALSE(n >= p);
        CHECK(z >= n);
        CHECK(z >= z);
        CHECK_FALSE(z >= p);
        CHECK(p >= n);
        CHECK(p >= z);
        CHECK(p >= p);
    }

    SECTION("==")
    {
        CHECK(n == n);
        CHECK_FALSE(n == z);
        CHECK_FALSE(n == p);
        CHECK_FALSE(z == n);
        CHECK(z == z);
        CHECK_FALSE(z == p);
        CHECK_FALSE(p == n);
        CHECK_FALSE(p == z);
        CHECK(p == p);
    }

    SECTION("!=")
    {
        CHECK_FALSE(n != n);
        CHECK(n != z);
        CHECK(n != p);
        CHECK(z != n);
        CHECK_FALSE(z != z);
        CHECK(z != p);
        CHECK(p != n);
        CHECK(p != z);
        CHECK_FALSE(p != p);
    }

    SECTION("Equals")
    {
        CHECK(Int128::Equals(n, n));
        CHECK_FALSE(Int128::Equals(n, z));
        CHECK_FALSE(Int128::Equals(n, p));
        CHECK_FALSE(Int128::Equals(z, n));
        CHECK(Int128::Equals(z, z));
        CHECK_FALSE(Int128::Equals(z, p));
        CHECK_FALSE(Int128::Equals(p, n));
        CHECK_FALSE(Int128::Equals(p, z));
        CHECK(Int128::Equals(p, p));
    }

    SECTION("Compare")
    {
        CHECK(Int128::Compare(n, n) == 0);
        CHECK(Int128::Compare(n, z) == -1);
        CHECK(Int128::Compare(n, p) == -1);
        CHECK(Int128::Compare(z, n) == 1);
        CHECK(Int128::Compare(z, z) == 0);
        CHECK(Int128::Compare(z, p) == -1);
        CHECK(Int128::Compare(p, n) == 1);
        CHECK(Int128::Compare(p, z) == 1);
        CHECK(Int128::Compare(p, p) == 0);
    }
}

TEST_CASE("Numerics / Int128 / MultiplyHigh")
{
    using namespace weave::numerics;

    SECTION("Sanity check")
    {
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(-5), Int128::Make(-5), lower);
            CHECK(lower == Int128::Make(25));
            CHECK(upper == Int128::Make(0));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(-5), Int128::Make(0), lower);
            CHECK(lower == Int128::Make(0));
            CHECK(upper == Int128::Make(0));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(-5), Int128::Make(5), lower);
            CHECK(lower == Int128::Make(-25));
            CHECK(upper == Int128::Make(-1));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(0), Int128::Make(-5), lower);
            CHECK(lower == Int128::Make(0));
            CHECK(upper == Int128::Make(0));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(0), Int128::Make(0), lower);
            CHECK(lower == Int128::Make(0));
            CHECK(upper == Int128::Make(0));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(0), Int128::Make(5), lower);
            CHECK(lower == Int128::Make(0));
            CHECK(upper == Int128::Make(0));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(5), Int128::Make(-5), lower);
            CHECK(lower == Int128::Make(-25));
            CHECK(upper == Int128::Make(-1));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(5), Int128::Make(0), lower);
            CHECK(lower == Int128::Make(0));
            CHECK(upper == Int128::Make(0));
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(Int128::Make(5), Int128::Make(5), lower);
            CHECK(lower == Int128::Make(25));
            CHECK(upper == Int128::Make(0));
        }
    }

    SECTION("Bits")
    {
        Int128 const nu = Int128::Min();
        Int128 const nl = Int128::Make(INT64_MIN);
        Int128 const zz = Int128::Make(0);
        Int128 const pl = Int128::Make(INT64_MAX);
        Int128 const pu = Int128::Max();

        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nu, nu, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x4000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nu, nl, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x4000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nu, zz, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nu, pl, lower);
            CHECK(lower == Int128{0x8000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0xffffffffffffffff, 0xc000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nu, pu, lower);
            CHECK(lower == Int128{0x8000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0xc000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nl, nu, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x4000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nl, nl, lower);
            CHECK(lower == Int128{0x4000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nl, zz, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nl, pl, lower);
            CHECK(lower == Int128{0xc000000000000000, 0x8000000000000000});
            CHECK(upper == Int128{0xffffffffffffffff, 0xffffffffffffffff});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(nl, pu, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x8000000000000000});
            CHECK(upper == Int128{0xffffffffffffffff, 0xc000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(zz, nu, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(zz, nl, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(zz, zz, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(zz, pl, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(zz, pu, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pl, nu, lower);
            CHECK(lower == Int128{0x8000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0xffffffffffffffff, 0xc000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pl, nl, lower);
            CHECK(lower == Int128{0xc000000000000000, 0x8000000000000000});
            CHECK(upper == Int128{0xffffffffffffffff, 0xffffffffffffffff});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pl, zz, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pl, pl, lower);
            CHECK(lower == Int128{0x3fffffffffffffff, 0x0000000000000001});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pl, pu, lower);
            CHECK(lower == Int128{0x7fffffffffffffff, 0x8000000000000001});
            CHECK(upper == Int128{0x0000000000000000, 0x3fffffffffffffff});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pu, nu, lower);
            CHECK(lower == Int128{0x8000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0xc000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pu, nl, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x8000000000000000});
            CHECK(upper == Int128{0xffffffffffffffff, 0xc000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pu, zz, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000000});
            CHECK(upper == Int128{0x0000000000000000, 0x0000000000000000});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pu, pl, lower);
            CHECK(lower == Int128{0x7fffffffffffffff, 0x8000000000000001});
            CHECK(upper == Int128{0x0000000000000000, 0x3fffffffffffffff});
        }
        {
            Int128 lower;
            Int128 upper = Int128::MultiplyHigh(pu, pu, lower);
            CHECK(lower == Int128{0x0000000000000000, 0x0000000000000001});
            CHECK(upper == Int128{0x3fffffffffffffff, 0xffffffffffffffff});
        }
    }
}

TEST_CASE("Numerics / Int128 / CheckedDivide")
{
    using namespace weave::numerics;

    SECTION("Crossing")
    {
        Int128 const numbers[]{
            Int128::Make(INT64_MAX),
            Int128::Make(INT32_MAX),
            Int128::Make(INT16_MAX),
            Int128::Make(INT8_MAX),
            Int128::Make(INT64_MIN),
            Int128::Make(INT32_MIN),
            Int128::Make(INT16_MIN),
            Int128::Make(INT8_MIN),
            Int128::Make(2137),
            Int128::Make(-1337),
            Int128::Make(6269629),
            Int128::Make(-4847464523),
            Int128::Make(256147),
            Int128::Make(-999999999),
            Int128::Make(250000),
            Int128::Make(-250000),
            Int128::Make(2500),
            Int128::Make(-2500),
            Int128::Make(67),
            Int128::Make(-63),
        };

        for (Int128 const& left : numbers)
        {
            int64_t const i64_left = left.ToInt64();

            for (Int128 const& right : numbers)
            {
                int64_t const i64_right = right.ToUInt64();

                CAPTURE(i64_left);
                CAPTURE(i64_right);
                Int128 q{2, 1};
                Int128 r{3, 7};
                CHECK_FALSE(Int128::CheckedDivide(q, r, left, right));

                Int128 const expected_q = Int128::Make(i64_left / i64_right);
                Int128 const expected_r = Int128::Make(i64_left % i64_right);
                CHECK(q == expected_q);
                CHECK(r == expected_r);

#if defined(__GNUC__) || defined(__clang__)
                // Additional tests: on compilers with support for 128-bit integers, check that the results are correct as well.
                signed __int128 const expected_q2 = static_cast<signed __int128>(i64_left) / static_cast<signed __int128>(i64_right);
                signed __int128 const expected_r2 = static_cast<signed __int128>(i64_left) % static_cast<signed __int128>(i64_right);
                CHECK(q == Int128::Make(static_cast<int64_t>(expected_q2)));
                CHECK(r == Int128::Make(static_cast<int64_t>(expected_r2)));
#endif
            }
        }
    }

    SECTION("Large Numbers")
    {
        // clang-format off
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Max(), Int128::Max())); CHECK(q == Int128::Make(1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Max(), Int128::Make(5))); CHECK(q == Int128{0x1999999999999999, 0x9999999999999999}); CHECK(r == Int128::Make(2)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Max(), Int128::Make(1))); CHECK(q == Int128{0x7fffffffffffffff, 0xffffffffffffffff}); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Max(), Int128::Make(-1))); CHECK(q == -Int128::Min() + Int128::Make(1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Max(), Int128::Make(-5))); CHECK(q == Int128{0xe666666666666666, 0x6666666666666667}); CHECK(r == Int128::Make(2)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Max(), Int128::Min())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Max()); }

        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(5), Int128::Max())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(5)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(5), Int128::Make(5))); CHECK(q == Int128::Make(1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(5), Int128::Make(1))); CHECK(q == Int128::Make(5)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(5), Int128::Make(-1))); CHECK(q == Int128::Make(-5)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(5), Int128::Make(-5))); CHECK(q == Int128::Make(-1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(5), Int128::Min())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(5)); }

        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(1), Int128::Max())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(1)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(1), Int128::Make(5))); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(1)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(1), Int128::Make(1))); CHECK(q == Int128::Make(1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(1), Int128::Make(-1))); CHECK(q == Int128::Make(-1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(1), Int128::Make(-5))); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(1)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(1), Int128::Min())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(1)); }

        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-1), Int128::Max())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(-1)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-1), Int128::Make(5))); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(-1)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-1), Int128::Make(1))); CHECK(q == Int128::Make(-1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-1), Int128::Make(-1))); CHECK(q == Int128::Make(1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-1), Int128::Make(-5))); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(-1)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-1), Int128::Min())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(-1)); }

        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-5), Int128::Max())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(-5)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-5), Int128::Make(5))); CHECK(q == Int128::Make(-1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-5), Int128::Make(1))); CHECK(q == Int128::Make(-5)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-5), Int128::Make(-1))); CHECK(q == Int128::Make(5)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-5), Int128::Make(-5))); CHECK(q == Int128::Make(1)); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Make(-5), Int128::Min())); CHECK(q == Int128::Make(0)); CHECK(r == Int128::Make(-5)); }

        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Min(), Int128::Max())); CHECK(q == Int128::Make(-1)); CHECK(r == Int128::Make(-1)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Min(), Int128::Make(5))); CHECK(q == Int128{0xe666666666666666, 0x6666666666666667}); CHECK(r == Int128::Make(-3)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Min(), Int128::Make(1))); CHECK(q == Int128::Min()); CHECK(r == Int128::Make(0)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK(Int128::CheckedDivide(q, r, Int128::Min(), Int128::Make(-1))); CHECK(q == Int128{2, 1}); CHECK(r == Int128{3, 7}); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Min(), Int128::Make(-5))); CHECK(q == Int128{0x1999999999999999, 0x9999999999999999}); CHECK(r == Int128::Make(-3)); }
        { Int128 q{2, 1}; Int128 r{3, 7}; CHECK_FALSE(Int128::CheckedDivide(q, r, Int128::Min(), Int128::Min())); CHECK(q == Int128::Make(1)); CHECK(r == Int128::Make(0)); }
        // clang-format on
    }
}

TEST_CASE("Numerics / Int128 / ToString")
{
    using namespace weave::numerics;
    {
        Int128 const c = Int128::Min();
        CHECK_FALSE(c.IsZero());
        CHECK(c.IsNegative());
        CHECK(Int128::ToString(c, 2) == "-10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
        CHECK(Int128::ToString(c, 10) == "-170141183460469231731687303715884105728");
        CHECK(Int128::ToString(c, 12) == "-2a695925806818735399a37a20a31b3534a8");
        CHECK(Int128::ToString(c, 16) == "-80000000000000000000000000000000");
        CHECK(Int128::ToString(c, 24) == "-95b794mjicl2m0cbfjnjnd2hdm58");
        CHECK(Int128::ToString(c, 32) == "-40000000000000000000000000");
        CHECK(Int128::ToString(c, 34) == "-ttq3btpo06a5neskugckddplq");
        CHECK(Int128::ToString(c, 36) == "-7ksyyizzkutudzbv8aqztecjk");
    }
    {
        Int128 const c = Int128::Make(INT64_MIN);
        CHECK_FALSE(c.IsZero());
        CHECK(c.IsNegative());
        CHECK(Int128::ToString(c, 2) == "-1000000000000000000000000000000000000000000000000000000000000000");
        CHECK(Int128::ToString(c, 10) == "-9223372036854775808");
        CHECK(Int128::ToString(c, 12) == "-41a792678515120368");
        CHECK(Int128::ToString(c, 16) == "-8000000000000000");
        CHECK(Int128::ToString(c, 24) == "-acd772jnc9l0l8");
        CHECK(Int128::ToString(c, 32) == "-8000000000000");
        CHECK(Int128::ToString(c, 34) == "-3tdtk1v8j6tpq");
        CHECK(Int128::ToString(c, 36) == "-1y2p0ij32e8e8");
    }
    {
        Int128 const c = Int128{};
        CHECK(c.IsZero());
        CHECK_FALSE(c.IsNegative());
        CHECK(Int128::ToString(c, 2) == "0");
        CHECK(Int128::ToString(c, 10) == "0");
        CHECK(Int128::ToString(c, 12) == "0");
        CHECK(Int128::ToString(c, 16) == "0");
        CHECK(Int128::ToString(c, 24) == "0");
        CHECK(Int128::ToString(c, 32) == "0");
        CHECK(Int128::ToString(c, 34) == "0");
        CHECK(Int128::ToString(c, 36) == "0");
    }
    {
        Int128 const c = Int128::Make(INT64_MAX);
        CHECK_FALSE(c.IsZero());
        CHECK_FALSE(c.IsNegative());
        CHECK(Int128::ToString(c, 2) == "111111111111111111111111111111111111111111111111111111111111111");
        CHECK(Int128::ToString(c, 10) == "9223372036854775807");
        CHECK(Int128::ToString(c, 12) == "41a792678515120367");
        CHECK(Int128::ToString(c, 16) == "7fffffffffffffff");
        CHECK(Int128::ToString(c, 24) == "acd772jnc9l0l7");
        CHECK(Int128::ToString(c, 32) == "7vvvvvvvvvvvv");
        CHECK(Int128::ToString(c, 34) == "3tdtk1v8j6tpp");
        CHECK(Int128::ToString(c, 36) == "1y2p0ij32e8e7");
    }
    {
        Int128 const c = Int128::Max();
        CHECK_FALSE(c.IsZero());
        CHECK_FALSE(c.IsNegative());
        CHECK(Int128::ToString(c, 2) == "1111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
        CHECK(Int128::ToString(c, 10) == "170141183460469231731687303715884105727");
        CHECK(Int128::ToString(c, 12) == "2a695925806818735399a37a20a31b3534a7");
        CHECK(Int128::ToString(c, 16) == "7fffffffffffffffffffffffffffffff");
        CHECK(Int128::ToString(c, 24) == "95b794mjicl2m0cbfjnjnd2hdm57");
        CHECK(Int128::ToString(c, 32) == "3vvvvvvvvvvvvvvvvvvvvvvvvv");
        CHECK(Int128::ToString(c, 34) == "ttq3btpo06a5neskugckddplp");
        CHECK(Int128::ToString(c, 36) == "7ksyyizzkutudzbv8aqztecjj");
    }
    {
        Int128 const c{0xdeadc0dedeadbeef, 0xcafebabeb00b1355};
        CHECK_FALSE(c.IsZero());
        CHECK(c.IsNegative());
        CHECK(Int128::ToString(c, 2) == "-100001010100100011111100100001001000010101001001000001000100000011010100000001010001010100000101001111111101001110110010101011");
        CHECK(Int128::ToString(c, 10) == "-44291572619948917191426471624367729835");
        CHECK(Int128::ToString(c, 12) == "-8bb9000718007a979286065b6329123274b");
        CHECK(Int128::ToString(c, 16) == "-21523f2121524110350145414ff4ecab");
        CHECK(Int128::ToString(c, 24) == "-29fgfhkfmj8nn05hd4n2if4mgjkb");
        CHECK(Int128::ToString(c, 32) == "-11a8vi28ai8483a0a5857v9r5b");
        CHECK(Int128::ToString(c, 34) == "-7qehpt0twrkts5aqvaaswes5x");
        CHECK(Int128::ToString(c, 36) == "-1z0marql1evev28zae4zm43hn");
    }
    {
        Int128 const c{0x0cafebabeb00b135, 0xdeadc0dedeadbeef};
        CHECK_FALSE(c.IsZero());
        CHECK_FALSE(c.IsNegative());
        CHECK(Int128::ToString(c, 2) == "1100101011111110101110101011111010110000000010110001001101011101111010101101110000001101111011011110101011011011111011101111");
        CHECK(Int128::ToString(c, 10) == "16864167886665047552196565776020127471");
        CHECK(Int128::ToString(c, 12) == "35144030908a77210a9b1ba385067905213");
        CHECK(Int128::ToString(c, 16) == "cafebabeb00b135deadc0dedeadbeef");
        CHECK(Int128::ToString(c, 24) == "lmk7a5jnjb5g5jlid66ng5lifcf");
        CHECK(Int128::ToString(c, 32) == "clvlqnqo0m4qttbe0rrfarfnf");
        CHECK(Int128::ToString(c, 34) == "2wn5t0qm4a61oelkb4fw68ax5");
        CHECK(Int128::ToString(c, 36) == "r1fwb420yhwydbn7jjils6wf");
    }
}
