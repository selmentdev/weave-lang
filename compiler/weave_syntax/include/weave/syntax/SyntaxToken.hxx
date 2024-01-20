#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/bitwise/Flag.hxx"

#include <span>

namespace weave::syntax
{
    struct SyntaxTrivia final
    {
        SyntaxKind Kind;
        source::SourceSpan Source;
    };

    struct SyntaxTriviaRange
    {
        std::span<SyntaxTrivia const> Leading{};
        std::span<SyntaxTrivia const> Trailing{};
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
        SyntaxTriviaRange const* Trivia;

        constexpr SyntaxToken(SyntaxKind kind, source::SourceSpan source, SyntaxTriviaRange const* trivia)
            : SyntaxNode{kind}
            , Source{source}
            , Trivia{trivia}
        {
        }

        constexpr SyntaxToken(SyntaxKind kind, source::SourceSpan source, SyntaxTriviaRange const* trivia, bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxNode{kind}
            , Source{source}
            , Flags{flags}
            , Trivia{trivia}
        {
        }

        [[nodiscard]] constexpr std::span<SyntaxTrivia const> GetLeadingTrivia() const
        {
            if (this->Trivia != nullptr)
            {
                return this->Trivia->Leading;
            }

            return {};
        }

        [[nodiscard]] constexpr std::span<SyntaxTrivia const> GetTrailingTrivia() const
        {
            if (this->Trivia != nullptr)
            {
                return this->Trivia->Trailing;
            }

            return {};
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
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix)
            : SyntaxToken{SyntaxKind::IntegerLiteralToken, source, trivia}
            , Prefix{prefix}
            , Value{value}
            , Suffix{suffix}
        {
        }

        constexpr IntegerLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::IntegerLiteralToken, source, trivia, flags}
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
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix)
            : SyntaxToken{SyntaxKind::FloatLiteralToken, source, trivia}
            , Prefix{prefix}
            , Value{value}
            , Suffix{suffix}
        {
        }

        constexpr FloatLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            std::string_view suffix,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::FloatLiteralToken, source, trivia, flags}
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
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            std::string_view value)
            : SyntaxToken{SyntaxKind::StringLiteralToken, source, trivia}
            , Prefix{prefix}
            , Value{value}
        {
        }

        constexpr StringLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            std::string_view value,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::StringLiteralToken, source, trivia, flags}
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
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            char32_t value)
            : SyntaxToken{SyntaxKind::CharacterLiteralToken, source, trivia}
            , Prefix{prefix}
            , Value{value}
        {
        }

        constexpr CharacterLiteralSyntaxToken(
            source::SourceSpan const& source,
            SyntaxTriviaRange const* trivia,
            LiteralPrefixKind prefix,
            char32_t value,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::CharacterLiteralToken, source, trivia, flags}
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

        std::string_view Identifier;

        constexpr IdentifierSyntaxToken(
            source::SourceSpan const& source,
            SyntaxTriviaRange const* trivia,
            std::string_view identifier)
            : SyntaxToken{SyntaxKind::IdentifierToken, source, trivia}
            , Identifier{identifier}
        {
        }

        constexpr IdentifierSyntaxToken(
            source::SourceSpan const& source,
            SyntaxTriviaRange const* trivia,
            std::string_view identifier,
            bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxToken{SyntaxKind::IdentifierToken, source, trivia, flags}
            , Identifier{identifier}
        {
        }
    };

    using SyntaxTokenHandle = SyntaxToken const*;
}
