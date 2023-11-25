#include "weave/Unicode.hxx"

namespace weave::unicode::impl
{
    // https://tools.ietf.org/html/rfc3629
    inline constexpr uint8_t UTF8_CHAR_WIDTH[256]{
        // 1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 0
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 1
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 2
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 3
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 4
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 5
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 6
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, // 7
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 8
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 9
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // A
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // B
        0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // D
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, // E
        4, 4, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // F
    };

    inline constexpr char32_t UTF8_MAX_ENCODED_VALUE_FOR_SIZE[5]{
        0u,
        0x0000'0080u,
        0x0000'0800u,
        0x0001'0000u,
        0x0011'0000u,
    };

    inline constexpr size_t UTF8_MAX_WIDTH = 4u;
    inline constexpr uint8_t UTF8_CONTINUATION_MASK = 0b0011'1111u;

    inline constexpr size_t UTF16_SURROGATE_SHIFT = 10u;
    inline constexpr uint32_t UTF16_SURROGATE_MASK = (1u << UTF16_SURROGATE_SHIFT) - 1u;
    inline constexpr uint32_t UTF16_SURROGATE_BASE = 0x1'0000u;

    inline constexpr uint32_t UTF16_SURROGATE_HIGH_FIRST = 0xD800u;
    inline constexpr uint32_t UTF16_SURROGATE_HIGH_LAST = 0xDBFFu;
    inline constexpr uint32_t UTF16_SURROGATE_LOW_FIRST = 0xDC00u;
    inline constexpr uint32_t UTF16_SURROGATE_LOW_LAST = 0xDFFFu;
    inline constexpr uint32_t UTF16_SURROGATE_FIRST = UTF16_SURROGATE_HIGH_FIRST;
    inline constexpr uint32_t UTF16_SURROGATE_LAST = UTF16_SURROGATE_LOW_LAST;

    inline constexpr uint32_t UNICODE_REPLACEMENT_CHARACTER = 0xFFFDu;
    inline constexpr uint32_t UNICODE_MAX_BMP = 0xFFFFu;
    inline constexpr uint32_t UNICODE_MAX = 0x10'FFFFu;

    constexpr bool UTF16IsSurrogatePair(char32_t value)
    {
        return (UTF16_SURROGATE_FIRST <= value) and (value <= UTF16_SURROGATE_LAST);
    }

    constexpr bool UTF16IsLowSurrogatePair(char32_t value)
    {
        return (UTF16_SURROGATE_LOW_FIRST <= value) and (value <= UTF16_SURROGATE_LOW_LAST);
    }

    constexpr bool UTF16IsHighSurrogatePair(char32_t value)
    {
        return (UTF16_SURROGATE_HIGH_FIRST <= value) and (value <= UTF16_SURROGATE_HIGH_LAST);
    }

    constexpr uint32_t UTF16CombineSurrogatePair(uint32_t high, uint32_t low)
    {
        return ((high - UTF16_SURROGATE_HIGH_FIRST) << UTF16_SURROGATE_SHIFT) + (low - UTF16_SURROGATE_LOW_FIRST) + UTF16_SURROGATE_BASE;
    }

