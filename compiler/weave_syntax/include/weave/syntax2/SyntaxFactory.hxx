#pragma once
#include "weave/memory/TypedLinearAllocator.hxx"
#include "weave/stringpool/StringPool.hxx"
#include "weave/syntax2/SyntaxToken.hxx"

namespace weave::syntax2
{
    class SyntaxFactory final
    {
    private:
        memory::TypedLinearAllocator<SyntaxToken> TokenAllocator{16u << 10u};
        memory::TypedLinearAllocator<SyntaxTriviaRange> TriviaRangeAllocator{};
        memory::TypedLinearAllocator<SyntaxTrivia> TriviaAllocator{};
        memory::TypedLinearAllocator<CharacterLiteralSyntaxToken> CharacterLiteralAllocator{};
        memory::TypedLinearAllocator<StringLiteralSyntaxToken> StringLiteralAllocator{};
        memory::TypedLinearAllocator<FloatLiteralSyntaxToken> FloatLiteralAllocator{};
        memory::TypedLinearAllocator<IntegerLiteralSyntaxToken> IntegerLiteralAllocator{};
        memory::TypedLinearAllocator<IdentifierSyntaxToken> IdentifierAllocator{};

        memory::LinearAllocator SyntaxNodeAllocator{128u << 10u};
        stringpool::StringPool Strings{};

        static SyntaxTriviaRange const EmptyTriviaRange;

    public:
        template <typename NodeT, typename... ArgsT>
            requires(std::is_base_of_v<SyntaxNode, NodeT>)
        NodeT* CreateNode(ArgsT&&... args) // NOLINT(cppcoreguidelines-missing-std-forward)
        {
            return this->SyntaxNodeAllocator.Emplace<NodeT>(std::forward<ArgsT>(args)...);
        }

    public:
        SyntaxTriviaRange const* CreateTriviaRange(
            std::span<SyntaxTrivia const> leading,
            std::span<SyntaxTrivia const> trailing);

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
            source::SourceSpan const& source);

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
    };

    std::unique_ptr<SyntaxFactory> CreateSyntaxFactory();
}