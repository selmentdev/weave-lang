#pragma once
#include <cstddef>
#include <cstdint>
#include <optional>

#include "Weave.Core/Assert.hxx"

namespace Weave::Builtin
{
    constexpr uint64_t BigMul(uint64_t left, uint64_t right, uint64_t& lower)
    {
        uint64_t const ll = left & 0xFFFFFFFFu;
        uint64_t const lu = left >> 32u;
        uint64_t const rl = right & 0xFFFFFFFFu;
        uint64_t const ru = right >> 32u;
        uint64_t const ll_rl = ll * rl;
        uint64_t const ll_ru = ll * ru;
        uint64_t const lu_rl = lu * rl;
        uint64_t const lu_ru = lu * ru;
        uint64_t const m = (ll_rl >> 32u) + ll_ru + (lu_rl & 0xFFFFFFFFu);
        lower = (ll_rl & 0xFFFFFFFFu) | (m << 32u);
        return (m >> 32u) + (lu_rl >> 32u) + lu_ru;
    }
}

namespace Weave::Builtin
{
    struct UInt128 final
    {
    private:
        uint64_t _lower;
        uint64_t _upper;

    public:
        constexpr UInt128()
            : _lower{}
            , _upper{}
        {
        }

        constexpr explicit UInt128(uint64_t upper, uint64_t lower)
            : _lower{lower}
            , _upper{upper}
        {
        }

