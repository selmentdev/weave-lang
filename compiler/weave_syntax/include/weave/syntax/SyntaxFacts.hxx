#pragma once
#include "weave/syntax/SyntaxKind.hxx"

namespace weave::syntax
{
    enum class Precedence : int
    {
        Expression = 0,
        Assignment = Expression,
        Lambda = Assignment,
        Conditional,
        Coalescing,
        ConditionalOr,
        ConditionalAnd,
        LogicalOr,
        LogicalXor,
        LogicalAnd,
        Equality,
        Relational,
        Shift,
        Additive,
        Multiplicative,
        Switch,
        Range,
        Unary,
        Cast,
        Dereference,
        AddressOf,
        Primary,
    };

    class SyntaxFacts final
    {
    public:
        static bool IsFunctionParameterModifier(SyntaxKind kind);
        static bool IsFunctionArgumentDirectionKind(SyntaxKind kind);
        static bool IsMemberModifier(SyntaxKind kind);

        static SyntaxKind GetPrefixUnaryExpression(SyntaxKind token);
        static SyntaxKind GetPostfixUnaryExpression(SyntaxKind token);
        static SyntaxKind GetBinaryExpression(SyntaxKind token);
        static SyntaxKind GetAssignmentExpression(SyntaxKind token);

        static bool IsRightAssociative(SyntaxKind operation);
        static Precedence GetPrecedence(SyntaxKind operation);

        static bool IsName(SyntaxKind kind);

        static bool IsInvalidSubexpression(SyntaxKind kind);
        static bool IsValidExpression(SyntaxKind kind);
        static bool IsValidStatement(SyntaxKind kind);

        static bool IsStartOfStatement(SyntaxKind kind);
        static bool IsStartOfDeclaration(SyntaxKind kind);
        static bool IsStartOfExpression(SyntaxKind kind);
    };
}
