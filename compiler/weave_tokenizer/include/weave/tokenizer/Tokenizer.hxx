#pragma once
#include "weave/tokenizer/Token.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/source/SourceCursor.hxx"
#include "weave/source/SourceText.hxx"

namespace weave::tokenizer
{
    struct LexerToken final
    {
        TokenKind Kind;
        source::SourceSpan Span;

        std::string_view Prefix{};
        std::string_view Value{};
        std::string_view Suffix{};
    };

    class Tokenizer final
    {
    private:
        source::DiagnosticSink& _diagnostic;
        source::SourceText const& _source;
        source::SourceCursor _cursor;
        TriviaMode _triviaMode;

        std::vector<Trivia> _token_leading_trivia{};
        std::vector<Trivia> _token_trailing_trivia{};
        std::string _token_value{};
        std::string _token_prefix{};
        std::string _token_suffix{};
        source::SourceSpan _token_span{};
        TokenKind _token_kind{TokenKind::Error};

    public:
        explicit Tokenizer(
            source::DiagnosticSink& diagnostic,
            source::SourceText const& source,
            TriviaMode triviaMode)
            : _diagnostic{diagnostic}
            , _source{source}
            , _cursor{source.GetContentView()}
            , _triviaMode{triviaMode}
        {
        }

    public:
        [[nodiscard]] source::DiagnosticSink const& GetDiagnostics() const
        {
            return this->_diagnostic;
        }

        [[nodiscard]] source::SourceText const& GetSource() const
        {
            return this->_source;
        }

        [[nodiscard]] std::string_view GetPrefix() const
        {
            return this->_token_prefix;
        }

        [[nodiscard]] std::string_view GetSuffix() const
        {
            return this->_token_suffix;
        }

        [[nodiscard]] std::string_view GetValue() const
        {
            return this->_token_value;
        }

        [[nodiscard]] source::SourceSpan GetSpan() const
        {
            return this->_token_span;
        }

        [[nodiscard]] TokenKind GetKind() const
        {
            return this->_token_kind;
        }

        [[nodiscard]] std::span<Trivia const> GetLeadingTrivia() const
        {
            return this->_token_leading_trivia;
        }

        [[nodiscard]] std::span<Trivia const> GetTrailingTrivia() const
        {
            return this->_token_trailing_trivia;
        }

    public:
        bool Lex();

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
        struct SingleInteger final
        {
            bool HasValue;
            bool HasSeparator;
            bool HasLeadingSeparator;
            bool HasTrailingSeparator;
        };

    private:
        [[nodiscard]] bool TryReadToken();
        void TryReadTrivia(std::vector<Trivia>& builder, bool leading);

    private:
        [[nodiscard]] bool TryReadRawIdentifier();
        [[nodiscard]] bool TryReadRawStringLiteral();
        [[nodiscard]] bool TryReadDefaultStringLiteral();
        [[nodiscard]] bool TryReadStringPrefix();
        [[nodiscard]] bool TryReadStringLiteral();
        [[nodiscard]] bool TryReadCharacterLiteral();
        [[nodiscard]] bool TryReadNumericLiteral();
        [[nodiscard]] bool TryReadPunctuation();
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
        void TryReadNumberValueType();
    };
}
