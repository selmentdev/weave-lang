#pragma once
#include <cstdint>

namespace weave::lexer
{
    class CharTraits final
    {
    public:
        [[nodiscard]] static constexpr bool IsIdentifierStartAscii(char32_t c)
        {
            return ((U'a' <= c) and (c <= U'z')) or
                ((U'A' <= c) and (c <= U'Z')) or
                (c == U'_');
        }

        [[nodiscard]] static constexpr bool IsIdentifierContinuationAscii(char32_t c)
        {
            return ((U'a' <= c) and (c <= U'z')) or
                ((U'A' <= c) and (c <= U'Z')) or
                ((U'0' <= c) and (c <= U'9')) or
                (c == U'_');
        }

        [[nodiscard]] static constexpr bool IsDecimalExponent(char32_t c)
        {
            return (c == U'e') or (c == U'E');
        }

        [[nodiscard]] static constexpr bool IsHexadecimalExponent(char32_t c)
        {
            return (c == U'p') or (c == U'P');
        }

        [[nodiscard]] static bool IsIdentifierStart(char32_t c);

        [[nodiscard]] static bool IsIdentifierContinuation(char32_t c);

        [[nodiscard]] static bool IsWhitespace(char32_t c);

        [[nodiscard]] static constexpr bool IsBinaryDigit(char32_t c)
        {
            return (c == U'0') or (c == U'1');
        }

        [[nodiscard]] static constexpr bool IsOctalDigit(char32_t c)
        {
            return (U'0' <= c) and (c <= U'7');
        }

        [[nodiscard]] static constexpr bool IsDecimalDigit(char32_t c)
        {
            return (U'0' <= c) and (c <= U'9');
        }

        [[nodiscard]] static constexpr bool IsHexadecimalDigit(char32_t c)
        {
            return ((U'0' <= c) and (c <= U'9')) or
                ((U'a' <= c) and (c <= U'f')) or
                ((U'A' <= c) and (c <= U'F'));
        }

        [[nodiscard]] static constexpr bool IsNewLine(char32_t c)
        {
            return (c == U'\n') or (c == U'\r');
        }
    };
}
