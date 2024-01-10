#pragma once
#include <string_view>
#include <optional>
#include <cstdint>
#include <utility>

namespace weave::syntax2
{
    enum class SyntaxKind : uint16_t
    {
#define WEAVE_SYNTAX(name, value, spelling) name = value,
#include "weave/syntax2/SyntaxKind.inl"
    };

    class SyntaxKindTraits
    {
    public:
#define WEAVE_SYNTAX_BEGIN_GROUP(name, value) \
    static constexpr uint16_t name##_First = value;
#define WEAVE_SYNTAX_END_GROUP(name, value) \
    static constexpr uint16_t name##_Last = value; \
    static constexpr size_t name##_Count = name##_Last - name##_First + 1;
#include "weave/syntax2/SyntaxKind.inl"
    public:
        [[nodiscard]] static constexpr bool IsTrivia(SyntaxKind value)
        {
            auto const index = std::to_underlying(value);
            return (Trivia_First <= index) and (index <= Trivia_Last);
        }

        [[nodiscard]] static constexpr bool IsToken(SyntaxKind value)
        {
            auto const index = std::to_underlying(value);
            return (Token_First <= index) and (index <= Token_Last);
        }

        [[nodiscard]] static constexpr bool IsBaseKeyword(SyntaxKind value)
        {
            auto const index = std::to_underlying(value);
            return (Keyword_First <= index) and (index <= Keyword_Last);
        }

        [[nodiscard]] static constexpr bool IsTypeKeyword(SyntaxKind value)
        {
            auto const index = std::to_underlying(value);
            return (TypeKeyword_First <= index) and (index <= TypeKeyword_Last);
        }

        [[nodiscard]] static constexpr bool IsKeyword(SyntaxKind value)
        {
            return IsBaseKeyword(value) or IsTypeKeyword(value);
        }

        [[nodiscard]] static constexpr bool IsSyntaxNode(SyntaxKind kind)
        {
            auto const index = std::to_underlying(kind);
            return (Node_First <= index) and (index <= Node_Last);
        }

        [[nodiscard]] static constexpr bool IsSyntaxToken(SyntaxKind kind)
        {
            return IsToken(kind) or IsKeyword(kind);
        }

    public:
        [[nodiscard]] static std::string_view GetName(SyntaxKind value);

        [[nodiscard]] static std::string_view GetCategoryName(SyntaxKind value);

        [[nodiscard]] static std::string_view GetSpelling(SyntaxKind value);

        [[nodiscard]] static std::optional<SyntaxKind> TryMapIdentifierToKeyword(std::string_view value);

        [[nodiscard]] static constexpr bool IsDocumentationTrivia(SyntaxKind value)
        {
            return (value == SyntaxKind::MultiLineDocumentationTrivia) or (value == SyntaxKind::SingleLineDocumentationTrivia);
        }

        // [[nodiscard]] static bool IsDeclaration(SyntaxKind value);

        // [[nodiscard]] static bool IsExpression(SyntaxKind value);

        // [[nodiscard]] static bool IsStatement(SyntaxKind value);

        // [[nodiscard]] static bool IsSymbol(SyntaxKind value);

        // [[nodiscard]] static bool IsOpeningGroupSymbol(SyntaxKind value);

        // [[nodiscard]] static bool IsClosingGroupSymbol(SyntaxKind value);

        // [[nodiscard]] static bool IsLiteral(SyntaxKind value);

        // [[nodiscard]] static SyntaxKind GetOpeningGroupSymbol(SyntaxKind value);

        // [[nodiscard]] static SyntaxKind GetClosingGroupSymbol(SyntaxKind value);
    };
}
