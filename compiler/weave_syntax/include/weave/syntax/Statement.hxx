#pragma once
#include "weave/syntax/SyntaxNode.hxx"

namespace weave::syntax
{
    class Statement : public SyntaxNode
    {
    public:
        explicit constexpr Statement(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class EmptyStatement final : public Statement
    {
    };

    class BlockStatement final : public Statement
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::BlockStatement;
        }

        static constexpr bool ClassOf(Statement const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr BlockStatement()
            : Statement{SyntaxKind::BlockStatement}
        {
        }

    public:
        tokenizer::Token* OpenBraceToken{};
        std::span<Statement*> Statements{};
        tokenizer::Token* CloseBraceToken{};
    };

    class UsingStatement final : public Statement
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::UsingStatement;
        }

        static constexpr bool ClassOf(Statement const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr UsingStatement()
            : Statement{SyntaxKind::UsingStatement}
        {
        }

    public:
        tokenizer::Token* UsingKeyword{};
        NameExpression* Name{};
        tokenizer::Token* Semicolon{};
    };

    class IfStatement final : public Statement
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::IfStatement;
        }

        static constexpr bool ClassOf(Statement const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr IfStatement()
            : Statement{SyntaxKind::IfStatement}
        {
        }

    public:
        tokenizer::Token* IFKeyword{};
        Expression* Condition{};
        BlockStatement* Body{};
        ElseStatement* Else{};
    };

    class ElseStatement final : public Statement
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::ElseStatement;
        }

        static constexpr bool ClassOf(Statement const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr ElseStatement()
            : Statement{SyntaxKind::ElseStatement}
        {
        }

    public:
        tokenizer::Token* ElseKeyword{};
        BlockStatement* Body{};
    };
}
