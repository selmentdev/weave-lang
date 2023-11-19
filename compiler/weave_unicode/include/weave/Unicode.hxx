#pragma once
#include <cstdint>
#include <string>

namespace weave::unicode
{
    enum class UnicodeConversionResult
    {
        Success,
        SourceIllegal,
        TargetExhausted,
        SourceExhausted,
    };

    [[nodiscard]] UnicodeConversionResult utf8_decode(char32_t& codepoint, const uint8_t*& first, const uint8_t* last);

    [[nodiscard]] UnicodeConversionResult utf8_encode(uint8_t*& first, uint8_t* last, char32_t codepoint);

    [[nodiscard]] bool utf8_validate_string(const uint8_t* first, const uint8_t* last);
}

namespace weave::unicode
{
    [[nodiscard]] inline UnicodeConversionResult utf8_decode(char32_t& codepoint, const char*& first, const char* last)
    {
        const uint8_t* c8first = reinterpret_cast<const uint8_t*>(first);
        const uint8_t* c8last = reinterpret_cast<const uint8_t*>(last);

        UnicodeConversionResult const result = utf8_decode(codepoint, c8first, c8last);

        first = reinterpret_cast<const char*>(c8first);

        return result;
    }

    [[nodiscard]] inline UnicodeConversionResult utf8_encode(char*& first, char* last, char32_t codepoint)
    {
        uint8_t* c8first = reinterpret_cast<uint8_t*>(first);
        uint8_t* c8last = reinterpret_cast<uint8_t*>(last);

        UnicodeConversionResult const result = utf8_encode(c8first, c8last, codepoint);

        first = reinterpret_cast<char*>(c8first);

        return result;
    }

    [[nodiscard]] inline bool utf8_validate_string(std::string_view value)
    {
        const uint8_t* first = reinterpret_cast<const uint8_t*>(value.data());
        const uint8_t* last = first + value.size();

        return utf8_validate_string(first, last);
    }
}
