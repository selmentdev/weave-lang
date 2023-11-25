#pragma once
#include <cstdint>
#include <string>
#include <expected>

namespace weave::unicode
{
    enum class ConversionResult
    {
        Success,
        SourceIllegal,
        TargetExhausted,
        SourceExhausted,
    };

    enum class ConversionType
    {
        None,
        Strict,
    };
}

namespace weave::unicode
{
    bool IsValidCodePoint(char32_t value);

    char32_t SanitizeCodepoint(char32_t value);
}

namespace weave::unicode
{
    ConversionResult Decode(char32_t& result, const uint8_t*& first, const uint8_t* last);

    inline ConversionResult Decode(char32_t& result, const char8_t*& first, const char8_t* last)
    {
        const uint8_t* c8first = reinterpret_cast<const uint8_t*>(first);
        const uint8_t* c8last = reinterpret_cast<const uint8_t*>(last);

        ConversionResult const ret = Decode(result, c8first, c8last);

        first = reinterpret_cast<const char8_t*>(c8first);

        return ret;
    }

    inline ConversionResult Decode(char32_t& result, const char*& first, const char* last)
    {
        const uint8_t* c8first = reinterpret_cast<const uint8_t*>(first);
        const uint8_t* c8last = reinterpret_cast<const uint8_t*>(last);

        ConversionResult const ret = Decode(result, c8first, c8last);

        first = reinterpret_cast<const char*>(c8first);

        return ret;
    }

    ConversionResult Encode(uint8_t*& first, uint8_t* last, char32_t codepoint);

    inline ConversionResult Encode(char8_t*& first, char8_t* last, char32_t codepoint)
    {
        uint8_t* c8first = reinterpret_cast<uint8_t*>(first);
        uint8_t* c8last = reinterpret_cast<uint8_t*>(last);

        ConversionResult const ret = Encode(c8first, c8last, codepoint);

        first = reinterpret_cast<char8_t*>(c8first);

        return ret;
    }

    inline ConversionResult Encode(char*& first, char* last, char32_t codepoint)
    {
        uint8_t* c8first = reinterpret_cast<uint8_t*>(first);
        uint8_t* c8last = reinterpret_cast<uint8_t*>(last);

        ConversionResult const ret = Encode(c8first, c8last, codepoint);

        first = reinterpret_cast<char*>(c8first);

        return ret;
    }

    ConversionResult Decode(char32_t& result, const char16_t*& first, const char16_t* last);

    ConversionResult Encode(char16_t*& first, char16_t* last, char32_t codepoint);

    ConversionResult Decode(char32_t& result, const char32_t*& first, const char32_t* last);

    ConversionResult Encode(char32_t*& first, char32_t* last, char32_t codepoint);
}

namespace weave::unicode
{
    ConversionResult Convert(
        const uint8_t*& sourceFirst,
        const uint8_t* sourceLast,
        char32_t*& destinationFirst,
        char32_t* destinationLast,
        ConversionType conversionType);

    ConversionResult Convert(
        const char32_t*& sourceFirst,
        const char32_t* sourceLast,
        uint8_t*& destinationFirst,
        uint8_t* destinationLast,
        ConversionType conversionType);

    ConversionResult Convert(
        const uint8_t*& sourceFirst,
        const uint8_t* sourceLast,
        char16_t*& destinationFirst,
        char16_t* destinationLast,
        ConversionType conversionType);

    ConversionResult Convert(
        const char16_t*& sourceFirst,
        const char16_t* sourceLast,
        uint8_t*& destinationFirst,
        uint8_t* destinationLast,
        ConversionType conversionType);

    ConversionResult Convert(
        const char32_t*& sourceFirst,
        const char32_t* sourceLast,
        char16_t*& destinationFirst,
        char16_t* destinationLast,
        ConversionType conversionType);

