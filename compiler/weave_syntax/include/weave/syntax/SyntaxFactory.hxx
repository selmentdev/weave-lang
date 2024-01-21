#pragma once
#include "weave/memory/TypedLinearAllocator.hxx"
#include "weave/stringpool/StringPool.hxx"
#include "weave/syntax/SyntaxToken.hxx"

namespace weave::syntax
{
    class SyntaxFactory final
    {
    private:
        memory::TypedLinearAllocator<SyntaxToken> TokenAllocator{16u << 10u};
        memory::TypedLinearAllocator<SyntaxTrivia> TriviaAllocator{};
        memory::TypedLinearAllocator<CharacterLiteralSyntaxToken> CharacterLiteralAllocator{};
        memory::TypedLinearAllocator<StringLiteralSyntaxToken> StringLiteralAllocator{};
        memory::TypedLinearAllocator<FloatLiteralSyntaxToken> FloatLiteralAllocator{};
        memory::TypedLinearAllocator<IntegerLiteralSyntaxToken> IntegerLiteralAllocator{};
        memory::TypedLinearAllocator<IdentifierSyntaxToken> IdentifierAllocator{};

        memory::LinearAllocator SyntaxNodeAllocator{128u << 10u};
        stringpool::StringPool Strings{};

    public:
        template <typename NodeT, typename... ArgsT>
            requires(std::is_base_of_v<SyntaxNode, NodeT>)
        NodeT* CreateNode(ArgsT&&... args) // NOLINT(cppcoreguidelines-missing-std-forward)
        {
            return this->SyntaxNodeAllocator.Emplace<NodeT>(std::forward<ArgsT>(args)...);
        }

        template <typename ContainerT>
        SyntaxList* CreateList(ContainerT const& builder)
        {
            if (builder.empty())
            {
                return nullptr;
            }

            std::span<SyntaxNode*> const elements = this->SyntaxNodeAllocator.EmplaceArray<SyntaxNode*>(builder.size());
            for (size_t i = 0; i < builder.size(); ++i)
            {
                elements[i] = static_cast<SyntaxNode*>(builder[i]);
            }

            return this->CreateNode<SyntaxList>(elements);
        }

    public:
        SyntaxListView<SyntaxTrivia> CreateTriviaList(std::span<SyntaxTrivia const> trivia);

        SyntaxToken* CreateToken(
            SyntaxKind kind,
            source::SourceSpan const& source);

        SyntaxToken* CreateToken(
            SyntaxKind kind,
            source::SourceSpan const& source,
            std::span<SyntaxTrivia const> leadingTrivia,
            std::span<SyntaxTrivia const> trailingTrivia);

        SyntaxToken* CreateMissingToken(
            SyntaxKind kind,
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia);

        SyntaxToken* CreateMissingToken(
            SyntaxKind kind,
            source::SourceSpan const& source)
        {
            return this->CreateMissingToken(
                kind,
                source,
                SyntaxListView<SyntaxTrivia>{},
                SyntaxListView<SyntaxTrivia>{});
        }

        SyntaxToken* CreateMissingToken(
            SyntaxKind kind,
            source::SourceSpan const& source,
            std::span<SyntaxTrivia const> leadingTrivia,
            std::span<SyntaxTrivia const> trailingTrivia);

        CharacterLiteralSyntaxToken* CreateCharacterLiteralToken(
            source::SourceSpan const& source,
            std::span<SyntaxTrivia const> leadingTrivia,
            std::span<SyntaxTrivia const> trailingTrivia,
            LiteralPrefixKind prefix,
            char32_t value);

        StringLiteralSyntaxToken* CreateStringLiteralToken(
            source::SourceSpan const& source,
            std::span<SyntaxTrivia const> leadingTrivia,
            std::span<SyntaxTrivia const> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value);

        FloatLiteralSyntaxToken* CreateFloatLiteralToken(
            source::SourceSpan const& source,
            std::span<SyntaxTrivia const> leadingTrivia,
            std::span<SyntaxTrivia const> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix);

        IntegerLiteralSyntaxToken* CreateIntegerLiteralToken(
            source::SourceSpan const& source,
            std::span<SyntaxTrivia const> leadingTrivia,
            std::span<SyntaxTrivia const> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix);

        IdentifierSyntaxToken* CreateIdentifierToken(
            source::SourceSpan const& source,
            std::span<SyntaxTrivia const> leadingTrivia,
            std::span<SyntaxTrivia const> trailingTrivia,
            std::string_view value);

    public:
        void DebugDump();
    };
}
