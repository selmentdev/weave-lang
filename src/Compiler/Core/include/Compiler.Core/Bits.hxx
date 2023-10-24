#pragma once
#include <bit>

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

    constexpr void ToBigEndianBuffer(uint32_t* output, uint8_t const* input, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            output[i] = (static_cast<uint32_t>(input[i * 4 + 0]) << 24) |
                (static_cast<uint32_t>(input[i * 4 + 1]) << 16) |
                (static_cast<uint32_t>(input[i * 4 + 2]) << 8) |
                (static_cast<uint32_t>(input[i * 4 + 3]) << 0);
        }
    }

    constexpr void ToBigEndianBuffer(uint8_t* output, uint32_t const* input, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            output[i * 4 + 0] = static_cast<uint8_t>(input[i] >> 24);
            output[i * 4 + 1] = static_cast<uint8_t>(input[i] >> 16);
            output[i * 4 + 2] = static_cast<uint8_t>(input[i] >> 8);
            output[i * 4 + 3] = static_cast<uint8_t>(input[i] >> 0);
        }
    }

    constexpr void ToBigEndianBuffer(uint64_t* output, uint8_t const* input, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            output[i] = (static_cast<uint64_t>(input[i * 8 + 0]) << 56) |
                (static_cast<uint64_t>(input[i * 8 + 1]) << 48) |
                (static_cast<uint64_t>(input[i * 8 + 2]) << 40) |
                (static_cast<uint64_t>(input[i * 8 + 3]) << 32) |
                (static_cast<uint64_t>(input[i * 8 + 4]) << 24) |
                (static_cast<uint64_t>(input[i * 8 + 5]) << 16) |
                (static_cast<uint64_t>(input[i * 8 + 6]) << 8) |
                (static_cast<uint64_t>(input[i * 8 + 7]) << 0);
        }
    }

    constexpr void ToBigEndianBuffer(uint8_t* output, uint64_t const* input, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
        {
            output[i * 8 + 0] = static_cast<uint8_t>(input[i] >> 56);
            output[i * 8 + 1] = static_cast<uint8_t>(input[i] >> 48);
            output[i * 8 + 2] = static_cast<uint8_t>(input[i] >> 40);
            output[i * 8 + 3] = static_cast<uint8_t>(input[i] >> 32);
            output[i * 8 + 4] = static_cast<uint8_t>(input[i] >> 24);
            output[i * 8 + 5] = static_cast<uint8_t>(input[i] >> 16);
            output[i * 8 + 6] = static_cast<uint8_t>(input[i] >> 8);
            output[i * 8 + 7] = static_cast<uint8_t>(input[i] >> 0);
        }
    }
}
