#include "Compiler.Lexer/CharTraits.hxx"

namespace Weave::Lexer
{
    bool CharTraits::IsIdentifierStart(char32_t c)
    {
        if (not IsIdentifierContinuation(c))
        {
            return false;
        }

        if ((c < 0x80) and (IsDecimalDigit(c) or (c == U'$')))
        {
            return false;
        }

        // N1518: Recommendations for extended identifier characters for C and C++
        // Proposed Annex X.2: Ranges of characters disallowed initially
        if (((0x0300 <= c) and (c <= 0x036F)) or
            ((0x1DC0 <= c) and (c <= 0x1DFF)) or
            ((0x20D0 <= c) and (c <= 0x20FF)) or
            ((0xFE20 <= c) and (c <= 0xFE2F)))
        {
            return false;
        }

        return true;
    }

    bool CharTraits::IsIdentifierContinuation(char32_t c)
    {
        if (c < 0x80u)
        {
            return IsIdentifierContinuationAscii(c);
        }

        // N1518: Recommendations for extended identifier characters for C and C++
        // Proposed Annex X.1: Ranges of characters allowed
        return (c == 0x00A8) or
            (c == 0x00AA) or
            (c == 0x00AD) or
            (c == 0x00AF) or
            ((0x00B2 <= c) and (c <= 0x00B5)) or
            ((0x00B7 <= c) and (c <= 0x00BA)) or
            ((0x00BC <= c) and (c <= 0x00BE)) or
            ((0x00C0 <= c) and (c <= 0x00D6)) or
            ((0x00D8 <= c) and (c <= 0x00F6)) or
            ((0x00F8 <= c) and (c <= 0x00FF)) or
            ((0x0100 <= c) and (c <= 0x167F)) or
            ((0x1681 <= c) and (c <= 0x180D)) or
            ((0x180F <= c) and (c <= 0x1FFF)) or
            ((0x200B <= c) and (c <= 0x200D)) or
            ((0x202A <= c) and (c <= 0x202E)) or
            ((0x203F <= c) and (c <= 0x2040)) or
            (0x2054 == c) or
            ((0x2060 <= c) and c <= (0x206F)) or
            ((0x2070 <= c) and c <= (0x218F)) or
            ((0x2460 <= c) and c <= (0x24FF)) or
            ((0x2776 <= c) and c <= (0x2793)) or
            ((0x2C00 <= c) and c <= (0x2DFF)) or
            ((0x2E80 <= c) and c <= (0x2FFF)) or
            ((0x3004 <= c) and c <= (0x3007)) or
            ((0x3021 <= c) and c <= (0x302F)) or
            ((0x3031 <= c) and c <= (0x303F)) or
            ((0x3040 <= c) and c <= (0xD7FF)) or
            ((0xF900 <= c) and c <= (0xFD3D)) or
            ((0xFD40 <= c) and c <= (0xFDCF)) or
            ((0xFDF0 <= c) and c <= (0xFE44)) or
            ((0xFE47 <= c) and c <= (0xFFF8)) or
            ((0x10000 <= c) and (c <= 0x1FFFD)) or
            ((0x20000 <= c) and (c <= 0x2FFFD)) or
            ((0x30000 <= c) and (c <= 0x3FFFD)) or
            ((0x40000 <= c) and (c <= 0x4FFFD)) or
            ((0x50000 <= c) and (c <= 0x5FFFD)) or
            ((0x60000 <= c) and (c <= 0x6FFFD)) or
            ((0x70000 <= c) and (c <= 0x7FFFD)) or
            ((0x80000 <= c) and (c <= 0x8FFFD)) or
            ((0x90000 <= c) and (c <= 0x9FFFD)) or
            ((0xA0000 <= c) and (c <= 0xAFFFD)) or
            ((0xB0000 <= c) and (c <= 0xBFFFD)) or
            ((0xC0000 <= c) and (c <= 0xCFFFD)) or
            ((0xD0000 <= c) and (c <= 0xDFFFD)) or
            ((0xE0000 <= c) and (c <= 0xEFFFD));
    }

    bool CharTraits::IsWhitespace(char32_t c)
    {
        switch (c)
        {
            // New line characters are lexed separately
            // case 0x000A: // \n
            // case 0x000D: // \r

        case 0x0009: // tab
        case 0x000B: // vertical tab
        case 0x000C: // form feed
        case 0x0020: // space

        case 0x0085: // next line

        case 0x200E: // LEFT-TO-RIGHT MARK
        case 0x200F: // RIGHT-TO-LEFT MARK

        case 0x2028: // LINE SEPARATOR
        case 0x2029: // PARAGRAPH SEPARATOR
            return true;

        default:
            break;
        }

        return false;
    }
}
