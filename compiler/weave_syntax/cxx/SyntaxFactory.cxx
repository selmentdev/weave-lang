#include "weave/syntax/SyntaxFactory.hxx"
#include "weave/bugcheck/Assert.hxx"

namespace weave::syntax
{
    SyntaxTriviaRange const SyntaxFactory::EmptyTriviaRange{};

    SyntaxTriviaRange const* SyntaxFactory::CreateTriviaRange(
        std::span<SyntaxTrivia const> leading,
        std::span<SyntaxTrivia const> trailing)
    {
        if (leading.empty() and trailing.empty())
        {
            return &EmptyTriviaRange;
        }

        SyntaxTriviaRange const result{
            .Leading = this->TriviaAllocator.EmplaceArray(leading),
            .Trailing = this->TriviaAllocator.EmplaceArray(trailing),
        };

        return this->TriviaRangeAllocator.Emplace(result);
    }

    SyntaxToken* SyntaxFactory::CreateToken(
        SyntaxKind kind,
        source::SourceSpan const& source)
    {
        WEAVE_ASSERT(kind != SyntaxKind::IdentifierToken);
        WEAVE_ASSERT(kind != SyntaxKind::IntegerLiteralToken);
        WEAVE_ASSERT(kind != SyntaxKind::FloatLiteralToken);
        WEAVE_ASSERT(kind != SyntaxKind::StringLiteralToken);
        WEAVE_ASSERT(kind != SyntaxKind::CharacterLiteralToken);

        return this->TokenAllocator.Emplace(
            kind,
            source,
            &EmptyTriviaRange);
    }

    SyntaxToken* SyntaxFactory::CreateToken(
        SyntaxKind kind,
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia)
    {
        WEAVE_ASSERT(kind != SyntaxKind::IdentifierToken);
        WEAVE_ASSERT(kind != SyntaxKind::IntegerLiteralToken);
        WEAVE_ASSERT(kind != SyntaxKind::FloatLiteralToken);
        WEAVE_ASSERT(kind != SyntaxKind::StringLiteralToken);
        WEAVE_ASSERT(kind != SyntaxKind::CharacterLiteralToken);

        return this->TokenAllocator.Emplace(
            kind,
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia));
    }

    SyntaxToken* SyntaxFactory::CreateMissingToken(
        SyntaxKind kind,
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia)
    {
        SyntaxTriviaRange const* trivia = this->CreateTriviaRange(leadingTrivia, trailingTrivia);
        if (kind == SyntaxKind::CharacterLiteralToken)
        {
            return this->CharacterLiteralAllocator.Emplace(
                source,
                trivia,
                LiteralPrefixKind::Default,
                char32_t{},
                SyntaxTokenFlags::Missing);
        }

        if (kind == SyntaxKind::FloatLiteralToken)
        {
            return this->FloatLiteralAllocator.Emplace(
                source,
                trivia,
                LiteralPrefixKind::Default,
                std::string_view{},
                std::string_view{},
                SyntaxTokenFlags::Missing);
        }

        if (kind == SyntaxKind::IntegerLiteralToken)
        {
            return this->IntegerLiteralAllocator.Emplace(
                source,
                trivia,
                LiteralPrefixKind::Default,
                std::string_view{},
                std::string_view{},
                SyntaxTokenFlags::Missing);
        }

        if (kind == SyntaxKind::IdentifierToken)
        {
            return this->IdentifierAllocator.Emplace(
                source,
                trivia,
                std::string_view{},
                SyntaxTokenFlags::Missing);
        }

        return this->TokenAllocator.Emplace(
            kind,
            source,
            trivia,
            SyntaxTokenFlags::Missing);
    }

    CharacterLiteralSyntaxToken* SyntaxFactory::CreateCharacterLiteralToken(
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia,
        LiteralPrefixKind prefix,
        char32_t value)
    {
        return this->CharacterLiteralAllocator.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            value);
    }

    StringLiteralSyntaxToken* SyntaxFactory::CreateStringLiteralToken(
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia,
        LiteralPrefixKind prefix,
        std::string_view value)
    {
        return this->StringLiteralAllocator.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            this->Strings.Get(value));
    }

    FloatLiteralSyntaxToken* SyntaxFactory::CreateFloatLiteralToken(
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia,
        LiteralPrefixKind prefix,
        std::string_view value,
        std::string_view suffix)
    {
        return this->FloatLiteralAllocator.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            this->Strings.Get(value),
            this->Strings.Get(suffix));
    }

    IntegerLiteralSyntaxToken* SyntaxFactory::CreateIntegerLiteralToken(
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia,
        LiteralPrefixKind prefix,
        std::string_view value,
        std::string_view suffix)
    {
        return this->IntegerLiteralAllocator.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            this->Strings.Get(value),
            this->Strings.Get(suffix));
    }

    IdentifierSyntaxToken* SyntaxFactory::CreateIdentifierToken(
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia,
        std::string_view value)
    {
        return this->IdentifierAllocator.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            this->Strings.Get(value));
    }
}