    ConversionResult Convert(
        const char16_t*& sourceFirst,
        const char16_t* sourceLast,
        char32_t*& destinationFirst,
        char32_t* destinationLast,
        ConversionType conversionType);
}

namespace weave::unicode
{
    inline ConversionResult Convert(
        const char16_t*& sourceFirst,
        const char16_t* sourceLast,
        char8_t*& destinationFirst,
        char8_t* destinationLast,
        ConversionType conversionType)
    {
        uint8_t* destination = reinterpret_cast<uint8_t*>(destinationFirst);
        ConversionResult const result = Convert(
            sourceFirst,
            sourceLast,
            destination,
            reinterpret_cast<uint8_t*>(destinationLast),
            conversionType);
        destinationFirst = reinterpret_cast<char8_t*>(destination);
        return result;
    }

    inline ConversionResult Convert(
        const char16_t*& sourceFirst,
        const char16_t* sourceLast,
        char*& destinationFirst,
        char* destinationLast,
        ConversionType conversionType)
    {
        uint8_t* destination = reinterpret_cast<uint8_t*>(destinationFirst);
        ConversionResult const result = Convert(
            sourceFirst,
            sourceLast,
            destination,
            reinterpret_cast<uint8_t*>(destinationLast),
            conversionType);
        destinationFirst = reinterpret_cast<char*>(destination);
        return result;
    }

    inline ConversionResult Convert(
        const char32_t*& sourceFirst,
        const char32_t* sourceLast,
        char8_t*& destinationFirst,
        char8_t* destinationLast,
        ConversionType conversionType)
    {
        uint8_t* destination = reinterpret_cast<uint8_t*>(destinationFirst);
        ConversionResult const result = Convert(
            sourceFirst,
            sourceLast,
            destination,
            reinterpret_cast<uint8_t*>(destinationLast),
            conversionType);
        destinationFirst = reinterpret_cast<char8_t*>(destination);
        return result;
    }

    inline ConversionResult Convert(
        const char32_t*& sourceFirst,
        const char32_t* sourceLast,
        char*& destinationFirst,
        char* destinationLast,
        ConversionType conversionType)
    {
        uint8_t* destination = reinterpret_cast<uint8_t*>(destinationFirst);
        ConversionResult const result = Convert(
            sourceFirst,
            sourceLast,
            destination,
            reinterpret_cast<uint8_t*>(destinationLast),
            conversionType);
        destinationFirst = reinterpret_cast<char*>(destination);
        return result;
    }

    inline ConversionResult Convert(
        const char8_t*& sourceFirst,
        const char8_t* sourceLast,
        char16_t*& destinationFirst,
        char16_t* destinationLast,
        ConversionType conversionType)
    {
        const uint8_t* source = reinterpret_cast<const uint8_t*>(sourceFirst);
        ConversionResult const result = Convert(
            source,
            reinterpret_cast<const uint8_t*>(sourceLast),
            destinationFirst,
            destinationLast,
            conversionType);
        sourceFirst = reinterpret_cast<const char8_t*>(source);
        return result;
    }

    inline ConversionResult Convert(
        const char*& sourceFirst,
        const char* sourceLast,
        char16_t*& destinationFirst,
        char16_t* destinationLast,
        ConversionType conversionType)
    {
        const uint8_t* source = reinterpret_cast<const uint8_t*>(sourceFirst);
        ConversionResult const result = Convert(
            source,
            reinterpret_cast<const uint8_t*>(sourceLast),
            destinationFirst,
            destinationLast,
            conversionType);
        sourceFirst = reinterpret_cast<const char*>(source);
        return result;
    }

    inline ConversionResult Convert(
        const char8_t*& sourceFirst,
        const char8_t* sourceLast,
        char32_t*& destinationFirst,
        char32_t* destinationLast,
        ConversionType conversionType)
    {
        const uint8_t* source = reinterpret_cast<const uint8_t*>(sourceFirst);
        ConversionResult const result = Convert(
            source,
            reinterpret_cast<const uint8_t*>(sourceLast),
            destinationFirst,
            destinationLast,
            conversionType);
        sourceFirst = reinterpret_cast<const char8_t*>(source);
        return result;
    }

