#pragma once
#include <cstdint>
#include <string_view>
#include <optional>

namespace Weave::Lexer
{
    enum class TokenKind : uint32_t
    {
#define WEAVE_TOKEN(name, spelling) name,
#include "Compiler.Lexer/TokenKind.inc"
    };

    class TokenKindTraits final
    {
    public:
        [[nodiscard]] static std::string_view GetName(TokenKind value);

        [[nodiscard]] static std::string_view GetSpelling(TokenKind value);

        [[nodiscard]] static bool IsSymbol(TokenKind value);

        [[nodiscard]] static bool IsOpeningGroupSymbol(TokenKind value);

        [[nodiscard]] static bool IsClosingGroupSymbol(TokenKind value);

        [[nodiscard]] static bool IsLiteral(TokenKind value);

        [[nodiscard]] static bool IsKeyword(TokenKind value);

        [[nodiscard]] static bool IsTypeKeyword(TokenKind value);

        [[nodiscard]] static TokenKind GetOpeningGroupSymbol(TokenKind value);

        [[nodiscard]] static TokenKind GetClosingGroupSymbol(TokenKind value);

        [[nodiscard]] static std::optional<TokenKind> TryMapIdentifierToKeyword(std::string_view value);
    };
}
