#pragma once
#include "weave/platform/Compiler.hxx"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace weave::bitwise
{
    template <typename T>
    constexpr T AlignUp(T size, T alignment)
    {
        return (size + alignment - 1) & ~(alignment - 1);
    }

    template <typename T>
    constexpr T AlignDown(T size, T alignment)
    {
        return size & ~(alignment - 1);
    }

    template <typename T>
    constexpr bool IsAligned(T size, T alignment)
    {
        return (size & (alignment - 1)) == 0;
    }

    constexpr std::byte* AlignUp(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignUp(address, alignment);
        return ptr + (aligned - address);
    }

    constexpr std::byte* AlignDown(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignDown(address, alignment);
        return ptr + (aligned - address);
    }

    constexpr bool IsAligned(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        return (address & (alignment - 1)) == 0;
    }

    constexpr void* AlignUp(void* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignUp(address, alignment);
        return static_cast<char*>(ptr) + (aligned - address);
    }

    constexpr void* AlignDown(void* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignDown(address, alignment);
        return static_cast<char*>(ptr) + (aligned - address);
    }

    constexpr bool IsAligned(void* ptr, size_t alignment)
    {
        uintptr_t const address = std::bit_cast<uintptr_t>(ptr);
        return (address & (alignment - 1)) == 0;
    }
}

namespace weave::bitwise
{
    template <typename T>
    [[nodiscard]] constexpr T RotateRight(T value, int bits)
        requires(std::is_unsigned_v<T>);

    template <typename T>
    [[nodiscard]] constexpr T RotateLeft(T value, int bits)
        requires(std::is_unsigned_v<T>)
    {
        unsigned int const digits = std::numeric_limits<T>::digits;

        bits = bits % digits;

        if (!bits)
        {
            return value;
        }

        if (bits < 0)
        {
            return RotateRight(value, -bits);
        }

        return (value << bits) | (value >> (digits - bits));
    }

    template <typename T>
    [[nodiscard]] constexpr T RotateRight(T value, int bits)
        requires(std::is_unsigned_v<T>)
    {
        unsigned int const digits = std::numeric_limits<T>::digits;

        bits = bits % digits;

        if (!bits)
        {
            return value;
        }

        if (bits < 0)
        {
            return RotateLeft(value, -bits);
        }

        return (value >> bits) | (value << (digits - bits));
    }
}

namespace weave::bitwise
{
    template <typename T>
    [[nodiscard]] constexpr int BitCountLeadingZeros(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countl_zero(value);
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCountLeadingOnes(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countl_one(value);
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCountTrailingZeros(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countr_zero(value);
    }

    template <typename T>
    [[nodiscard]] constexpr int BitCountTrailingOnes(T value)
        requires(std::is_unsigned_v<T>)
    {
        return std::countr_one(value);
    }

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr int BitCount(T value) noexcept
    {
        return std::popcount(value);
    }

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr size_t BitWidth(T value) noexcept
    {
        return std::bit_width(value);
    }

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr bool IsPowerOf2(T value) noexcept
    {
        return (value != 0) && ((value & (value - 1)) == 0);
    }

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr T BitCeil(T value) noexcept
    {
        if (value == 0)
        {
            return 1;
        }

        return static_cast<T>(T{1} << (std::numeric_limits<T>::digits - BitCountLeadingZeros(static_cast<T>(value - 1))));
    }

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr T BitFloor(T value) noexcept
    {
        if (value == 0)
        {
            return 0;
        }

        return static_cast<T>(T{1} << (std::numeric_limits<T>::digits - 1 - BitCountLeadingZeros(value)));
    }
}

namespace weave::bitwise
{
    template <typename T>
    constexpr T ToBigEndian(T value)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return std::byteswap(value);
        }
        else
        {
            return value;
        }
    }

    template <typename T>
    constexpr T ToLittleEndian(T value)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return std::byteswap(value);
        }
        else
        {
            return value;
        }
    }
}

namespace weave::bitwise
{
    template <typename T>
    T LoadUnaligned(void const* source)
        requires(std::is_trivially_copyable_v<T>)
    {
#if WEAVE_FEATURE_UNALIGNED_ACCESS
        return *static_cast<T const*>(source);
#else
        T result;
        memcpy(&result, source, sizeof(T));
        return result;
#endif
    }

    template <typename T>
    void StoreUnaligned(void* destination, T value)
        requires(std::is_trivially_copyable_v<T>)
    {
#if WEAVE_FEATURE_UNALIGNED_ACCESS
        *static_cast<T*>(destination) = value;
#else
        memcpy(destination, &value, sizeof(T));
#endif
    }
}

// NOTE: there is no typed functions - laod & store are supported by language directly.
namespace weave::bitwise
{
    template <typename T>
    T LoadUnalignedLittleEndian(void const* source)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            return LoadUnaligned<T>(source);
        }
        else
        {
            return std::byteswap(LoadUnaligned<T>(source));
        }
    }

    template <typename T>
    void StoreUnalignedLittleEndian(void* destination, T value)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            StoreUnaligned<T>(destination, value);
        }
        else
        {
            StoreUnaligned<T>(destination, std::byteswap(value));
        }
    }

    template <typename T>
    T LoadUnalignedBigEndian(void const* source)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            return LoadUnaligned<T>(source);
        }
        else
        {
            return std::byteswap(LoadUnaligned<T>(source));
        }
    }

    template <typename T>
    void StoreUnalignedBigEndian(void* destination, T value)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            StoreUnaligned<T>(destination, value);
        }
        else
        {
            StoreUnaligned<T>(destination, std::byteswap(value));
        }
    }
}
