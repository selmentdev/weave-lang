#include "weave/Unicode.hxx"
#include <cstddef>
#include <cstdint>

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

    constexpr bool utf16_is_surrogate_pair(char32_t value)
    {
        return (UTF16_SURROGATE_FIRST <= value) and (value <= UTF16_SURROGATE_LAST);
    }

    constexpr bool utf16_is_low_surrogate_pair(char32_t value)
    {
        return (UTF16_SURROGATE_LOW_FIRST <= value) and (value <= UTF16_SURROGATE_LOW_LAST);
    }

    constexpr bool utf16_is_high_surrogate_pair(char32_t value)
    {
        return (UTF16_SURROGATE_HIGH_FIRST <= value) and (value <= UTF16_SURROGATE_HIGH_LAST);
    }

    constexpr uint32_t utf16_combine_surrogate_pair(uint32_t high, uint32_t low)
    {
        return ((high - UTF16_SURROGATE_HIGH_FIRST) << UTF16_SURROGATE_SHIFT) + (low - UTF16_SURROGATE_LOW_FIRST) + UTF16_SURROGATE_BASE;
    }

    constexpr uint32_t utf8_first_byte(uint8_t byte, size_t width)
    {
        return (byte & (0b0111'1111u >> width));
    }

    constexpr uint32_t utf8_accumulate_continuation_byte(uint32_t ch, uint8_t byte)
    {
        return (ch << 6) | (byte & UTF8_CONTINUATION_MASK);
    }

    constexpr bool utf8_is_continuation_byte(uint8_t byte)
    {
        return static_cast<int8_t>(byte) < -64;
    }
    
}

namespace weave::unicode
{
    bool utf8_is_legal(const uint8_t* it, size_t length)
    {
        uint8_t c;

        const uint8_t* end = it + length;

        switch (length)
        {
        default:
            return false;

        case 4: // NOLINT(bugprone-branch-clone)
            if (((c = (*--end)) < 0x80u) or (c > 0xBFu))
            {
                return false;
            }

            [[fallthrough]];

        case 3:
            if (((c = (*--end)) < 0x80u) or (c > 0xBFu))
            {
                return false;
            }

            [[fallthrough]];

        case 2:
            if (((c = (*--end)) < 0x80u) or (c > 0xBFu))
            {
                return false;
            }

            switch (*it)
            {
            case 0xE0u:
                if (c < 0xA0u)
                {
                    return false;
                }

                break;

            case 0xEDu:
                if (c > 0x9Fu)
                {
                    return false;
                }

                break;

            case 0xF0:
                if (c < 0x90u)
                {
                    return false;
                }

                break;

            case 0xF4:
                if (c > 0x8Fu)
                {
                    return false;
                }

                break;

            default:
                if (c < 0x80)
                {
                    return false;
                }

                break;
            }

            [[fallthrough]];

        case 1:
            if ((*it >= 0x80u) and (*it < 0xC2))
            {
                return false;
            }
        }

        if (*it > 0xF4)
        {
            return false;
        }

        return true;
    }

    bool utf8_is_legal(const uint8_t* first, const uint8_t* last)
    {
        size_t const length = impl::UTF8_CHAR_WIDTH[*first];

        if (length > static_cast<size_t>(last - first))
        {
            return false;
        }

        return utf8_is_legal(first, length);
    }

    size_t utf8_sequence_size(const uint8_t* first, const uint8_t* last)
    {
        size_t const length = impl::UTF8_CHAR_WIDTH[*first];

        if ((length <= static_cast<size_t>(last - first)) and utf8_is_legal(first, length))
        {
            return length;
        }

        return 0u;
    }

    bool utf8_validate_string(const uint8_t* first, const uint8_t* last)
    {
        while (first < last)
        {
            size_t const skip = utf8_sequence_size(first, last);

            if (skip == 0)
            {
                // Malformed UTF8 sequence.
                break;
            }

            first += skip;
        }

        return first == last;
    }

    bool utf32_is_valid_code_point(char32_t value) noexcept
    {
        if (value <= impl::UNICODE_MAX)
        {
            return not impl::utf16_is_surrogate_pair(value);
        }

        return false;
    }

    char32_t utf32_sanitize_codepoint(char32_t value) noexcept
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

    UnicodeConversionResult utf8_decode(char32_t& codepoint, const uint8_t*& first, const uint8_t* last)
    {
        if (first < last)
        {
            uint8_t const lead = *first++;

            if (lead <= 0x7Fu)
            {
                codepoint = lead;
                return UnicodeConversionResult::Success;
            }

            size_t const width = impl::UTF8_CHAR_WIDTH[lead];

            if (width == 0)
            {
                // Lead byte is not valid.
                codepoint = impl::UNICODE_REPLACEMENT_CHARACTER;
                return UnicodeConversionResult::SourceIllegal;
            }

            uint32_t ch = impl::utf8_first_byte(lead, width);

            size_t consumed = 1;

            for (; (consumed < width) and (first < last); ++consumed)
            {
                uint8_t const next = static_cast<uint8_t>(*first++);

                if (not impl::utf8_is_continuation_byte(next))
                {
                    // Invalid byte sequence.
                    --first;
                    codepoint = impl::UNICODE_REPLACEMENT_CHARACTER;
                    return UnicodeConversionResult::SourceIllegal;
                }

                ch = impl::utf8_accumulate_continuation_byte(ch, next);
            }

            if (consumed != width)
            {
                // Byte sequence was legal, but source exhausted. Restore iterator and return.
                first -= consumed;
                return UnicodeConversionResult::SourceExhausted;
            }

            // ch -= Private::UTF8_CODEPOINT_OFFSETS[width];

            if (impl::utf16_is_surrogate_pair(ch) or (ch > impl::UNICODE_MAX))
            {
                // Byte sequence was valid UTF8 sequence, but encoded invalid codepoint.
                codepoint = impl::UNICODE_REPLACEMENT_CHARACTER;
                return UnicodeConversionResult::SourceIllegal;
            }

            codepoint = ch;
            return UnicodeConversionResult::Success;
        }

        return UnicodeConversionResult::SourceExhausted;
    }

    UnicodeConversionResult utf8_encode(uint8_t*& first, uint8_t* last, char32_t codepoint)
    {
        if (not utf32_is_valid_code_point(codepoint))
        {
            return UnicodeConversionResult::SourceIllegal;
        }

        if (codepoint == 0u)
        {
            if ((first + 1) < last)
            {
                (*first++) = static_cast<uint8_t>(0b1100'0000u);
                (*first++) = static_cast<uint8_t>(0b1000'0000u);
                return UnicodeConversionResult::Success;
            }
        }

        if (codepoint < (1u << 7u))
        {
            if (first < last)
            {
                (*first++) = static_cast<uint8_t>(codepoint);
                return UnicodeConversionResult::Success;
            }
        }

        if (codepoint < (1u << 11u))
        {
            if ((first + 1) < last)
            {
                (*first++) = static_cast<uint8_t>(0b1100'0000u | (0b0001'1111u & (codepoint >> 6u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint)));
                return UnicodeConversionResult::Success;
            }
        }

        if (codepoint < (1u << 16u))
        {
            if ((first + 2) < last)
            {
                (*first++) = static_cast<uint8_t>(0b1110'0000u | (0b0000'1111u & (codepoint >> 12u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint >> 6u)));
                (*first++) = static_cast<uint8_t>(0b1000'0000u | (0b0011'1111u & (codepoint)));
                return UnicodeConversionResult::Success;
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
                return UnicodeConversionResult::Success;
            }
        }

        return UnicodeConversionResult::TargetExhausted;
    }
}
