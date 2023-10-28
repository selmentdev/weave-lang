#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <span>
#include <optional>

namespace Weave
{
    enum class UnicodeConversionResult
    {
        Success,
        SourceIllegal,
        TargetExhausted,
        SourceExhausted,
    };

    [[nodiscard]] UnicodeConversionResult UTF8Decode(char32_t& codepoint, const uint8_t*& first, const uint8_t* last);

    [[nodiscard]] UnicodeConversionResult UTF8Encode(uint8_t*& first, uint8_t* last, char32_t codepoint);

    [[nodiscard]] bool UTF8ValidateString(const uint8_t* first, const uint8_t* last);
}

namespace Weave
{
    [[nodiscard]] inline UnicodeConversionResult UTF8Decode(char32_t& codepoint, const char*& first, const char* last)
    {
        const uint8_t* c8first = reinterpret_cast<const uint8_t*>(first);
        const uint8_t* c8last = reinterpret_cast<const uint8_t*>(last);

        UnicodeConversionResult const result = UTF8Decode(codepoint, c8first, c8last);

        first = reinterpret_cast<const char*>(c8first);

        return result;
    }

    [[nodiscard]] inline UnicodeConversionResult UTF8Encode(char*& first, char* last, char32_t codepoint)
    {
        uint8_t* c8first = reinterpret_cast<uint8_t*>(first);
        uint8_t* c8last = reinterpret_cast<uint8_t*>(last);

        UnicodeConversionResult const result = UTF8Encode(c8first, c8last, codepoint);

        first = reinterpret_cast<char*>(c8first);

        return result;
    }

    [[nodiscard]] inline bool UTF8ValidateString(std::string_view value)
    {
        const uint8_t* first = reinterpret_cast<const uint8_t*>(value.data());
        const uint8_t* last = first + value.size();

        return UTF8ValidateString(first, last);
    }
}
