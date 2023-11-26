#pragma once
#include "weave/memory/TypedLinearAllocator.hxx"
#include "weave/stringpool/StringPool.hxx"
#include "weave/lexer/Token.hxx"

namespace weave::lexer
{
    class Lexer;

    struct LexerContext
    {
        // Regular tokens, produced during parsing.
        memory::TypedLinearAllocator<Token> Tokens{16u << 10u};

        // Trivias allocated for tokens.
        memory::TypedLinearAllocator<TriviaRange> TriviaRanges{};
        memory::TypedLinearAllocator<Trivia> Trivias{};
        memory::TypedLinearAllocator<CharacterLiteralToken> CharacterLiterals{};
        memory::TypedLinearAllocator<StringLiteralToken> StringLiterals{};
        memory::TypedLinearAllocator<FloatLiteralToken> FloatLiterals{};
        memory::TypedLinearAllocator<IntegerLiteralToken> IntegerLiterals{};
        memory::TypedLinearAllocator<IdentifierToken> Identifiers{};

        stringpool::StringPool Strings{};

        Token* Lex(Lexer& lexer);

    private:
        TriviaRange _empty_trivia{};

        TriviaRange* CreateTriviaRange(
            std::span<Trivia const> leading,
            std::span<Trivia const> trailing);

    public:
        Token* Create(
            TokenKind kind,
            source::SourceSpan const& source);

        Token* Create(
            TokenKind kind,
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia);

        Token* CreateMissing(
            TokenKind kind,
            source::SourceSpan const& source);

        CharacterLiteralToken* CreateCharacter(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            CharacterPrefixKind prefix,
            char32_t value);

        StringLiteralToken* CreateString(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            StringPrefixKind prefix,
            std::string_view value);

        FloatLiteralToken* CreateFloat(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            NumberLiteralPrefixKind prefix,
            std::string_view value,
            FloatLiteralSuffixKind suffix);

        IntegerLiteralToken* CreateInteger(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            NumberLiteralPrefixKind prefix,
            std::string_view value,
            IntegerLiteralSuffixKind suffix);

        IdentifierToken* CreateIdentifier(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            std::string_view value);

        void QueryMemoryUsage(
            size_t& allocated,
            size_t& reserved) const;

        void DumpMemoryUsage() const;
    };
}
