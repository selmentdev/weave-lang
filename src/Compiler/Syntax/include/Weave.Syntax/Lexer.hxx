#pragma once
#include "LexerContext.hxx"
#include "Weave.Syntax/Token.hxx"
#include "Weave.Core/Diagnostic.hxx"
#include "Weave.Core/SourceCursor.hxx"
#include "Weave.Core/SourceText.hxx"

#include <variant>

namespace Weave::Syntax
{
    class Lexer final
    {
    private:
        DiagnosticSink& _diagnostic;
        LexerContext& _context;
        SourceText const& _source;
        SourceCursor _cursor;
        TriviaMode _triviaMode;

        std::vector<Trivia> _leadingTriviaBuilder{};
        std::vector<Trivia> _trailingTriviaBuilder{};
        std::string _buffer{};

    public:
        explicit Lexer(
            DiagnosticSink& _diagnostic,
            LexerContext& context,
            SourceText const& source,
            TriviaMode triviaMode)
            : _diagnostic{_diagnostic}
            , _context{context}
            , _source{source}
            , _cursor{source.GetContent()}
            , _triviaMode{triviaMode}
        {
        }

    public:
        [[nodiscard]] DiagnosticSink const& GetDiagnostics() const
        {
            return this->_diagnostic;
        }

        [[nodiscard]] SourceText const& GetSource() const
        {
            return this->_source;
        }

    public:
        Token* Lex();

    private:
        static constexpr bool AppendDecCharacter(uint64_t& result, char32_t c)
        {
            if ((U'0' <= c) and (c <= U'9'))
            {
                uint64_t const digit = (c - U'0');

                result *= 10;
                result += digit;
                return true;
            }

            return false;
        }

        static constexpr bool AppendHexCharacter(uint64_t& result, char32_t c)
        {
            uint64_t digit = 16;

            if ((U'0' <= c) and (c <= U'9'))
            {
                digit = (c - U'0');
            }
            else if ((U'a' <= c) and (c <= U'f'))
            {
                digit = (c - U'a') + 10;
            }
            else if ((U'A' <= c) and (c <= U'F'))
            {
                digit = (c - U'A') + 10;
            }

            bool const valid = digit < 16;

            if (valid)
            {
                result <<= 4u;
                result |= digit;
            }

            return valid;
        }

    private:
        struct TokenInfo final
        {
            TokenKind Kind{};
            SourceSpan Span{};

            // Note:
            //   - use builtin value type instead of suffixes - lexer knows about all known types already,
            //   - prefix is used to determine the type of the literal - once we parse actual value, we don't need it anymore as value is stored in widest possible type,

            std::variant<std::string, double, uint64_t> Value{};
            // Value Type

            std::optional<NumberLiteralPrefixKind> NumberPrefix{};
            std::optional<StringPrefixKind> StringPrefix{};
            std::optional<CharacterPrefixKind> CharacterPrefix{};
            std::optional<IntegerLiteralSuffixKind> IntegerSuffix{};
            std::optional<FloatLiteralSuffixKind> FloatSuffix{};
        };

        struct SingleInteger final
        {
            bool HasValue;
            bool HasSeparator;
            bool HasLeadingSeparator;
            bool HasTrailingSeparator;
        };

    private:
        [[nodiscard]] bool TryReadToken(TokenInfo& info);
        void TryReadTrivia(std::vector<Trivia>& builder, bool leading);

    private:
        [[nodiscard]] bool TryReadRawIdentifier();
        [[nodiscard]] bool TryReadRawStringLiteral();
        [[nodiscard]] bool TryReadStringLiteral();
        [[nodiscard]] bool TryReadCharacterLiteral();
        [[nodiscard]] bool TryReadNumericLiteral(TokenInfo& info);
        [[nodiscard]] bool TryReadPunctuation(TokenInfo& info);
        [[nodiscard]] bool TryReadEndOfLine();
        [[nodiscard]] bool TryReadWhitespace();
        [[nodiscard]] TriviaKind TryReadSingleLineComment();
        [[nodiscard]] TriviaKind TryReadMultiLineComment();
        [[nodiscard]] TriviaKind TryReadComment();
        [[nodiscard]] bool TryReadIdentifier();

    private:
        [[nodiscard]] SingleInteger TryReadSingleInteger(int base);

        [[nodiscard]] size_t TryReadDecNumber(uint64_t& result);
        [[nodiscard]] size_t TryReadHexNumber(uint64_t& result);
        [[nodiscard]] size_t TryReadDecNumber(uint64_t& result, size_t maxLength);
        [[nodiscard]] size_t TryReadHexNumber(uint64_t& result, size_t maxLength);

        [[nodiscard]] bool TryReadHexEscapeSequence();
        [[nodiscard]] bool TryAppendUnicodeCodepoint(char32_t c);
        [[nodiscard]] bool TryReadTrivialEscapeSequence();
        [[nodiscard]] bool TryReadUnicodeEscapeSequence();
        [[nodiscard]] bool TryReadEscapeSequence();
        [[nodiscard]] bool TryReadStringOrCharacterLiteralCore(char32_t specifier, size_t& consumed);
        [[nodiscard]] int TryReadNumberLiteralPrefixWithRadix();
        void TryReadNumberValueType(TokenInfo& info);
    };
}
