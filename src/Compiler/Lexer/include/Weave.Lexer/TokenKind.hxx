#pragma once
#include <cstdint>
#include <string_view>
#include <optional>

namespace Weave::Lexer
{
    enum class TokenKind : uint32_t
    {
#define WEAVE_TOKEN(name, spelling) name,
#include "Weave.Lexer/TokenKind.inc"
    };

    class TokenKindTraits final
    {
    public:
        [[nodiscard]] static std::string_view GetName(TokenKind value);

        [[nodiscard]] static std::string_view GetSpelling(TokenKind value);

        [[nodiscard]] static bool IsPunctuation(TokenKind value);

        [[nodiscard]] static bool IsOpeningGroupPunctuation(TokenKind value);

        [[nodiscard]] static bool IsClosingGroupPunctuation(TokenKind value);

        [[nodiscard]] static bool IsLiteral(TokenKind value);

        [[nodiscard]] static bool IsKeyword(TokenKind value);

        [[nodiscard]] static bool IsTypeKeyword(TokenKind value);

        [[nodiscard]] static TokenKind GetOpeningGroupPunctuation(TokenKind value);

        [[nodiscard]] static TokenKind GetClosingGroupPunctuation(TokenKind value);

        [[nodiscard]] static std::optional<TokenKind> TryMapIdentifierToKeyword(std::string_view value);
    };
}
