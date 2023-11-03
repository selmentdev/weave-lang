#pragma once
#include <cstdint>
#include <string_view>

namespace Weave::Syntax
{
    enum class SyntaxKind
    {
#define WEAVE_SYNTAX_NODE(name, spelling) name,
#include "Weave.Syntax/SyntaxKind.inl"
    };

    class SyntaxKindTraits
    {
    public:
        [[nodiscard]] static std::string_view GetName(SyntaxKind value);

        [[nodiscard]] static std::string_view GetSpelling(SyntaxKind value);

        [[nodiscard]] static bool IsAbstract(SyntaxKind value);

        [[nodiscard]] static bool IsDeclaration(SyntaxKind value);

        [[nodiscard]] static bool IsExpression(SyntaxKind value);

        [[nodiscard]] static bool IsStatement(SyntaxKind value);
    };
}