    public:
        static constexpr UInt128 Make(signed long long value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed long value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed int value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed short value)
        {
            int64_t const lower = value;
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(signed char value)
        {
            int64_t const lower = value;  // NOLINT(bugprone-signed-char-misuse)
            return UInt128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr UInt128 Make(unsigned long long value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned long value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned int value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned short value)
        {
            return UInt128{0, value};
        }

        static constexpr UInt128 Make(unsigned char value)
        {
            return UInt128{0, value};
        }

    public:
        constexpr bool IsZero() const
        {
            return (this->_lower | this->_upper) == 0;
        }

        constexpr uint64_t GetLower() const
        {
            return this->_lower;
        }

        constexpr uint64_t GetUpper() const
        {
            return this->_upper;
        }

        constexpr uint64_t ToUInt64() const
        {
            return this->_lower;
        }

    public:
        static constexpr UInt128 Min()
        {
            return UInt128{0, 0};
        }

        static constexpr UInt128 Max()
        {
            return UInt128{UINT64_MAX, UINT64_MAX};
        }

        static constexpr int Compare(UInt128 self, UInt128 other)
        {
            if (self._upper != other._upper)
            {
                return (self._upper < other._upper) ? -1 : 1;
            }

            if (self._lower != other._lower)
            {
                return (self._lower < other._lower) ? -1 : 1;
            }

            return 0;
        }

        static constexpr bool Equals(UInt128 left, UInt128 right)
        {
            return (left._lower == right._lower) && (left._upper == right._upper);
        }

        [[nodiscard]] constexpr bool operator<(UInt128 const& rhs) const
        {
            return Compare(*this, rhs) < 0;
        }

        [[nodiscard]] constexpr bool operator<=(UInt128 const& rhs) const
        {
            return Compare(*this, rhs) <= 0;
        }

        [[nodiscard]] constexpr bool operator>(UInt128 const& rhs) const
        {
            return Compare(*this, rhs) > 0;
        }

        [[nodiscard]] constexpr bool operator>=(UInt128 const& rhs) const
        {
            return Compare(*this, rhs) >= 0;
        }

        [[nodiscard]] constexpr bool operator==(UInt128 const& rhs) const
        {
            return Equals(*this, rhs);
        }

        [[nodiscard]] constexpr bool operator!=(UInt128 const& rhs) const
        {
            return not Equals(*this, rhs);
        }

    public:
        static constexpr bool CheckedAdd(UInt128& result, UInt128 lhs, UInt128 rhs)
        {
            result = Add(lhs, rhs);
            return (result._upper < lhs._upper); // overflow
        }

        static constexpr UInt128 Add(UInt128 lhs, UInt128 rhs)
        {
            uint64_t const lower = lhs._lower + rhs._lower;

            return UInt128{
                lhs._upper + rhs._upper + ((lower < lhs._lower) ? 1 : 0), // carry
                lower,
            };
        }

        static constexpr bool CheckedSubtract(UInt128& result, UInt128 lhs, UInt128 rhs)
        {
            result = Subtract(lhs, rhs);
            return (result._upper > lhs._upper); // underflow
        }

        static constexpr UInt128 Subtract(UInt128 lhs, UInt128 rhs)
        {
            uint64_t const lower = lhs._lower - rhs._lower;

            return UInt128{
                lhs._upper - rhs._upper - ((lower > lhs._lower) ? 1 : 0), // borrow
                lower,
            };
        }

        static constexpr bool CheckedMultiply(UInt128& result, UInt128 lhs, UInt128 rhs)
        {
            return not BigMul(lhs, rhs, result).IsZero();
        }

        static constexpr UInt128 Multiply(UInt128 x, UInt128 y)
        {
            uint64_t lower;
            uint64_t upper = Builtin::BigMul(x._lower, y._lower, lower);
            upper += (x._lower * y._upper) + (x._upper * y._lower);
            return UInt128{upper, lower};
        }

        static constexpr bool CheckedMultiply2(UInt128& result, UInt128 lhs, UInt128 rhs)
        {
            bool overflow = false;

            result._lower = 0;
            result._upper = 0;

            for (size_t i = 0; i < 128; ++i)
            {
                uint64_t const mask = uint64_t{1} << (i & 0x3F);

                if ((lhs._upper == 0) and (lhs._lower < mask))
                {
                    // Early exit
                    break;
                }

                if ((i >= 64) and (lhs._upper < mask))
                {
                    // Early exit
                    break;
                }

                if (mask & ((i >= 64) ? lhs._upper : lhs._lower))
                {
                    overflow |= CheckedAdd(result, rhs, result); // NOLINT(readability-suspicious-call-argument)
                }

                rhs._upper = (rhs._upper << 1) | (rhs._lower >> 63);
                rhs._lower = (rhs._lower << 1);
            }

            return overflow;
        }

        static constexpr size_t countl_zero(UInt128 value)
        {
            if (value._upper != 0)
            {
                return 127 - std::countl_zero(value._upper);
            }

            return 63 - std::countl_zero(value._lower);
        }

        static constexpr bool CheckedDivide(UInt128& quotient, UInt128& remainder, UInt128 lhs, UInt128 rhs)
        {
            if ((rhs._upper == 0) and (rhs._lower == 1))
            {
                quotient = lhs;

                remainder._lower = 0;
                remainder._upper = 0;

                return false;
            }

            if ((lhs._upper == 0) and (rhs._upper == 0))
            {
                // 64-bit fast path
                quotient._lower = lhs._lower / rhs._lower;
                quotient._upper = 0;

                remainder._lower = lhs._lower % rhs._lower;
                remainder._upper = 0;

                // No overflow possible
                return false;
            }

            if (int const r = Compare(lhs, rhs); r == 0)
            {
                // lhs / rhs => 1 rem 0
                quotient._lower = 1;
                quotient._upper = 0;

                remainder._lower = 0;
                remainder._upper = 0;

                return false;
            }
            else if (r < 0)
            {
                quotient._lower = 0;
                quotient._upper = 0;

                remainder = lhs;

                return false;
            }

            size_t const shift = countl_zero(lhs) - countl_zero(rhs);

            if (shift == 128)
            {
                // Overflow
                return true;
            }

            UInt128 d = BitShiftLeft(rhs, shift);
            UInt128 q{};

            for (size_t i = 0; i <= shift; ++i)
            {
                q = BitShiftLeft(q, 1);

                if (lhs >= d)
                {
                    lhs = lhs - d;
                    q._lower |= 1;
                }

                d = BitShiftRightZeroExtend(d, 1);
            }

            remainder = lhs;
            quotient = q;

            return false;
        }

        [[nodiscard]] static constexpr UInt128 Divide(UInt128& remainder, UInt128 lhs, UInt128 rhs)
        {
            UInt128 result{};
            CheckedDivide(result, remainder, lhs, rhs);
            return result;
        }

        [[nodiscard]] constexpr UInt128 operator+(UInt128 rhs) const
        {
            return Add(*this, rhs);
        }

        [[nodiscard]] constexpr UInt128 operator-(UInt128 rhs) const
        {
            return Subtract(*this, rhs);
        }

        [[nodiscard]] constexpr UInt128 operator*(UInt128 rhs) const
        {
            return Multiply(*this, rhs);
        }

        [[nodiscard]] constexpr UInt128 operator/(UInt128 rhs) const
        {
            UInt128 quotient;
            UInt128 remainder;
            CheckedDivide(quotient, remainder, *this, rhs);
            return quotient;
        }

        [[nodiscard]] constexpr UInt128 operator%(UInt128 rhs) const
        {
            UInt128 quotient;
            UInt128 remainder;
            CheckedDivide(quotient, remainder, *this, rhs);
            return remainder;
        }

        [[nodiscard]] constexpr UInt128 operator+(unsigned int rhs) const
        {
            return *this + UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator-(unsigned int rhs) const
        {
            return *this - UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator*(unsigned int rhs) const
        {
            return *this * UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator/(unsigned int rhs) const
        {
            return *this / UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator%(unsigned int rhs) const
        {
            return *this % UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator+(unsigned long rhs) const
        {
            return *this + UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator-(unsigned long rhs) const
        {
            return *this - UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator*(unsigned long rhs) const
        {
            return *this * UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator/(unsigned long rhs) const
        {
            return *this / UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator%(unsigned long rhs) const
        {
            return *this % UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator+(unsigned long long rhs) const
        {
            return *this + UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator-(unsigned long long rhs) const
        {
            return *this - UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator*(unsigned long long rhs) const
        {
            return *this * UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator/(unsigned long long rhs) const
        {
            return *this / UInt128{0, rhs};
        }

        [[nodiscard]] constexpr UInt128 operator%(unsigned long long rhs) const
        {
            return *this % UInt128{0, rhs};
        }

    public:
        [[nodiscard]] static constexpr UInt128 BitCompl(UInt128 rhs)
        {
            return UInt128{
                ~rhs._upper,
                ~rhs._lower,
                };
        }

        [[nodiscard]] static constexpr UInt128 BitAnd(UInt128 lhs, UInt128 rhs)
        {
            return UInt128{
                lhs._upper & rhs._upper,
                lhs._lower & rhs._lower,
                };
        }

        [[nodiscard]] static constexpr UInt128 BitOr(UInt128 lhs, UInt128 rhs)
        {
            return UInt128{
                lhs._upper | rhs._upper,
                lhs._lower | rhs._lower,
                };
        }

        [[nodiscard]] static constexpr UInt128 BitXor(UInt128 lhs, UInt128 rhs)
        {
            return UInt128{
                lhs._upper ^ rhs._upper,
                lhs._lower ^ rhs._lower,
                };
        }

        [[nodiscard]] static constexpr UInt128 BitRotateLeft(UInt128 value, size_t bits)
        {
            return BitOr(
                BitShiftLeft(value, bits),
                BitShiftRightZeroExtend(value, 128 - bits));
        }

        [[nodiscard]] static constexpr UInt128 BitRotateRight(UInt128 value, size_t bits)
        {
            return BitOr(
                BitShiftRightZeroExtend(value, bits),
                BitShiftLeft(value, 128 - bits));
        }

        [[nodiscard]] static constexpr UInt128 BitShiftLeft(UInt128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const upper = value._lower << (shift - 64);
                return UInt128{upper, 0};
            }

            uint64_t const lower = value._lower << shift;
            uint64_t const upper = (value._upper << shift) | (value._lower >> (64 - shift));
            return UInt128{upper, lower};
        }

        [[nodiscard]] static constexpr UInt128 BitShiftRightZeroExtend(UInt128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const lower = value._upper >> (shift - 64);
                return UInt128{0, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = value._upper >> shift;
            return UInt128{upper, lower};
        }

        [[nodiscard]] static constexpr UInt128 BitShiftRightSignExtend(UInt128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const lower = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> (shift - 64));
                uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> 63);
                return UInt128{upper, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> shift);
            return UInt128{upper, lower};
        }

        [[nodiscard]] static constexpr UInt128 ByteSwap(UInt128 value)
        {
            uint64_t const lower = std::byteswap(value._upper);
            uint64_t const upper = std::byteswap(value._lower);
            return UInt128{upper, lower};
        }

    public:
        [[nodiscard]] constexpr UInt128 operator~() const
        {
            return BitCompl(*this);
        }

        [[nodiscard]] constexpr UInt128 operator&(UInt128 rhs) const
        {
            return BitAnd(*this, rhs);
        }

        [[nodiscard]] constexpr UInt128 operator|(UInt128 rhs) const
        {
            return BitOr(*this, rhs);
        }

        [[nodiscard]] constexpr UInt128 operator^(UInt128 rhs) const
        {
            return BitXor(*this, rhs);
        }

    public:
        static constexpr UInt128 BigMul(UInt128 left, UInt128 right, UInt128& lower)
        {
            UInt128 const ll{0, left._lower};
            UInt128 const lu{0, left._upper};
            UInt128 const rl{0, right._lower};
            UInt128 const ru{0, right._upper};

            UInt128 const m = ll * rl;
            UInt128 const t = lu * rl + m._upper;
            UInt128 const tl = ll * ru + t._lower;
            lower._lower = m._lower;
            lower._upper = tl._lower;
            return lu * ru + t._upper + tl._upper;
        }

        static inline constexpr signed char g_character_to_digit_mapping[] = {
            // clang-format off
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
             0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1, // [0-9]
            -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // [A-Z]
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
            -1, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, // [a-z]
            25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            // clang-format on
        };
        static_assert(sizeof(g_character_to_digit_mapping) == 256);

        [[nodiscard]] static constexpr unsigned char digit_from_char(char ch) noexcept
        {
            return static_cast<unsigned char>(g_character_to_digit_mapping[static_cast<unsigned char>(ch)]);
        }

        static constexpr bool TryParse(UInt128& result, std::string_view value, unsigned radix = 10)
        {
            UInt128 cutoff;
            UInt128 cutlim;

            UInt128 ubase{0, radix};
            CheckedDivide(cutoff, cutlim, Max(), ubase);

            UInt128 temp{};

            bool overflow = false;

            for (char const c : value)
            {
                UInt128 const digit{0, digit_from_char(c)};

                if (digit._lower >= ubase._lower)
                {
                    break;
                }

                if (int const r = Compare(temp, cutoff); (r < 0) || ((r == 0) && (cutlim >= digit)))
                {
                    temp = (temp * ubase) + digit;
                }
                else
                {
                    overflow = true;
                }
            }

            result = temp;
            return !overflow;
        }

        static constexpr std::optional<UInt128> Parse(std::string_view value, unsigned radix = 10)
        {
            UInt128 result;
            return TryParse(result, value, radix) ? std::optional{result} : std::nullopt;
        }

        static std::string ToStringHex(UInt128 value)
        {
            constexpr char const* const digits = "0123456789ABCDEF";

            std::string result{};

            if (value.IsZero())
            {
                result.push_back('0');
            }
            else
            {
                while (not value.IsZero())
                {
                    result.push_back(digits[value._lower & 0xF]);
                    value = BitShiftRightZeroExtend(value, 4);
                }

                std::reverse(result.begin(), result.end());
            }

            return result;
        }

        static std::string ToString(UInt128 value)
        {
            constexpr char const* const digits = "0123456789";

            constexpr UInt128 radix{0, 10};

            std::string result{};

            if (value.IsZero())
            {
                result.push_back('0');
            }
            else
            {
                UInt128 digit{0, 0};

                while (not value.IsZero())
                {
                    CheckedDivide(value, digit, value, radix);
                    result.push_back(digits[digit._lower]);
                }

                std::reverse(result.begin(), result.end());
            }

            return result;
        }

    public:
        static constexpr double ToDouble(UInt128 value)
        {
            // Algorithm borrowed from dotnet runtime.
            constexpr uint64_t TwoPow52Bits = 0x4330000000000000;
            constexpr uint64_t TwoPow76Bits = 0x44B0000000000000;
            constexpr uint64_t TwoPow104Bits = 0x4670000000000000;
            constexpr uint64_t TwoPow128Bits = 0x47F0000000000000;

            constexpr double TwoPow52 = std::bit_cast<double>(TwoPow52Bits);
            constexpr double TwoPow76 = std::bit_cast<double>(TwoPow76Bits);
            constexpr double TwoPow104 = std::bit_cast<double>(TwoPow104Bits);
            constexpr double TwoPow128 = std::bit_cast<double>(TwoPow128Bits);

            if (value._upper == 0)
            {
                return static_cast<double>(value._lower);
            }

            if ((value._upper >> 24) == 0) // value < (2^104)
            {
                double const lower = std::bit_cast<double>(TwoPow52Bits | (value._lower << 12) >> 12) - TwoPow52;
                double const upper = std::bit_cast<double>(TwoPow104Bits | BitShiftRightZeroExtend(value, 52).ToUInt64()) - TwoPow104;

                return lower + upper;
            }

            double const lower = std::bit_cast<double>(TwoPow76Bits | (BitShiftRightZeroExtend(value, 12).ToUInt64() >> 12) | (value._lower & 0xFFFFFF)) - TwoPow76;
            double const upper = std::bit_cast<double>(TwoPow128Bits | BitShiftRightZeroExtend(value, 76).ToUInt64()) - TwoPow128;

            return lower + upper;
        }

        static constexpr float ToFloat(UInt128 value)
        {
            return static_cast<float>(ToDouble(value));
        }

        static UInt128 FromDouble(double value)
        {
            WEAVE_ASSERT(std::isfinite(value) && (value > static_cast<double>(-1)) && (value < std::ldexp(static_cast<double>(1), 128)));

            if (value >= std::ldexp(static_cast<double>(1), 64))
            {
                uint64_t const upper = static_cast<uint64_t>(std::ldexp(value, -64));
                uint64_t const lower = static_cast<uint64_t>(value - std::ldexp(static_cast<double>(upper), 64));

                return UInt128{upper, lower};
            }

            return UInt128{0, static_cast<uint64_t>(value)};
        }

        static UInt128 FromFloat(float value)
        {
            WEAVE_ASSERT(std::isfinite(value) && (value > static_cast<double>(-1)) && (value < std::ldexp(static_cast<double>(1), 128)));

            if (value >= std::ldexp(static_cast<double>(1), 64))
            {
                uint64_t const upper = static_cast<uint64_t>(std::ldexp(value, -64));
                uint64_t const lower = static_cast<uint64_t>(value - std::ldexp(static_cast<double>(upper), 64));

                return UInt128{upper, lower};
            }

            return UInt128{0, static_cast<uint64_t>(value)};
        }
    };
}

namespace Weave::Builtin
{
}