    constexpr uint32_t UTF8FirstByte(uint8_t byte, size_t width)
    {
        return (byte & (0b0111'1111u >> width));
    }

    constexpr uint32_t UTF8AccumulateContinuationByte(uint32_t ch, uint8_t byte)
    {
        return (ch << 6) | (byte & UTF8_CONTINUATION_MASK);
    }

    constexpr bool UTF8IsContinuationByte(uint8_t byte)
    {
        return static_cast<int8_t>(byte) < -64;
    }
}

namespace weave::unicode
{
    bool IsValidCodePoint(char32_t value)
    {
        if (value <= impl::UNICODE_MAX)
        {
            return not impl::UTF16IsSurrogatePair(value);
        }

        return false;
    }

    char32_t SanitizeCodepoint(char32_t value)
    {
        if (value <= impl::UNICODE_MAX)
        {
            if ((impl::UTF16_SURROGATE_FIRST <= value) and (value <= impl::UTF16_SURROGATE_LAST))
            {
                return impl::UNICODE_REPLACEMENT_CHARACTER;
            }

            return value;
        }

        return impl::UNICODE_REPLACEMENT_CHARACTER;
    }
}

namespace weave::unicode
{
    ConversionResult Decode(char32_t& result, const uint8_t*& first, const uint8_t* last)
    {
        if (first < last)
        {
            uint8_t const lead = *first++;

            if (lead <= 0x7Fu)
            {
                result = lead;
                return ConversionResult::Success;
            }

            size_t const width = impl::UTF8_CHAR_WIDTH[lead];

            if (width == 0)
            {
                // Lead byte is not valid.
                result = impl::UNICODE_REPLACEMENT_CHARACTER;
                return ConversionResult::SourceIllegal;
            }

            uint32_t ch = impl::UTF8FirstByte(lead, width);

            size_t consumed = 1;

            for (; (consumed < width) and (first < last); ++consumed)
            {
                uint8_t const next = *first++;

                if (not impl::UTF8IsContinuationByte(next))
                {
                    // Invalid byte sequence.
                    --first;
                    result = impl::UNICODE_REPLACEMENT_CHARACTER;
                    return ConversionResult::SourceIllegal;
                }

                ch = impl::UTF8AccumulateContinuationByte(ch, next);
            }

            if (consumed != width)
            {
                // Byte sequence was legal, but source exhausted. Restore iterator and return.
                first -= consumed;
                result = impl::UNICODE_REPLACEMENT_CHARACTER;
                return ConversionResult::SourceExhausted;
            }

            char32_t const valid_range_start = impl::UTF8_MAX_ENCODED_VALUE_FOR_SIZE[width - 1];
            char32_t const valid_range_end = impl::UTF8_MAX_ENCODED_VALUE_FOR_SIZE[width];

            bool const is_valid_range = (valid_range_start <= ch) and (ch < valid_range_end);

            if (not is_valid_range)
            {
                // Encoded character could be encoded using fewer bytes. Overlong sequences are not allowed.
                first -= consumed;
                result = impl::UNICODE_REPLACEMENT_CHARACTER;
                return ConversionResult::SourceIllegal;
            }

            // ch -= Private::UTF8_CODEPOINT_OFFSETS[width];

            if (impl::UTF16IsSurrogatePair(ch) or (ch > impl::UNICODE_MAX))
            {
                // Byte sequence was valid UTF8 sequence, but encoded invalid codepoint.
                result = impl::UNICODE_REPLACEMENT_CHARACTER;
                return ConversionResult::SourceIllegal;
            }

            result = ch;
            return ConversionResult::Success;
        }

        return ConversionResult::SourceExhausted;
    }

    ConversionResult Encode(uint8_t*& first, uint8_t* last, char32_t codepoint)
    {
        if (not IsValidCodePoint(codepoint))
        {
            return ConversionResult::SourceIllegal;
        }

        if (codepoint == 0u)
        {
            if ((first + 1) < last)
            {
                (*first++) = static_cast<uint8_t>(0b1100'0000u);
                (*first++) = static_cast<uint8_t>(0b1000'0000u);
                return ConversionResult::Success;
            }
        }

        if (codepoint < (1u << 7u))
        {
            if (first < last)
            {
                (*first++) = static_cast<uint8_t>(codepoint);
                return ConversionResult::Success;
            }
        }

        if (codepoint < (1u << 11u))
        {
            if ((first + 1) < last)
            {
                (*first++) = static_cast<uint8_t>(0b1100'0000u | (0b0001'1111u & (codepoint >> 6u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint)));
                return ConversionResult::Success;
            }
        }

        if (codepoint < (1u << 16u))
        {
            if ((first + 2) < last)
            {
                (*first++) = static_cast<uint8_t>(0b1110'0000u | (0b0000'1111u & (codepoint >> 12u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint >> 6u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint)));
                return ConversionResult::Success;
            }
        }

        if (codepoint < (1u << 21u))
        {
            if ((first + 3) < last)
            {
                (*first++) = static_cast<uint8_t>(0b1111'0000u | (0b0000'0111u & (codepoint >> 18u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint >> 12u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint >> 6u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint)));
                return ConversionResult::Success;
            }
        }

        return ConversionResult::TargetExhausted;
    }

    ConversionResult Decode(char32_t& result, const char16_t*& first, const char16_t* last)
    {
        if (first < last)
        {
            // Consume first element.
            uint32_t const ch = (*first++);

            if (impl::UTF16IsLowSurrogatePair(ch))
            {
                // High surrogate expected here
                result = impl::UNICODE_REPLACEMENT_CHARACTER;
                return ConversionResult::SourceIllegal;
            }

            if (impl::UTF16IsHighSurrogatePair(ch))
            {
                if (first < last)
                {
                    uint32_t const next = *first;

                    if (impl::UTF16IsLowSurrogatePair(next))
                    {
                        ++first;
                        result = impl::UTF16CombineSurrogatePair(ch, next);
                        return ConversionResult::Success;
                    }
                    else
                    {
                        // Consume only high surrogate pair and emit replacement.
                        result = impl::UNICODE_REPLACEMENT_CHARACTER;
                        return ConversionResult::SourceIllegal;
                    }
                }
                else
                {
                    // Tried to decode surrogate pair, but source exhausted prematurely.
                    result = impl::UNICODE_REPLACEMENT_CHARACTER;
                    return ConversionResult::SourceExhausted;
                }
            }

            // Trivial case
            result = ch;
            return ConversionResult::Success;
        }

        // Not enough source.
        result = impl::UNICODE_REPLACEMENT_CHARACTER;
        return ConversionResult::SourceExhausted;
    }

    ConversionResult Encode(char16_t*& first, char16_t* last, char32_t codepoint)
    {
        if (codepoint <= impl::UNICODE_MAX_BMP)
        {
            if (first < last)
            {
                if (impl::UTF16IsSurrogatePair(codepoint))
                {
                    // Surrogate pair range.
                    (*first++) = impl::UNICODE_REPLACEMENT_CHARACTER;
                    return ConversionResult::SourceIllegal;
                }

                // Trivial case
                (*first++) = static_cast<char16_t>(codepoint);
                return ConversionResult::Success;
            }

            return ConversionResult::TargetExhausted;
        }

        if (codepoint <= impl::UNICODE_MAX)
        {
            if ((first + 1) < last)
            {
                // Surrogate pair
                codepoint -= impl::UTF16_SURROGATE_BASE;

                // High surrogate.
                (*first++) = static_cast<char16_t>(impl::UTF16_SURROGATE_HIGH_FIRST + (codepoint >> impl::UTF16_SURROGATE_SHIFT));

                // Low surrogate.
                (*first++) = static_cast<char16_t>(impl::UTF16_SURROGATE_LOW_FIRST + (codepoint & impl::UTF16_SURROGATE_MASK));
                return ConversionResult::Success;
            }

            return ConversionResult::TargetExhausted;
        }

        if (first < last)
        {
            // Invalid range.
            (*first++) = impl::UNICODE_REPLACEMENT_CHARACTER;
            return ConversionResult::SourceIllegal;
        }

        return ConversionResult::TargetExhausted;
    }

    ConversionResult Decode(char32_t& result, const char32_t*& first, const char32_t* last)
    {
        if (first < last)
        {
            char32_t const codepoint = (*first++);

            if (impl::UTF16IsSurrogatePair(codepoint))
            {
                // Byte sequence was valid UTF8 sequence, but encoded invalid codepoint.
                result = impl::UNICODE_REPLACEMENT_CHARACTER;
                return ConversionResult::SourceIllegal;
            }

            result = codepoint;
            return ConversionResult::Success;
        }

        result = impl::UNICODE_REPLACEMENT_CHARACTER;
        return ConversionResult::SourceExhausted;
    }

    ConversionResult Encode(char32_t*& first, char32_t* last, char32_t codepoint)
    {
        if (not IsValidCodePoint(codepoint))
        {
            return ConversionResult::SourceIllegal;
        }

        if (first < last)
        {
            (*first++) = codepoint;
            return ConversionResult::Success;
        }

        return ConversionResult::TargetExhausted;
    }
}

namespace weave::unicode
{
    ConversionResult Convert(
        const uint8_t*& sourceFirst,
        const uint8_t* sourceLast,
        char32_t*& destinationFirst,
        char32_t* destinationLast,
        ConversionType conversionType)
    {
        ConversionResult result = ConversionResult::Success;

        while (sourceFirst < sourceLast)
        {
            const uint8_t* source = sourceFirst;
            char32_t* destination = destinationFirst;

            char32_t codepoint;
            result = Decode(codepoint, source, sourceLast);

            if (conversionType == ConversionType::None)
            {
                if (result != ConversionResult::Success)
                {
                    // Relaxed conversion rules allow to emit replacement character instead of invalid bytes.
                    result = ConversionResult::Success;

                    // Retry with next byte.
                    source = sourceFirst + 1;
                }
            }
            else if (result != ConversionResult::Success)
            {
                break;
            }

            result = Encode(destination, destinationLast, codepoint);

            if (result != ConversionResult::Success)
            {
                break;
            }

            sourceFirst = source;
            destinationFirst = destination;
        }

        return result;
    }

