#include "weave/Unicode.hxx"
#include "weave/BugCheck.hxx"

namespace weave::unicode::impl
{
    // https://bjoern.hoehrmann.de/utf-8/decoder/dfa/

    constexpr uint32_t UTF8_ACCEPT = 0;
    constexpr uint32_t UTF8_REJECT = 12;

    constexpr uint8_t utf8d[] = {
        // clang-format off
        // The first part of the table maps bytes to character classes that
        // to reduce the size of the transition table and create bitmasks.
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
         9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
         7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
         7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
         8,  8,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
         2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
        10,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  3,  3,
        11,  6,  6,  6,  5,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,

        // The second part is a transition table that maps a combination
        // of a state of the automaton and a character class to a state.
         0, 12, 24, 36, 60, 96, 84, 12, 12, 12, 48, 72, 12, 12, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 12, 12,  0, 12, 12, 12, 12, 12,  0,
        12,  0, 12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 24, 12, 12,
        12, 12, 12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 24, 12, 12,
        12, 12, 12, 12, 12, 24, 12, 12, 12, 12, 12, 12, 12, 12, 12, 36,
        12, 36, 12, 12, 12, 36, 12, 12, 12, 12, 12, 36, 12, 36, 12, 12,
        12, 36, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
        // clang-format on
    };

    constexpr void DecodeUTF8(uint32_t& state, uint32_t& codep, uint32_t byte)
    {
        uint32_t const type = utf8d[byte];

        codep = (state != UTF8_ACCEPT) ? (byte & 0x3fu) | (codep << 6) : (0xff >> type) & (byte);

        state = utf8d[256 + state + type];
    }
}

namespace weave::unicode::impl
{
    // https://tools.ietf.org/html/rfc3629
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
        uint32_t state{};
        uint32_t codepoint{};

        while (first != last)
        {
            impl::DecodeUTF8(state, codepoint, *first++);

            switch (state)
            {
            case impl::UTF8_ACCEPT:
                result = static_cast<char32_t>(codepoint);
                return ConversionResult::Success;

            case impl::UTF8_REJECT:
                result = impl::UNICODE_REPLACEMENT_CHARACTER;
                return ConversionResult::SourceIllegal;

            default:
                break;
            }
        }

        result = impl::UNICODE_REPLACEMENT_CHARACTER;
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
