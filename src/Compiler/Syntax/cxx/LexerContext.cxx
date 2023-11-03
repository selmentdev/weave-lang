#include "Weave.Syntax/LexerContext.hxx"

namespace Weave::Syntax
{
    Token* LexerContext::Create(
        TokenKind kind,
        SourceSpan const& source)
    {
        return this->Tokens.Create(
            kind,
            source);
    }

    Token* LexerContext::Create(
        TokenKind kind,
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia)
    {
        return this->Tokens.Create(
            kind,
            source,
            this->Trivias.CreateArray(leadingTrivia),
            this->Trivias.CreateArray(trailingTrivia));
    }

    Token* LexerContext::CreateMissing(
        TokenKind kind,
        SourceSpan const& source)
    {
        return this->MissingTokens.Create(
            kind,
            source);
    }

    Token* LexerContext::CreateCharacter(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        CharacterPrefix prefix,
        char32_t value)
    {
        Token* result = this->Create(
            TokenKind::CharacterLiteral,
            source,
            leadingTrivia,
            trailingTrivia);
        result->Value = this->CharacterLiterals.Create(prefix, value);
        return result;
    }

    Token* LexerContext::CreateString(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        StringPrefix prefix,
        std::string_view value)
    {
        Token* result = this->Create(
            TokenKind::StringLiteral,
            source,
            leadingTrivia,
            trailingTrivia);
        result->Value = this->StringLiterals.Create(prefix, this->Strings.Get(value));
        return result;
    }

    Token* LexerContext::CreateFloat(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefix prefix,
        std::string_view value,
        FloatLiteralSuffix suffix)
    {
        Token* result = this->Create(
            TokenKind::FloatLiteral,
            source,
            leadingTrivia,
            trailingTrivia);
        result->Value = this->FloatLiterals.Create(prefix, suffix, this->Strings.Get(value));
        return result;
    }

    Token* LexerContext::CreateInteger(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefix prefix,
        std::string_view value,
        IntegerLiteralSuffix suffix)
    {
        Token* result = this->Create(
            TokenKind::IntegerLiteral,
            source,
            leadingTrivia,
            trailingTrivia);
        result->Value = this->IntegerLiterals.Create(prefix, suffix, this->Strings.Get(value));
        return result;
    }

    Token* LexerContext::CreateIdentifier(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        std::string_view value)
    {
        Token* result = this->Create(
            TokenKind::Identifier,
            source,
            leadingTrivia,
            trailingTrivia);
        result->Value = this->Identifiers.Create(this->Strings.Get(value));
        return result;
    }

    void LexerContext::QueryMemoryUsage(size_t& allocated, size_t& reserved) const
    {
        allocated = 0;
        reserved = 0;

        this->Tokens.QueryMemoryUsage(allocated, reserved);
        this->MissingTokens.QueryMemoryUsage(allocated, reserved);
        this->Trivias.QueryMemoryUsage(allocated, reserved);
        this->CharacterLiterals.QueryMemoryUsage(allocated, reserved);
        this->StringLiterals.QueryMemoryUsage(allocated, reserved);
        this->FloatLiterals.QueryMemoryUsage(allocated, reserved);
        this->IntegerLiterals.QueryMemoryUsage(allocated, reserved);
        this->Identifiers.QueryMemoryUsage(allocated, reserved);

        this->Strings.QueryMemoryUsage(allocated, reserved);
    }
}