    ConversionResult Convert(
        const char32_t*& sourceFirst,
        const char32_t* sourceLast,
        uint8_t*& destinationFirst,
        uint8_t* destinationLast,
        ConversionType conversionType)
    {
        ConversionResult result = ConversionResult::Success;

        while (sourceFirst < sourceLast)
        {
            const char32_t* source = sourceFirst;
            uint8_t* destination = destinationFirst;

            char32_t codepoint;
            result = Decode(codepoint, source, sourceLast);

            if (conversionType == ConversionType::None)
            {
                if (result != ConversionResult::Success)
                {
                    // Relaxed conversion rules allow to emit replacement character instead of invalid bytes.
                    result = ConversionResult::Success;

                    // Retry with next byte.
                    source = sourceFirst + 1;
                }
            }
            else if (result != ConversionResult::Success)
            {
                break;
            }

            result = Encode(destination, destinationLast, codepoint);

            if (result != ConversionResult::Success)
            {
                break;
            }

            sourceFirst = source;
            destinationFirst = destination;
        }

        return result;
    }

    ConversionResult Convert(
        const uint8_t*& sourceFirst,
        const uint8_t* sourceLast,
        char16_t*& destinationFirst,
        char16_t* destinationLast,
        ConversionType conversionType)
    {
        ConversionResult result = ConversionResult::Success;

        while (sourceFirst < sourceLast)
        {
            const uint8_t* source = sourceFirst;
            char16_t* destination = destinationFirst;

            char32_t codepoint;
            result = Decode(codepoint, source, sourceLast);

            if (conversionType == ConversionType::None)
            {
                if (result != ConversionResult::Success)
                {
                    // Relaxed conversion rules allow to emit replacement character instead of invalid bytes.
                    result = ConversionResult::Success;

                    // Retry with next byte.
                    source = sourceFirst + 1;
                }
            }
            else if (result != ConversionResult::Success)
            {
                break;
            }

            result = Encode(destination, destinationLast, codepoint);

            if (result != ConversionResult::Success)
            {
                break;
            }

            sourceFirst = source;
            destinationFirst = destination;
        }

        return result;
    }

