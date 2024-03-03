#include "weave/bitwise/CompressedInteger.hxx"
#include "weave/Bitwise.hxx"
#include <utility>

namespace weave::bitwise
{
    // Encoding uses prefix bits to determine length of the integer.
    // 0xxxxxxx - 7 bits
    // 10xxxxxx ... - 14 bits
    // 110xxxxx ... - 29 bits
    // 1110xxxx ... - 60 bits

    inline constexpr uint64_t MaxEncodingValue8 = 0x7F;
    inline constexpr uint64_t MaxEncodingValue16 = 0x3FFF;
    inline constexpr uint64_t MaxEncodingValue32 = 0x1FFFFFFF;
    inline constexpr uint64_t MaxEncodingValue64 = 0x0FFFFFFFFFFFFFFF;

    inline constexpr uint16_t MaskEncoding16 = 0x8000;
    inline constexpr uint32_t MaskEncoding32 = 0xC0000000;
    inline constexpr uint64_t MaskEncoding64 = 0xE000000000000000;

    inline constexpr uint64_t ValueMask6 = (uint64_t{1} << 6) - 1;
    inline constexpr uint64_t ValueMask13 = (uint64_t{1} << 13) - 1;
    inline constexpr uint64_t ValueMask28 = (uint64_t{1} << 28) - 1;
    inline constexpr uint64_t ValueMask59 = (uint64_t{1} << 59) - 1;

    inline constexpr uint64_t InvalidCompressedInteger = INT64_MAX;

    size_t DecodeUnsigned(uint64_t& result, uint8_t const* first, uint8_t const* last)
    {
        size_t const length = last - first;

        if (length == 0)
        {
            result = InvalidCompressedInteger;
            return 0;
        }

        uint8_t const header = first[0];

        if ((header & 0b1000'0000) == 0)
        {
            result = header;
            return 1;
        }

        // Implementation note: We test only single bit in each test because it was already tested in previous test.
        if ((header & 0b0100'0000) == 0)
        {
            if (length >= 2)
            {
                uint16_t const temp = LoadUnalignedBigEndian<uint16_t>(first);
                result = temp & ~MaskEncoding16;
                return 2;
            }
        }

        if ((header & 0b0010'0000) == 0)
        {
            if (length >= 4)
            {
                uint32_t const temp = LoadUnalignedBigEndian<uint32_t>(first);
                result = temp & ~MaskEncoding32;
                return 4;
            }
        }

        if ((header & 0b0001'0000) == 0)
        {
            if (length >= 8)
            {
                uint64_t const temp = LoadUnalignedBigEndian<uint64_t>(first);
                result = temp & ~MaskEncoding64;
                return 8;
            }
        }

        result = InvalidCompressedInteger;
        return 0;
    }

    size_t EncodeUnsigned(uint64_t value, uint8_t* first, uint8_t* last)
    {
        size_t const length = last - first;

        if (length == 0)
        {
            return 0;
        }

        if (value <= MaxEncodingValue8)
        {
            first[0] = static_cast<uint8_t>(value);
            return 1;
        }

        if (value <= MaxEncodingValue16)
        {
            if (length >= 2)
            {
                uint16_t const encoded = static_cast<uint16_t>(value | MaskEncoding16);
                StoreUnalignedBigEndian<uint16_t>(first, encoded);
                return 2;
            }
        }

        if (value <= MaxEncodingValue32)
        {
            if (length >= 4)
            {
                uint32_t const encoded = static_cast<uint32_t>(value | MaskEncoding32);
                StoreUnalignedBigEndian<uint32_t>(first, encoded);
                return 4;
            }
        }

        if (value <= MaxEncodingValue64)
        {
            if (length >= 8)
            {
                uint64_t const encoded = value | MaskEncoding64;
                StoreUnalignedBigEndian<uint64_t>(first, encoded);
                return 8;
            }
        }

        return 0;
    }

    size_t DecodeSigned(int64_t& result, uint8_t const* first, uint8_t const* last)
    {
        uint64_t temp{};
        size_t const length = DecodeUnsigned(temp, first, last);

        if (length == 0)
        {
            result = InvalidCompressedInteger;
            return 0;
        }

        bool const extend = (temp & 1) != 0;
        temp >>= 1;

        if (extend)
        {
            switch (length)
            {
            case 1:
                temp |= ~ValueMask6;
                break;

            case 2:
                temp |= ~ValueMask13;
                break;

            case 4:
                temp |= ~ValueMask28;
                break;

            case 8:
                temp |= ~ValueMask59;
                break;

            default:
                std::unreachable();
            }
        }

        result = static_cast<int64_t>(temp);
        return length;
    }

    size_t EncodeSigned(int64_t value, uint8_t* first, uint8_t* last)
    {
        int64_t const sign = value >> 63;
        size_t const length = last - first;

        if ((value & ~ValueMask6) == (sign & ~ValueMask6))
        {
            if (length >= 1)
            {
                int64_t const n = ((value & static_cast<int64_t>(ValueMask6)) << 1) | (sign & 1);
                uint8_t const encoded = static_cast<uint8_t>(n);
                StoreUnalignedBigEndian<uint8_t>(first, encoded);
                return 1;
            }
        }
        else if ((value & ~ValueMask13) == (sign & ~ValueMask13))
        {
            if (length >= 2)
            {
                int64_t const n = ((value & static_cast<int64_t>(ValueMask13)) << 1) | (sign & 1);
                uint16_t const encoded = static_cast<uint16_t>(n) | MaskEncoding16;
                StoreUnalignedBigEndian<uint16_t>(first, encoded);
                return 2;
            }
        }
        else if ((value & ~ValueMask28) == (sign & ~ValueMask28))
        {
            if (length >= 4)
            {
                int64_t const n = ((value & static_cast<int64_t>(ValueMask28)) << 1) | (sign & 1);
                uint32_t const encoded = static_cast<uint32_t>(n) | MaskEncoding32;
                StoreUnalignedBigEndian<uint32_t>(first, encoded);
                return 4;
            }
        }
        else if ((value & ~ValueMask59) == (sign & ~ValueMask59))
        {
            if (length >= 8)
            {
                int64_t const n = ((value & static_cast<int64_t>(ValueMask59)) << 1) | (sign & 1);
                uint64_t const encoded = static_cast<uint64_t>(n) | MaskEncoding64;
                StoreUnalignedBigEndian<uint64_t>(first, encoded);
                return 8;
            }
        }

        return 0;
    }
}
