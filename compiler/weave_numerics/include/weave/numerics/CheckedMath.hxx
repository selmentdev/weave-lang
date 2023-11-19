#pragma once

#include <cstdint>
#include <climits>
#include <concepts>

namespace weave::numerics
{
    template <typename T>
    constexpr bool CheckedAdd(T x, T y, T& z) noexcept
        requires(std::is_unsigned_v<T>)
    {
        z = x + y;
        return z < x;
    }

    template <typename T>
    constexpr bool CheckedAdd(T x, T y, T& z) noexcept
        requires(std::is_signed_v<T>)
    {
        z = x + y;
        return ((z ^ x) & (z ^ y)) < 0;
    }

    template <typename T>
    constexpr bool CheckedSub(T x, T y, T& z) noexcept
        requires(std::is_unsigned_v<T>)
    {
        bool const overflow = x < y;
        z = x - y;
        return overflow;
    }

    template <typename T>
    constexpr bool CheckedSub(T x, T y, T& z) noexcept
        requires(std::is_signed_v<T>)
    {
        z = x - y;
        return ((x ^ y) & (z ^ x)) < 0;
    }

    template <typename T>
    constexpr bool CheckedMul(T x, T y, T& z) noexcept
        requires(std::is_unsigned_v<T>)
    {
        z = x * y;
        return (x != 0) && ((z / x) != y);
    }

    template <typename T>
    constexpr bool CheckedMul(T x, T y, T& z) noexcept
        requires(std::is_signed_v<T>)
    {
        z = x * y;
        return ((y < 0) and (x == std::numeric_limits<T>::min())) or ((y != 0) and ((z / y) != x));
    }

    template <typename TargetT, typename SourceT>
    constexpr bool CheckedCast(TargetT& result, SourceT x) noexcept
    {
        if constexpr (std::is_signed_v<TargetT> and std::is_unsigned_v<SourceT>)
        {
            // signed <-- unsigned
            if constexpr (sizeof(TargetT) > sizeof(SourceT))
            {
                // signed target can store unsigned source
                result = static_cast<TargetT>(x);
                return false; // no overflow
            }
            else
            {
                // Target is same size or smaller
                if (x <= static_cast<SourceT>(std::numeric_limits<TargetT>::max()))
                {
                    // Value fits
                    result = static_cast<TargetT>(x);
                    return false; // no overflow
                }
            }
        }
        else if constexpr (std::is_unsigned_v<TargetT> and std::is_signed_v<SourceT>)
        {
            // unsigned <-- signed
            if (x >= 0)
            {
                // Only positive value will fit.
                if constexpr (sizeof(TargetT) >= sizeof(SourceT))
                {
                    // Value fits
                    result = static_cast<TargetT>(x);
                    return false; // no overflow
                }
                else
                {
                    // Target size is smaller. Test for upper range bound.
                    if (x <= static_cast<SourceT>(std::numeric_limits<TargetT>::max()))
                    {
                        // Value fits
                        result = static_cast<TargetT>(x);
                        return false; // no overflow
                    }
                }
            }
        }
        else
        {
            if constexpr (sizeof(TargetT) >= sizeof(SourceT))
            {
                // widen
                result = static_cast<TargetT>(x);
                return false;
            }
            else
            {
                if constexpr (std::is_signed_v<TargetT>)
                {
                    // narrow: signed <-- signed
                    if ((static_cast<SourceT>(std::numeric_limits<TargetT>::min()) <= x) and (x <= static_cast<SourceT>(std::numeric_limits<TargetT>::max())))
                    {
                        // Value fits
                        result = static_cast<TargetT>(x);
                        return false; // no overflow
                    }
                }
                else
                {
                    // narrow: unsigned <-- unsigned
                    if (x <= static_cast<SourceT>(std::numeric_limits<TargetT>::max()))
                    {
                        // Value fits
                        result = static_cast<TargetT>(x);
                        return false; // no overflow
                    }
                }
            }
        }

        // overflow.
        return true;
    }
}