    ConversionResult Convert(
        const char16_t*& sourceFirst,
        const char16_t* sourceLast,
        uint8_t*& destinationFirst,
        uint8_t* destinationLast,
        ConversionType conversionType)
    {
        ConversionResult result = ConversionResult::Success;

        while (sourceFirst < sourceLast)
        {
            const char16_t* source = sourceFirst;
            uint8_t* destination = destinationFirst;

            char32_t codepoint;
            result = Decode(codepoint, source, sourceLast);

            if (conversionType == ConversionType::None)
            {
                if (result != ConversionResult::Success)
                {
                    // Relaxed conversion rules allow to emit replacement character instead of invalid bytes.
                    result = ConversionResult::Success;

                    // Retry with next byte.
                    source = sourceFirst + 1;
                }
            }
            else if (result != ConversionResult::Success)
            {
                break;
            }

            result = Encode(destination, destinationLast, codepoint);

            if (result != ConversionResult::Success)
            {
                break;
            }

            sourceFirst = source;
            destinationFirst = destination;
        }

        return result;
    }

    ConversionResult Convert(
        const char32_t*& sourceFirst,
        const char32_t* sourceLast,
        char16_t*& destinationFirst,
        char16_t* destinationLast,
        ConversionType conversionType)
    {
        ConversionResult result = ConversionResult::Success;

        while (sourceFirst < sourceLast)
        {
            const char32_t* source = sourceFirst;
            char16_t* destination = destinationFirst;

            char32_t codepoint;
            result = Decode(codepoint, source, sourceLast);

            if (conversionType == ConversionType::None)
            {
                if (result != ConversionResult::Success)
                {
                    // Relaxed conversion rules allow to emit replacement character instead of invalid bytes.
                    result = ConversionResult::Success;

                    // Retry with next byte.
                    source = sourceFirst + 1;
                }
            }
            else if (result != ConversionResult::Success)
            {
                break;
            }

            result = Encode(destination, destinationLast, codepoint);

            if (result != ConversionResult::Success)
            {
                break;
            }

            sourceFirst = source;
            destinationFirst = destination;
        }

        return result;
    }

