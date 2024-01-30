#pragma once
#include "weave/syntax/SyntaxToken.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/source/SourceCursor.hxx"
#include "weave/source/SourceText.hxx"

namespace weave::syntax
{
    class SyntaxFactory;

    struct TokenInfo
    {
        std::vector<SyntaxTrivia> LeadingTrivia{};
        std::vector<SyntaxTrivia> TrailingTrivia{};
        LiteralPrefixKind Prefix;
        std::string Value{};
        std::string Suffix{};
        source::SourceSpan Source{};
        SyntaxKind Kind{};
        SyntaxKind ContextualKeyword{};
    };

    enum class LexerTriviaMode
    {
        None,
        Documentation,
        All
    };

    class Lexer final
    {
    private:
        source::DiagnosticSink* _diagnostic{};
        source::SourceText const* _source{};
        source::SourceCursor _cursor;
        LexerTriviaMode _mode{};

        TokenInfo _token{};

    public:
        Lexer(
            source::DiagnosticSink& diagnostic,
            source::SourceText const& source,
            LexerTriviaMode mode)
            : _diagnostic{&diagnostic}
            , _source{&source}
            , _cursor{source.GetContentView()}
            , _mode{mode}
        {
        }

    public:
        [[nodiscard]] bool Lex(TokenInfo& token);

        [[nodiscard]] SyntaxToken* Lex(SyntaxFactory& factory);

    private:
        struct SingleInteger final
        {
            bool HasValue;
            bool HasSeparator;
            bool HasLeadingSeparator;
            bool HasTrailingSeparator;
        };

    private:
        [[nodiscard]] bool TryReadToken(TokenInfo& token);
        void TryReadTrivia(std::vector<SyntaxTrivia>& builder, bool leading);

    private:
        [[nodiscard]] bool TryReadRawIdentifier(TokenInfo& token);
        [[nodiscard]] bool TryReadRawStringLiteral(TokenInfo& token);
        [[nodiscard]] bool TryReadDefaultStringLiteral(TokenInfo& token);
        [[nodiscard]] bool TryReadStringLiteral(TokenInfo& token);
        [[nodiscard]] bool TryReadCharacterLiteral(TokenInfo& token);
        [[nodiscard]] bool TryReadNumericLiteral(TokenInfo& token);
        [[nodiscard]] bool TryReadPunctuation(TokenInfo& token);
        [[nodiscard]] bool TryReadIdentifier(TokenInfo& token);
        [[nodiscard]] bool TryReadEndOfLine();
        [[nodiscard]] bool TryReadWhitespace();
        [[nodiscard]] SyntaxKind TryReadSingleLineComment();
        [[nodiscard]] SyntaxKind TryReadMultiLineComment();
        [[nodiscard]] SyntaxKind TryReadComment();

    private:
        [[nodiscard]] LiteralPrefixKind TryReadStringPrefix();
        [[nodiscard]] SingleInteger TryReadSingleInteger(std::string& builder, int base);

        [[nodiscard]] size_t TryReadDecNumber(uint64_t& result);
        [[nodiscard]] size_t TryReadHexNumber(uint64_t& result);
        [[nodiscard]] size_t TryReadDecNumber(uint64_t& result, size_t maxLength);
        [[nodiscard]] size_t TryReadHexNumber(uint64_t& result, size_t maxLength);

        [[nodiscard]] bool TryReadHexEscapeSequence(std::string& builder);
        [[nodiscard]] bool TryAppendUnicodeCodepoint(std::string& builder, char32_t c);
        [[nodiscard]] bool TryReadTrivialEscapeSequence(std::string& builder);
        [[nodiscard]] bool TryReadUnicodeEscapeSequence(std::string& builder);
        [[nodiscard]] bool TryReadEscapeSequence(std::string& builder);
        [[nodiscard]] bool TryReadStringOrCharacterLiteralCore(std::string& builder, char32_t specifier, size_t& consumed);
        [[nodiscard]] int TryReadNumericLiteralPrefixWithRadix(LiteralPrefixKind& prefix);
        void TryReadLiteralSuffix(std::string& builder);
    };
}
