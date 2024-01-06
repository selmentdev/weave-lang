#pragma once
#include "weave/syntax/SyntaxNode.hxx"

namespace weave::syntax
{
    class Expression : public SyntaxNode
    {
    public:
        explicit constexpr Expression(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };
}

/// Name Expressions
///
/// ```
/// name
///     : qualified_name
///     | simple_name
///     ;
///
/// qualified_name
///     : name '.' name
///     ;
/// simple_name
///     : identifier_name
///     | generic_name
///     ;
///
/// generic_name
///     : identifier_name '![' type_argument_list ']'
///     ;
/// ```

namespace weave::syntax
{
    class NameExpression : public Expression
    {
    public:
        explicit constexpr NameExpression(SyntaxKind kind)
            : Expression{kind}
        {
        }
    };

    class QualifiedNameExpression final : public NameExpression
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::QualifiedNameExpression;
        }

        static constexpr bool ClassOf(NameExpression const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr QualifiedNameExpression(NameExpression* left, tokenizer::Token* dot, SimpleNameExpression* right)
            : NameExpression{SyntaxKind::QualifiedNameExpression}
            , Left{left}
            , DotToken{dot}
            , Right{right}
        {
        }

    public:
        NameExpression* Left{};
        tokenizer::Token* DotToken{};
        SimpleNameExpression* Right{};
    };

    class SimpleNameExpression : public NameExpression
    {
    public:
        explicit constexpr SimpleNameExpression(SyntaxKind kind, tokenizer::IdentifierToken* identifier)
            : NameExpression{kind}
            , IdentifierToken{identifier}
        {
        }

        tokenizer::IdentifierToken* IdentifierToken{};
    };

    class IdentifierNameExpression final : public SimpleNameExpression
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::SimpleNameExpression;
        }

        static constexpr bool ClassOf(NameExpression const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr IdentifierNameExpression(tokenizer::IdentifierToken* identifier)
            : SimpleNameExpression{SyntaxKind::IdentifierNameExpression, identifier}
        {
        }
    };

    class GenericNameExpression final : public SimpleNameExpression
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::GenericNameExpression;
        }

        static constexpr bool ClassOf(NameExpression const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr GenericNameExpression(tokenizer::IdentifierToken* identifier)
            : SimpleNameExpression{SyntaxKind::GenericNameExpression, identifier}
        {
        }

        //TypeParameterList* Parameters;
    };
}
