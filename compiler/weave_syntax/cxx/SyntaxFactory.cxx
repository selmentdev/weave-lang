#include "weave/syntax/SyntaxFactory.hxx"
#include "weave/bugcheck/Assert.hxx"

namespace weave::syntax
{
    SyntaxListView<SyntaxTrivia> SyntaxFactory::CreateTriviaList(std::span<const SyntaxTrivia> trivia)
    {
        if (not trivia.empty())
        {
            // Allocate builder of trivia nodes
            std::span<SyntaxTrivia> const nodes = this->TriviaAllocator.EmplaceArray(trivia);

            std::span<SyntaxNode*> const builder = this->SyntaxNodeAllocator.EmplaceArray<SyntaxNode*>(nodes.size());

            for (size_t i = 0; i < nodes.size(); ++i)
            {
                builder[i] = static_cast<SyntaxNode*>(&nodes[i]);
            }

            // Allocate builder of pointers to the nodes
            return SyntaxListView<SyntaxTrivia>{this->CreateList(builder)};
        }

        return SyntaxListView<SyntaxTrivia>{nullptr};
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
            SyntaxListView<SyntaxTrivia>{},
            SyntaxListView<SyntaxTrivia>{});
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
            this->CreateTriviaList(leadingTrivia),
            this->CreateTriviaList(trailingTrivia));
    }

    SyntaxToken* SyntaxFactory::CreateMissingToken(
        SyntaxKind kind,
        source::SourceSpan const& source,
        SyntaxListView<SyntaxTrivia> leadingTrivia,
        SyntaxListView<SyntaxTrivia> trailingTrivia)
    {
        if (kind == SyntaxKind::CharacterLiteralToken)
        {
            return this->CharacterLiteralAllocator.Emplace(
                source,
                leadingTrivia,
                trailingTrivia,
                LiteralPrefixKind::Default,
                char32_t{},
                SyntaxTokenFlags::Missing);
        }

        if (kind == SyntaxKind::FloatLiteralToken)
        {
            return this->FloatLiteralAllocator.Emplace(
                source,
                leadingTrivia,
                trailingTrivia,
                LiteralPrefixKind::Default,
                std::string_view{},
                std::string_view{},
                SyntaxTokenFlags::Missing);
        }

        if (kind == SyntaxKind::IntegerLiteralToken)
        {
            return this->IntegerLiteralAllocator.Emplace(
                source,
                leadingTrivia,
                trailingTrivia,
                LiteralPrefixKind::Default,
                std::string_view{},
                std::string_view{},
                SyntaxTokenFlags::Missing);
        }

        if (kind == SyntaxKind::IdentifierToken)
        {
            return this->IdentifierAllocator.Emplace(
                source,
                leadingTrivia,
                trailingTrivia,
                SyntaxKind::None,
                std::string_view{},
                SyntaxTokenFlags::Missing);
        }

        return this->TokenAllocator.Emplace(
            kind,
            source,
            leadingTrivia,
            trailingTrivia,
            SyntaxTokenFlags::Missing);
    }

    SyntaxToken* SyntaxFactory::CreateMissingContextualKeyword(
        SyntaxKind kind,
        source::SourceSpan const& source)
    {
        return this->IdentifierAllocator.Emplace(
            source,
            SyntaxListView<SyntaxTrivia>{},
            SyntaxListView<SyntaxTrivia> {},
            kind,
            std::string_view{},
            SyntaxTokenFlags::Missing);
    }

    SyntaxToken* SyntaxFactory::CreateMissingToken(
        SyntaxKind kind,
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia)
    {
        return this->CreateMissingToken(
            kind,
            source,
            this->CreateTriviaList(leadingTrivia),
            this->CreateTriviaList(trailingTrivia));
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
            this->CreateTriviaList(leadingTrivia),
            this->CreateTriviaList(trailingTrivia),
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
            this->CreateTriviaList(leadingTrivia),
            this->CreateTriviaList(trailingTrivia),
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
            this->CreateTriviaList(leadingTrivia),
            this->CreateTriviaList(trailingTrivia),
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
            this->CreateTriviaList(leadingTrivia),
            this->CreateTriviaList(trailingTrivia),
            prefix,
            this->Strings.Get(value),
            this->Strings.Get(suffix));
    }

    IdentifierSyntaxToken* SyntaxFactory::CreateIdentifierToken(
        source::SourceSpan const& source,
        std::span<SyntaxTrivia const> leadingTrivia,
        std::span<SyntaxTrivia const> trailingTrivia,
        SyntaxKind contextualKeyword,
        std::string_view value)
    {
        return this->IdentifierAllocator.Emplace(
            source,
            this->CreateTriviaList(leadingTrivia),
            this->CreateTriviaList(trailingTrivia),
            contextualKeyword,
            this->Strings.Get(value));
    }

    void SyntaxFactory::DebugDump()
    {
        size_t totalAllocated{};
        size_t totalReserved{};

        auto dump = [&](memory::LinearAllocator const& allocator, std::string_view name)
        {
            size_t allocated{};
            size_t reserved{};
            allocator.QueryMemoryUsage(allocated, reserved);

            fmt::println("{:>30}: (allocated: {:>9}, reserved: {:>9})", name, allocated, reserved);

            totalAllocated += allocated;
            totalReserved += reserved;
        };

        dump(this->TokenAllocator, "TokenAllocator");
        dump(this->TriviaAllocator, "TriviaAllocator");
        dump(this->CharacterLiteralAllocator, "CharacterLiteralAllocator");
        dump(this->StringLiteralAllocator, "StringLiteralAllocator");
        dump(this->FloatLiteralAllocator, "FloatLiteralAllocator");
        dump(this->IntegerLiteralAllocator, "IntegerLiteralAllocator");
        dump(this->IdentifierAllocator, "IdentifierAllocator");
        dump(this->SyntaxNodeAllocator, "SyntaxNodeAllocator");

        fmt::println("Total: (allocated: {}, reserved: {})", totalAllocated, totalReserved);
    }
}
