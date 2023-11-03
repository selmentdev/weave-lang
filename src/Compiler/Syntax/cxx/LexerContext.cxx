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
        return this->Create(
            kind,
            source,
            this->Trivias.CreateArray(leadingTrivia),
            this->Trivias.CreateArray(trailingTrivia),
            nullptr);
    }

    Token* LexerContext::Create(TokenKind kind, SourceSpan const& source, std::span<Trivia const> leadingTrivia, std::span<Trivia const> trailingTrivia, void* value)
    {
        WEAVE_ASSERT(leadingTrivia.size() < std::numeric_limits<uint16_t>::max());
        WEAVE_ASSERT(trailingTrivia.size() < std::numeric_limits<uint16_t>::max());

        Trivia const* trivia = nullptr;

        if (size_t const count = leadingTrivia.size() + trailingTrivia.size(); count != 0)
        {
            trivia = this->Trivias.CreateCopyCombined(leadingTrivia, trailingTrivia).data();
        }

        return this->Tokens.Create(
            kind,
            source,
            trivia,
            static_cast<uint16_t>(leadingTrivia.size()),
            static_cast<uint16_t>(trailingTrivia.size()),
            value);
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
        CharacterPrefixKind prefix,
        char32_t value)
    {
        Token* result = this->Create(
            TokenKind::CharacterLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->CharacterLiterals.Create(prefix, value)
            );
        return result;
    }

    Token* LexerContext::CreateString(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        StringPrefixKind prefix,
        std::string_view value)
    {
        Token* result = this->Create(
            TokenKind::StringLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->StringLiterals.Create(prefix, this->Strings.Get(value)));
        return result;
    }

    Token* LexerContext::CreateFloat(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        FloatLiteralSuffixKind suffix)
    {
        Token* result = this->Create(
            TokenKind::FloatLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->FloatLiterals.Create(prefix, suffix, this->Strings.Get(value)));
        return result;
    }

    Token* LexerContext::CreateInteger(
        SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        IntegerLiteralSuffixKind suffix)
    {
        Token* result = this->Create(
            TokenKind::IntegerLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->IntegerLiterals.Create(prefix, suffix, this->Strings.Get(value)));
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
            trailingTrivia,
            this->Identifiers.Create(this->Strings.Get(value)));
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
