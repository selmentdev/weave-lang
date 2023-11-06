#pragma once
#include "Weave.Core/UInt128.hxx"

namespace Weave::Builtin
{
    struct Int128
    {
    private:
        uint64_t _lower;
        uint64_t _upper;

    public:
        constexpr Int128()
            : _lower{}
            , _upper{}
        {
        }

        constexpr Int128(uint64_t upper, uint64_t lower)
            : _lower{lower}
            , _upper{upper}
        {
        }

    public:
        static constexpr Int128 Make(signed long long value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed long value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed int value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed short value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(signed char value)
        {
            int64_t const lower = value;
            return Int128{static_cast<uint64_t>(lower >> 63), static_cast<uint64_t>(lower)};
        }

        static constexpr Int128 Make(unsigned long long value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned long value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned int value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned short value)
        {
            return Int128{0, value};
        }

        static constexpr Int128 Make(unsigned char value)
        {
            return Int128{0, value};
        }

    public:
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

        constexpr int64_t ToInt64() const
        {
            return static_cast<int64_t>(this->_lower);
        }

        constexpr std::optional<int64_t> TryToInt64() const
        {
            if (this->_upper == 0)
            {
                return static_cast<int64_t>(this->_lower);
            }

            return std::nullopt;
        }

    public:
        static constexpr Int128 Min()
        {
            return Int128{0x8000000000000000, 0x0000000000000000};
        }

        static constexpr Int128 Max()
        {
            return Int128{0x7FFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFF};
        }

    public:
        static constexpr Int128 UncheckedAdd(Int128 left, Int128 right)
        {
            uint64_t const lower = left._lower + right._lower;
            uint64_t const carry = (lower < left._lower) ? 1u : 0u;
            uint64_t const upper = left._upper + right._upper + carry;
            return Int128{upper, lower};
        }

        static constexpr bool CheckedAdd(Int128& result, Int128 left, Int128 right)
        {
            result = UncheckedAdd(left, right);

            uint32_t const sign = static_cast<uint32_t>(left._upper >> 63);

            if (sign == static_cast<uint32_t>(right._upper >> 63))
            {
                if (sign != static_cast<uint32_t>(result._upper >> 63))
                {
                    return true;
                }
            }

            return false;
        }

        static constexpr Int128 UncheckedSubtract(Int128 left, Int128 right)
        {
            uint64_t const lower = left._lower - right._lower;
            uint64_t const borrow = (lower > left._lower) ? 1u : 0u;
            uint64_t const upper = left._upper - right._upper - borrow;
            return Int128{upper, lower};
        }

        static constexpr bool CheckedSubtract(Int128& result, Int128 left, Int128 right)
        {
            result = UncheckedSubtract(left, right);

            uint32_t const sign = static_cast<uint32_t>(left._upper >> 63);

            if (sign != static_cast<uint32_t>(right._upper >> 63))
            {
                if (sign != static_cast<uint32_t>(result._upper >> 63))
                {
                    return true;
                }
            }

            return false;
        }

        static constexpr Int128 UncheckedMultiply(Int128 left, Int128 right)
        {
            uint64_t lower;
            uint64_t upper = Builtin::BigMul(left._lower, right._lower, lower);
            upper += (left._lower * right._upper) + (left._upper * right._lower);
            return Int128{upper, lower};
        }

        static constexpr bool CheckedMultiply(Int128& result, Int128 left, Int128 right)
        {
            Int128 const upper = BigMul(left, right, result);

            return (!upper.IsZero() || result.IsNegative()) && (upper.IsZero() || !result.IsNegative());
        }

        static constexpr bool CheckedDivide(Int128& quotient, Int128& remainder, Int128 left, Int128 right)
        {
            if (((right._upper == UINT64_MAX) && (right._lower == UINT64_MAX)) && ((left._upper == 0x8000'0000'0000'0000) && (left._lower == 0)))
            {
                return true;
            }

            uint64_t const sign = (left._upper ^ right._upper) & uint64_t{1} << 63;

            bool const left_negative = left.IsNegative();

            if (left_negative)
            {
                left = UncheckedAdd(BitCompl(left), Int128::Make(1u));
            }

            if (right.IsNegative())
            {
                right = UncheckedAdd(BitCompl(right), Int128::Make(1));
            }

            UInt128 r;
            UInt128 q;

            if (UInt128::CheckedDivide(q, r, UInt128{left.GetUpper(), left.GetLower()}, UInt128{right.GetUpper(), right.GetLower()}))
            {
                return true;
            }

            if (sign != 0)
            {
                q = UInt128::Add(UInt128::BitCompl(q), UInt128::Make(1));
            }

            if (left_negative)
            {
                r = UInt128::Add(UInt128::BitCompl(r), UInt128::Make(1));
            }

            quotient._lower = q.GetLower();
            quotient._upper = q.GetUpper();
            remainder._lower = r.GetLower();
            remainder._upper = r.GetUpper();
            return false;
        }

        constexpr Int128 operator-() const
        {
            return Int128{0, 0} - *this;
        }

        constexpr Int128 operator-(Int128 const& right) const
        {
            return UncheckedSubtract(*this, right);
        }

        constexpr Int128 operator+() const
        {
            return *this;
        }

        constexpr Int128 operator+(Int128 const& right) const
        {
            return UncheckedAdd(*this, right);
        }

        constexpr Int128 operator*(Int128 const& right) const
        {
            return UncheckedMultiply(*this, right);
        }

        constexpr Int128 operator/(Int128 const& right) const
        {
            Int128 q;
            Int128 r;
            CheckedDivide(q, r, *this, right);
            return q;
        }

        constexpr Int128 operator%(Int128 const& right) const
        {
            Int128 q;
            Int128 r;
            CheckedDivide(q, r, *this, right);
            return r;
        }

#if false
        static bool CheckedDivide(Int128& result, Int128 left, Int128 right)
        {
            if ((right._lower == UINT64_MAX) && (right._upper == UINT64_MAX) && (left._upper== 0x8000'0000'0000'0000) && (left._lower == 0))
            {
                return true;
            }

            uint64_t const sign = (left._upper ^ right._upper) & (uint64_t{1} << 63);

            if (IsNegative(left))
            {
                left = ~left + Int128{1};
            }

            if (IsNegative(right))
            {
                right = ~right + Int128{1};
            }

            UInt128 r;
            UInt128 q;

            if (UInt128::CheckedDivide()
        }
#endif

        constexpr Int128& operator+=(Int128 const& right)
        {
            *this = UncheckedAdd(*this, right);
            return *this;
        }

        constexpr Int128& operator-=(Int128 const& right)
        {
            *this = UncheckedSubtract(*this, right);
            return *this;
        }

        constexpr Int128& operator*=(Int128 const& right)
        {
            *this = UncheckedMultiply(*this, right);
            return *this;
        }

        constexpr Int128& operator/=(Int128 const& right) = delete;
        constexpr Int128& operator%=(Int128 const& right) = delete;


    public:
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

        [[nodiscard]] static constexpr bool TryParse(Int128& result, std::string_view value, unsigned radix = 10)
        {
            bool negative = false;

            auto next = value.begin();
            auto const last = value.end();

            if (next != last && *next == '-')
            {
                negative = true;
                ++next;
            }

            constexpr UInt128 umax = UInt128::Max();
            constexpr UInt128 smax = UInt128::BitShiftRightZeroExtend(umax, 1);
            constexpr UInt128 abs_smin = UInt128::Add(smax, UInt128::Make(1));

            UInt128 cutoff;
            UInt128 cutlim;

            UInt128 const ubase = UInt128::Make(radix);

            if (negative)
            {
                cutoff = UInt128::Divide(cutlim, abs_smin, ubase);
            }
            else
            {
                cutoff = UInt128::Divide(cutlim, umax, ubase);
            }

            UInt128 temp{};

            bool overflow = false;

            for (; next != last; ++next)
            {
                UInt128 const digit{0, digit_from_char(*next)};

                if (digit.GetLower() >= ubase.GetLower())
                {
                    break;
                }

                if (int const r = UInt128::Compare(temp, cutoff); (r < 0) || ((r == 0) && (cutlim >= digit)))
                {
                    temp = (temp * ubase) + digit;
                }
                else
                {
                    overflow = true;
                }
            }

            if (negative)
            {
                temp = UInt128::Subtract(UInt128{}, temp);
            }

            if ((next != last) || overflow)
            {
                return false;
            }

            result._lower = temp.GetLower();
            result._upper = temp.GetUpper();
            return true;
        }

        [[nodiscard]] static constexpr std::optional<Int128> Parse(std::string_view value, unsigned radix = 10)
        {
            Int128 result{};

            if (TryParse(result, value, radix))
            {
                return result;
            }

            return {};
        }

        static constexpr char g_digit_to_char_mapping[36] = {
            // clang-format off
            '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
            'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
            'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't',
            'u', 'v', 'w', 'x', 'y', 'z'
            // clang-format on
        };

        static_assert(sizeof(g_digit_to_char_mapping) == 36);

        static std::string ToString(Int128 value, unsigned radix = 10)
        {
            WEAVE_ASSERT((radix >= 2) && (radix <= 36));
            UInt128 uvalue{value.GetUpper(), value.GetLower()};
            UInt128 ubase{0, radix};

            std::string result{};

            if (value.IsNegative())
            {
                result.push_back('-');

                uvalue = UInt128::Subtract(UInt128::Make(0), uvalue);
            }

            constexpr size_t buffer_size = 128 + 2;

            char buffer[buffer_size];
            char* const buffer_end = buffer + buffer_size;
            char* rend = buffer_end;

            switch (radix)
            {
            case 10:
                {
                    UInt128 digit{};
                    do
                    {
                        uvalue = UInt128::Divide(digit, uvalue, ubase);
                        (*--rend) = static_cast<char>('0' + digit.GetLower());
                    } while (!uvalue.IsZero());
                    break;
                }

            case 2:
                {
                    do
                    {
                        (*--rend) = static_cast<char>('0' + (uvalue.GetLower() & 0b1));
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 1);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 4:
                {
                    do
                    {
                        (*--rend) = static_cast<char>('0' + (uvalue.GetLower() & 0b11));
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 2);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 8:
                {
                    do
                    {
                        (*--rend) = static_cast<char>('0' + (uvalue.GetLower() & 0b111));
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 3);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 16:
                {
                    do
                    {
                        (*--rend) = g_digit_to_char_mapping[uvalue.GetLower() & 0b1111];
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 4);
                    } while (!uvalue.IsZero());
                    break;
                }

            case 32:
                {
                    do
                    {
                        (*--rend) = g_digit_to_char_mapping[uvalue.GetLower() & 0b11111];
                        uvalue = UInt128::BitShiftRightZeroExtend(uvalue, 5);
                    } while (!uvalue.IsZero());
                    break;
                }

            default:
                {
                    UInt128 digit{};
                    do
                    {
                        uvalue = UInt128::Divide(digit, uvalue, ubase);
                        (*--rend) = g_digit_to_char_mapping[digit.GetLower()];
                    } while (!uvalue.IsZero());
                    break;
                }
            }

            result.append(rend, buffer_end);
            return result;
        }

    public:
        static constexpr Int128 BigMul(Int128 left, Int128 right, Int128& lower)
        {
            UInt128 ulower;
            UInt128 const upper = UInt128::BigMul(
                UInt128{
                    left.GetUpper(),
                    left.GetLower(),
                },
                UInt128{
                    right.GetUpper(),
                    right.GetLower(),
                },
                ulower);

            lower._lower = ulower.GetLower();
            lower._upper = ulower.GetUpper();

            return Int128{upper.GetUpper(), upper.GetLower()} - BitAnd(BitShiftRightSignExtend(left, 127), right) - BitAnd(BitShiftRightSignExtend(right, 127), left);
        }

        [[nodiscard]] constexpr bool IsNegative() const
        {
            return static_cast<int64_t>(this->_upper) < 0;
        }

        [[nodiscard]] constexpr bool IsZero() const
        {
            return (this->_lower | this->_upper) == 0;
        }

    public:
        [[nodiscard]] static constexpr Int128 BitCompl(Int128 rhs)
        {
            return Int128{
                ~rhs._upper,
                ~rhs._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitAnd(Int128 lhs, Int128 rhs)
        {
            return Int128{
                lhs._upper & rhs._upper,
                lhs._lower & rhs._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitOr(Int128 lhs, Int128 rhs)
        {
            return Int128{
                lhs._upper | rhs._upper,
                lhs._lower | rhs._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitXor(Int128 lhs, Int128 rhs)
        {
            return Int128{
                lhs._upper ^ rhs._upper,
                lhs._lower ^ rhs._lower,
            };
        }

        [[nodiscard]] static constexpr Int128 BitRotateLeft(Int128 value, size_t bits)
        {
            return BitOr(
                BitShiftLeft(value, bits),
                BitShiftRightZeroExtend(value, 128 - bits));
        }

        [[nodiscard]] static constexpr Int128 BitRotateRight(Int128 value, size_t bits)
        {
            return BitOr(
                BitShiftRightZeroExtend(value, bits),
                BitShiftLeft(value, 128 - bits));
        }

        [[nodiscard]] static constexpr Int128 BitShiftLeft(Int128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const upper = value._lower << (shift - 64);
                return Int128{upper, 0};
            }

            uint64_t const lower = value._lower << shift;
            uint64_t const upper = (value._upper << shift) | (value._lower >> (64 - shift));
            return Int128{upper, lower};
        }

        [[nodiscard]] static constexpr Int128 BitShiftRightZeroExtend(Int128 value, size_t bits)
        {
            size_t const shift = bits & 0x7F;

            if (shift == 0)
            {
                return value;
            }

            if (shift >= 64)
            {
                uint64_t const lower = value._upper >> (shift - 64);
                return Int128{0, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = value._upper >> shift;
            return Int128{upper, lower};
        }

        [[nodiscard]] static constexpr Int128 BitShiftRightSignExtend(Int128 value, size_t bits)
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
                return Int128{upper, lower};
            }

            uint64_t const lower = (value._lower >> shift) | (value._upper << (64 - shift));
            uint64_t const upper = static_cast<uint64_t>(static_cast<int64_t>(value._upper) >> shift);
            return Int128{upper, lower};
        }

    public:
        [[nodiscard]] static constexpr int Compare(Int128 left, Int128 right)
        {
            if (left < right)
            {
                return -1;
            }

            if (left > right)
            {
                return 1;
            }

            return 0;
        }

        [[nodiscard]] static constexpr bool Equals(Int128 left, Int128 right)
        {
            return left == right;
        }

        [[nodiscard]] friend constexpr bool operator<(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper < right._upper) || ((left._upper == right._upper) && (left._lower < right._lower));
            }

            return left.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator<=(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper < right._upper) || ((left._upper == right._upper) && (left._lower <= right._lower));
            }

            return left.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator>(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper > right._upper) || ((left._upper == right._upper) && (left._lower > right._lower));
            }

            return right.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator>=(Int128 const& left, Int128 const& right)
        {
            if (left.IsNegative() == right.IsNegative())
            {
                return (left._upper > right._upper) || ((left._upper == right._upper) && (left._lower >= right._lower));
            }

            return right.IsNegative();
        }

        [[nodiscard]] friend constexpr bool operator==(Int128 const& left, Int128 const& right)
        {
            return (left._lower == right._lower) && (left._upper == right._upper);
        }

        [[nodiscard]] friend constexpr bool operator!=(Int128 const& left, Int128 const& right)
        {
            return (left._lower != right._lower) || (left._upper != right._upper);
        }

    public:
        [[nodiscard]] static constexpr double ToDouble(Int128 value)
        {
            if (value.IsNegative())
            {
                value = -value;
                return -UInt128::ToDouble(UInt128{value._upper, value._lower});
            }

            return UInt128::ToDouble(UInt128{value._upper, value._lower});
        }

        [[nodiscard]] static constexpr Int128 FromDouble(double value)
        {
            if (!std::is_constant_evaluated())
            {
                constexpr double p2_127 = 170141183460469231731687303715884105728.0;
                WEAVE_ASSERT(value >= -p2_127);
                WEAVE_ASSERT(std::isfinite(value));
                WEAVE_ASSERT(value < p2_127);
            }

            bool const negative = value < 0.0;

            if (negative)
            {
                value = -value;
            }

            if (value >= 1.0)
            {
                uint64_t const bits = std::bit_cast<uint64_t>(value);
                Int128 result{
                    (bits << 12) >> 1 | 0x8000'0000'0000'0000,
                    0x0000'0000'0000'0000,
                };

                result = BitShiftRightSignExtend(result, 1023 + 128 - 1 - static_cast<int>(bits >> 52));

                if (negative)
                {
                    result = -result;
                }

                return result;
            }

            return Int128{0, 0};
        }

        [[nodiscard]] static constexpr float ToFloat(Int128 value)
        {
            return static_cast<float>(ToDouble(value));
        }
    };
}
