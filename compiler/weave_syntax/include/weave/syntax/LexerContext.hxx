#pragma once
#include "weave/memory/TypedLinearAllocator.hxx"
#include "weave/stringpool/StringPool.hxx"
#include "weave/syntax/Token.hxx"

namespace weave::syntax
{
    class Lexer;

    struct LexerContext
    {
        // Regular tokens, produced during parsing.
        memory::TypedLinearAllocator<Token> Tokens{16u << 10u};

        // Additional missing tokens, added for tree validation.
        memory::TypedLinearAllocator<Token> MissingTokens{};

        // Trivias allocated for tokens.
        memory::TypedLinearAllocator<Trivia> Trivias{};
        memory::TypedLinearAllocator<CharacterLiteralValue> CharacterLiterals{};
        memory::TypedLinearAllocator<StringLiteralValue> StringLiterals{};
        memory::TypedLinearAllocator<FloatLiteralValue> FloatLiterals{};
        memory::TypedLinearAllocator<IntegerLiteralValue> IntegerLiterals{};
        memory::TypedLinearAllocator<Identifier> Identifiers{};

        stringpool::StringPool Strings{};

        Token* Lex(Lexer& lexer);

        Token* Create(
            TokenKind kind,
            source::SourceSpan const& source);

        Token* Create(
            TokenKind kind,
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia);

        Token* Create(
            TokenKind kind,
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            void* value);

        Token* CreateMissing(
            TokenKind kind,
            source::SourceSpan const& source);

        Token* CreateCharacter(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            CharacterPrefixKind prefix,
            char32_t value);

        Token* CreateString(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            StringPrefixKind prefix,
            std::string_view value);

        Token* CreateFloat(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            NumberLiteralPrefixKind prefix,
            std::string_view value,
            FloatLiteralSuffixKind suffix);

        Token* CreateInteger(
            source::SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            NumberLiteralPrefixKind prefix,
            std::string_view value,
            IntegerLiteralSuffixKind suffix);

        Token* CreateIdentifier(
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
