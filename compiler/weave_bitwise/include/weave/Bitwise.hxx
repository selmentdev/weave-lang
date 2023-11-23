#pragma once
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

    template <typename T>
    constexpr void ToBigEndianArray(T* buffer, size_t count)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            for (size_t i = 0; i < count; ++i)
            {
                buffer[i] = std::byteswap(buffer[i]);
            }
        }
    }

    template <typename T>
    constexpr void ToLittleEndianArray(T* buffer, size_t count)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            for (size_t i = 0; i < count; ++i)
            {
                buffer[i] = std::byteswap(buffer[i]);
            }
        }
    }

    template <typename T>
    constexpr void ToBigEndianArray(T* output, T const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::little)
        {
            for (size_t i = 0; i < count; ++i)
            {
                output[i] = std::byteswap(input[i]);
            }
        }
    }

    template <typename T>
    constexpr void ToLittleEndianArray(T* output, T const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        if constexpr (std::endian::native == std::endian::big)
        {
            for (size_t i = 0; i < count; ++i)
            {
                output[i] = std::byteswap(input[i]);
            }
        }
    }

    template <typename T>
    constexpr void ToBigEndianFromBuffer(T* output, void const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        ToBigEndianArray(output, static_cast<T const*>(input), count);
    }

    template <typename T>
    constexpr void ToLittleEndianFromBuffer(T* output, void const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        to_little_endian_array(output, static_cast<T const*>(input), count);
    }

    template <typename T>
    constexpr void ToBigEndianToBuffer(void* output, T const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        ToBigEndianArray(static_cast<T*>(output), input, count);
    }

    template <typename T>
    constexpr void ToLittleEndianToBuffer(void* output, T const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        to_little_endian_array(static_cast<T*>(output), input, count);
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
