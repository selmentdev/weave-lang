#pragma once
#include <bit>
#include <cstddef>

namespace Weave
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

    inline std::byte* AlignUp(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = reinterpret_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignUp(address, alignment);
        return ptr + (aligned - address);
    }

    inline std::byte* AlignDown(std::byte* ptr, size_t alignment)
    {
        uintptr_t const address = reinterpret_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignDown(address, alignment);
        return ptr + (aligned - address);
    }

    inline void* AlignUp(void* ptr, size_t alignment)
    {
        uintptr_t const address = reinterpret_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignUp(address, alignment);
        return static_cast<char*>(ptr) + (aligned - address);
    }

    inline void* AlignDown(void* ptr, size_t alignment)
    {
        uintptr_t const address = reinterpret_cast<uintptr_t>(ptr);
        uintptr_t const aligned = AlignDown(address, alignment);
        return static_cast<char*>(ptr) + (aligned - address);
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
        requires (std::is_integral_v<T>)
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
        for (size_t i = 0; i < count; ++i)
        {
            buffer[i] = ToBigEndian(buffer[i]);
        }
    }

    template <typename T>
    constexpr void ToLittleEndianArray(T* buffer, size_t count)
        requires(std::is_integral_v<T>)
    {
        for (size_t i = 0; i < count; ++i)
        {
            buffer[i] = ToLittleEndian(buffer[i]);
        }
    }

    template <typename T>
    constexpr void ToBigEndianArray(T* output, T const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        for (size_t i = 0; i < count; ++i)
        {
            output[i] = ToBigEndian(input[i]);
        }
    }

    template <typename T>
    constexpr void ToBigEndianFromBuffer(T* output, void const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        ToBigEndianArray(output, static_cast<T const*>(input), count);
    }

    template <typename T>
    constexpr void ToBigEndianToBuffer(void* output, T const* input, size_t count)
        requires(std::is_integral_v<T>)
    {
        ToBigEndianArray(static_cast<T*>(output), input, count);
    }
}