    ConversionResult Convert(
        const char16_t*& sourceFirst,
        const char16_t* sourceLast,
        char32_t*& destinationFirst,
        char32_t* destinationLast,
        ConversionType conversionType)
    {
        ConversionResult result = ConversionResult::Success;

        while (sourceFirst < sourceLast)
        {
            const char16_t* source = sourceFirst;
            char32_t* destination = destinationFirst;

            char32_t codepoint;
            result = Decode(codepoint, source, sourceLast);

            if (conversionType == ConversionType::None)
            {
                if (result != ConversionResult::Success)
                {
                    // Relaxed conversion rules allow to emit replacement character instead of invalid bytes.
                    result = ConversionResult::Success;

                    // Retry with next byte.
                    source = sourceFirst + 1;
                }
            }
            else if (result != ConversionResult::Success)
            {
                break;
            }

            result = Encode(destination, destinationLast, codepoint);

            if (result != ConversionResult::Success)
            {
                break;
            }

            sourceFirst = source;
            destinationFirst = destination;
        }

        return result;
    }
}

namespace weave::unicode
{
    bool Validate(
        const uint8_t* first,
        const uint8_t* last)
    {
        char32_t ch;

        while (first < last)
        {
            if (Decode(ch, first, last) != ConversionResult::Success)
            {
                return false;
            }
        }

        return first == last;
    }

    bool Validate(
        const char16_t* first,
        const char16_t* last)
    {
        char32_t ch;

        while (first < last)
        {
            if (Decode(ch, first, last) != ConversionResult::Success)
            {
                return false;
            }
        }

        return first == last;
    }

    bool Validate(
        const char32_t* first,
        const char32_t* last)
    {
        char32_t ch;

        while (first < last)
        {
            if (Decode(ch, first, last) != ConversionResult::Success)
            {
                return false;
            }
        }

        return first == last;
    }
}
