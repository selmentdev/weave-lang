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
        constexpr Int128(uint64_t upper, uint64_t lower)
            : _lower{lower}
            , _upper{upper}
        {
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

    public:
        static Int128 BigMul(Int128 left, Int128 right, Int128& lower)
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

        static constexpr bool IsNegative(Int128 value)
        {
            return static_cast<int64_t>(value._upper) < 0;
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
            if (IsNegative(left) == IsNegative(right))
            {
                return (left._upper < right._upper) || ((left._upper == right._upper) && (left._lower < right._lower));
            }

            return IsNegative(left);
        }

        [[nodiscard]] friend constexpr bool operator<=(Int128 const& left, Int128 const& right)
        {
            if (IsNegative(left) == IsNegative(right))
            {
                return (left._upper < right._upper) || ((left._upper == right._upper) && (left._lower <= right._lower));
            }

            return IsNegative(left);
        }

        [[nodiscard]] friend constexpr bool operator>(Int128 const& left, Int128 const& right)
        {
            if (IsNegative(left) == IsNegative(right))
            {
                return (left._upper > right._upper) || ((left._upper == right._upper) && (left._lower > right._lower));
            }

            return IsNegative(right);
        }

        [[nodiscard]] friend constexpr bool operator>=(Int128 const& left, Int128 const& right)
        {
            if (IsNegative(left) == IsNegative(right))
            {
                return (left._upper > right._upper) || ((left._upper == right._upper) && (left._lower >= right._lower));
            }

            return IsNegative(right);
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
            if (IsNegative(value))
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
