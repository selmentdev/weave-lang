#pragma once
#include <string_view>
#include <optional>
#include <cstdint>
#include <utility>

namespace weave::syntax
{
    enum class SyntaxKind : uint16_t
    {
#define WEAVE_SYNTAX(name, value) name,
#include "weave/syntax/SyntaxKind.inl"
    };

    std::string_view GetName(SyntaxKind kind);

    std::string_view GetSpelling(SyntaxKind kind);

    bool IsToken(SyntaxKind kind);

    bool IsTrivia(SyntaxKind kind);

    bool IsDocumentationTrivia(SyntaxKind kind);

    bool IsPunctuation(SyntaxKind kind);

    bool IsKeyword(SyntaxKind kind);

    bool IsContextualKeyword(SyntaxKind kind);

    bool IsNode(SyntaxKind kind);

    bool IsExpression(SyntaxKind kind);

    std::optional<SyntaxKind> TryMapIdentifierToKeyword(std::string_view value);
}
