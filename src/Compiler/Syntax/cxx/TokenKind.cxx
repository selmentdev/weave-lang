#include "Weave.Syntax/TokenKind.hxx"
#include "Weave.Core/Assert.hxx"

#include <utility>

namespace Weave::Syntax
{
    std::string_view TokenKindTraits::GetName(TokenKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_TOKEN(name, spelling) #name,
#include "Weave.Syntax/TokenKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    std::string_view TokenKindTraits::GetSpelling(TokenKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_TOKEN(name, spelling) spelling,
#include "Weave.Syntax/TokenKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    bool TokenKindTraits::IsSymbol(TokenKind value)
    {
        static constexpr bool lookup[]{
#define WEAVE_TOKEN(name, spelling) false,
#define WEAVE_TOKEN_PUNCTUATION(name, spelling) true,
#include "Weave.Syntax/TokenKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    bool TokenKindTraits::IsOpeningGroupSymbol(TokenKind value)
    {
        switch (value) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_TOKEN_PUNCTUATION_GROUP_OPENING(name, spelling, matching) case TokenKind::name:
#include "Weave.Syntax/TokenKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool TokenKindTraits::IsClosingGroupSymbol(TokenKind value)
    {
        switch (value) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_TOKEN_PUNCTUATION_GROUP_CLOSING(name, spelling, matching) case TokenKind::name:
#include "Weave.Syntax/TokenKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool TokenKindTraits::IsLiteral(TokenKind value)
    {
        static constexpr bool lookup[]{
#define WEAVE_TOKEN(name, spelling) false,
#define WEAVE_TOKEN_LITERAL(name, spelling) true,
#include "Weave.Syntax/TokenKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    bool TokenKindTraits::IsKeyword(TokenKind value)
    {
        static constexpr bool lookup[]{
#define WEAVE_TOKEN(name, spelling) false,
#define WEAVE_TOKEN_KEYWORD(name, spelling) true,
#include "Weave.Syntax/TokenKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    bool TokenKindTraits::IsTypeKeyword(TokenKind value)
    {
        static constexpr bool lookup[]{
#define WEAVE_TOKEN(name, spelling) false,
#define WEAVE_TOKEN_TYPE_KEYWORD(name, spelling) true,
#include "Weave.Syntax/TokenKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    TokenKind TokenKindTraits::GetOpeningGroupSymbol(TokenKind value)
    {
        switch (value) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_TOKEN_PUNCTUATION_GROUP_OPENING(name, spelling, matching) \
    case TokenKind::name: \
        return TokenKind::matching;
#include "Weave.Syntax/TokenKind.inl"

        default:
            break;
        }

        return TokenKind::Error;
    }

    TokenKind TokenKindTraits::GetClosingGroupSymbol(TokenKind value)
    {
        switch (value)  // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_TOKEN_PUNCTUATION_GROUP_CLOSING(name, spelling, matching) \
    case TokenKind::name: \
        return TokenKind::matching;
#include "Weave.Syntax/TokenKind.inl"

        default:
            break;
        }

        return TokenKind::Error;
    }

    std::optional<TokenKind> TokenKindTraits::TryMapIdentifierToKeyword(std::string_view value)
    {
        struct Entry
        {
            std::string_view Spelling;
            TokenKind Token;
        };

        static constexpr Entry lookup[]{
#define WEAVE_TOKEN_KEYWORD(name, spelling) {spelling, TokenKind::name},
#include "Weave.Syntax/TokenKind.inl"
        };

        for (Entry const& entry : lookup)
        {
            if (entry.Spelling == value)
            {
                return entry.Token;
            }
        }

        return {};
    }
}
