#pragma once
#include "Weave.Core/Memory/Allocator.hxx"
#include "Weave.Core/StringPool.hxx"
#include "Weave.Syntax/Token.hxx"

namespace Weave::Syntax
{
    struct LexerContext
    {
        // Regular tokens, produced during parsing.
        Memory::TypedLinearAllocator<Token> Tokens{16u << 10u};

        // Additional missing tokens, added for tree validation.
        Memory::TypedLinearAllocator<Token> MissingTokens{};

        // Trivias allocated for tokens.
        Memory::TypedLinearAllocator<Trivia> Trivias{};
        Memory::TypedLinearAllocator<CharacterLiteralValue> CharacterLiterals{};
        Memory::TypedLinearAllocator<StringLiteralValue> StringLiterals{};
        Memory::TypedLinearAllocator<FloatLiteralValue> FloatLiterals{};
        Memory::TypedLinearAllocator<IntegerLiteralValue> IntegerLiterals{};
        Memory::TypedLinearAllocator<Identifier> Identifiers{};

        StringPool Strings{};

        Token* Create(
            TokenKind kind,
            SourceSpan const& source);

        Token* Create(
            TokenKind kind,
            SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia);

        Token* CreateMissing(
            TokenKind kind,
            SourceSpan const& source);

        Token* CreateCharacter(
            SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            CharacterPrefixKind prefix,
            char32_t value);

        Token* CreateString(
            SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            StringPrefixKind prefix,
            std::string_view value);

        Token* CreateFloat(
            SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            NumberLiteralPrefixKind prefix,
            std::string_view value,
            FloatLiteralSuffixKind suffix);

        Token* CreateInteger(
            SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            NumberLiteralPrefixKind prefix,
            std::string_view value,
            IntegerLiteralSuffixKind suffix);

        Token* CreateIdentifier(
            SourceSpan const& source,
            std::span<Trivia const> leadingTrivia,
            std::span<Trivia const> trailingTrivia,
            std::string_view value);

        void QueryMemoryUsage(
            size_t& allocated,
            size_t& reserved) const;
    };
}
