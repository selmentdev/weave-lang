#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/bitwise/Flag.hxx"

#include <span>

namespace weave::syntax
{
    struct SyntaxTrivia final : SyntaxNode
    {
        source::SourceSpan Source;

        explicit constexpr SyntaxTrivia(
            SyntaxKind kind,
            source::SourceSpan const& source)
            : SyntaxNode{kind}
            , Source{source}
        {
        }
    };

    enum class SyntaxTokenFlags : uint32_t
    {
        None = 0u,
        Missing = 1u << 0u,
        HasErrors = 1u << 1u,
        ContainsSkippedText = 1u << 2u,
    };

    struct SyntaxToken : SyntaxNode
    {
        source::SourceSpan Source;

        bitwise::Flags<SyntaxTokenFlags> Flags{};

        SyntaxListView<SyntaxTrivia> LeadingTrivia{};
        SyntaxListView<SyntaxTrivia> TrailingTrivia{};

        constexpr SyntaxToken(
            SyntaxKind kind,
            source::SourceSpan source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia)
            : SyntaxNode{kind}
            , Source{source}
            , LeadingTrivia{leadingTrivia}
            , TrailingTrivia{trailingTrivia}
        {
        }

        constexpr SyntaxToken(
            SyntaxKind kind,
            source::SourceSpan source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxNode{kind}
            , Source{source}
            , Flags{flags}
            , LeadingTrivia{leadingTrivia}
            , TrailingTrivia{trailingTrivia}
        {
        }

        [[nodiscard]] constexpr bool IsMissing() const
        {
            return this->Flags.Any(SyntaxTokenFlags::Missing);
        }
    };

    enum class LiteralPrefixKind : uint8_t
    {
        Default, // default literal format

        Utf8, // u8"" strings
        Utf16, // u16"" strings
        Utf32, // u32"" strings

        Binary, // binary numbers / strings
        Octal, // 0o octal numbers
        Hexadecimal, // 0x hexadecimal numbers
    };

    struct IntegerLiteralSyntaxToken final : SyntaxToken
    {
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::IntegerLiteralToken;
        }

        static constexpr bool ClassOf(SyntaxNode const* token)
        {
            return ClassOf(token->Kind);
        }

        LiteralPrefixKind Prefix;
        std::string_view Value;
        std::string_view Suffix;

        constexpr IntegerLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix)
            : SyntaxToken{SyntaxKind::IntegerLiteralToken, source, leadingTrivia, trailingTrivia}
            , Prefix{prefix}
            , Value{value}
            , Suffix{suffix}
        {
        }

        constexpr IntegerLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::IntegerLiteralToken, source, leadingTrivia, trailingTrivia, flags}
            , Prefix{prefix}
            , Value{value}
            , Suffix{suffix}
        {
        }
    };

    struct FloatLiteralSyntaxToken final : SyntaxToken
    {
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::FloatLiteralToken;
        }

        static constexpr bool ClassOf(SyntaxNode const* token)
        {
            return ClassOf(token->Kind);
        }

        LiteralPrefixKind Prefix;
        std::string_view Value;
        std::string_view Suffix;

        constexpr FloatLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix)
            : SyntaxToken{SyntaxKind::FloatLiteralToken, source, leadingTrivia, trailingTrivia}
            , Prefix{prefix}
            , Value{value}
            , Suffix{suffix}
        {
        }

        constexpr FloatLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::FloatLiteralToken, source, leadingTrivia, trailingTrivia, flags}
            , Prefix{prefix}
            , Value{value}
            , Suffix{suffix}
        {
        }
    };

    struct StringLiteralSyntaxToken final : SyntaxToken
    {
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::StringLiteralToken;
        }

        static constexpr bool ClassOf(SyntaxNode const* token)
        {
            return ClassOf(token->Kind);
        }

        LiteralPrefixKind Prefix;
        std::string_view Value;

        constexpr StringLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value)
            : SyntaxToken{SyntaxKind::StringLiteralToken, source, leadingTrivia, trailingTrivia}
            , Prefix{prefix}
            , Value{value}
        {
        }

        constexpr StringLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::StringLiteralToken, source, leadingTrivia, trailingTrivia, flags}
            , Prefix{prefix}
            , Value{value}
        {
        }
    };

    struct CharacterLiteralSyntaxToken final : SyntaxToken
    {
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::CharacterLiteralToken;
        }

        static constexpr bool ClassOf(SyntaxNode const* token)
        {
            return ClassOf(token->Kind);
        }

        LiteralPrefixKind Prefix;
        char32_t Value;

        constexpr CharacterLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            char32_t value)
            : SyntaxToken{SyntaxKind::CharacterLiteralToken, source, leadingTrivia, trailingTrivia}
            , Prefix{prefix}
            , Value{value}
        {
        }

        constexpr CharacterLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            LiteralPrefixKind prefix,
            char32_t value,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::CharacterLiteralToken, source, leadingTrivia, trailingTrivia, flags}
            , Prefix{prefix}
            , Value{value}
        {
        }
    };

    struct IdentifierSyntaxToken final : SyntaxToken
    {
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::IdentifierToken;
        }

        static constexpr bool ClassOf(SyntaxNode const* token)
        {
            return ClassOf(token->Kind);
        }

        SyntaxKind ContextualKeyowrd{};
        std::string_view Identifier;

        constexpr IdentifierSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            SyntaxKind contextualKeyword,
            std::string_view identifier)
            : SyntaxToken{SyntaxKind::IdentifierToken, source, leadingTrivia, trailingTrivia}
            , ContextualKeyowrd{contextualKeyword}
            , Identifier{identifier}
        {
        }

        constexpr IdentifierSyntaxToken(
            source::SourceSpan const& source,
            SyntaxListView<SyntaxTrivia> leadingTrivia,
            SyntaxListView<SyntaxTrivia> trailingTrivia,
            SyntaxKind contextualKeyword,
            std::string_view identifier,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::IdentifierToken, source, leadingTrivia, trailingTrivia, flags}
            , ContextualKeyowrd{contextualKeyword}
            , Identifier{identifier}
        {
        }
    };

    using SyntaxTokenHandle = SyntaxToken const*;
}