    inline ConversionResult Convert(
        const char*& sourceFirst,
        const char* sourceLast,
        char32_t*& destinationFirst,
        char32_t* destinationLast,
        ConversionType conversionType)
    {
        const uint8_t* source = reinterpret_cast<const uint8_t*>(sourceFirst);
        ConversionResult const result = Convert(
            source,
            reinterpret_cast<const uint8_t*>(sourceLast),
            destinationFirst,
            destinationLast,
            conversionType);
        sourceFirst = reinterpret_cast<const char*>(source);
        return result;
    }
}

namespace weave::unicode
{
    template <typename CharTo, typename CharFrom>
    ConversionResult Convert(
        std::basic_string<CharTo>& destination,
        std::basic_string_view<CharFrom> source,
        ConversionType conversionType)
    {
        std::array<CharTo, 8> stackBuffer{};

        CharFrom const* sourceStart = source.data();
        CharFrom const* sourceEnd = sourceStart + source.size();

        CharTo* destinationStart = stackBuffer.data();
        CharTo* destinationEnd = destinationStart + stackBuffer.size();

        while (sourceStart < sourceEnd)
        {
            switch (auto const result = Convert(
                        sourceStart,
                        sourceEnd,
                        destinationStart,
                        destinationEnd,
                        conversionType))
            {
            case ConversionResult::Success:
                {
                    // Buffer was large enough to hold the entire string.
                    destination.append(stackBuffer.data(), destinationStart);
                    return result;
                }

            case ConversionResult::TargetExhausted:
                {
                    // Stack buffer was not large enough to hold the entire string. Append processed data to the destination string and continue.
                    destination.append(stackBuffer.data(), destinationStart);
                    destinationStart = stackBuffer.data();
                    continue;
                }

            default:
                {
                    return result;
                }
            }
        }

        // Empty source
        return ConversionResult::Success;
    }

    template <typename CharTo, typename CharFrom>
    std::expected<std::basic_string<CharTo>, ConversionResult> Convert(std::basic_string_view<CharFrom> source, ConversionType conversionType)
    {
        std::basic_string<CharTo> destination{};
        ConversionResult const result = Convert<CharTo, CharFrom>(destination, source, conversionType);

        if (result == ConversionResult::Success)
        {
            return destination;
        }

        return std::unexpected(result);
    }
}

namespace weave::unicode
{
    bool Validate(
        const char32_t* first,
        const char32_t* last);

    bool Validate(
        const char16_t* first,
        const char16_t* last);

    bool Validate(
        const uint8_t* first,
        const uint8_t* last);

    inline bool Validate(
        const char* first,
        const char* last)
    {
        return Validate(
            reinterpret_cast<const uint8_t*>(first),
            reinterpret_cast<const uint8_t*>(last));
    }

    inline bool Validate(
        const char8_t* first,
        const char8_t* last)
    {
        return Validate(
            reinterpret_cast<const uint8_t*>(first),
            reinterpret_cast<const uint8_t*>(last));
    }

    inline bool Validate(
        std::string_view value)
    {
        const char* first = value.data();
        const char* last = first + value.size();
        return Validate(first, last);
    }

    inline bool Validate(
        std::u8string_view value)
    {
        const char8_t* first = value.data();
        const char8_t* last = first + value.size();
        return Validate(first, last);
    }

    inline bool Validate(
        std::u16string_view value)
    {
        const char16_t* first = value.data();
        const char16_t* last = first + value.size();
        return Validate(first, last);
    }

    inline bool Validate(
        std::u32string_view value)
    {
        const char32_t* first = value.data();
        const char32_t* last = first + value.size();
        return Validate(first, last);
    }
}
