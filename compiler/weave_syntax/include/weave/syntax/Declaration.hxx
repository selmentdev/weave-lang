#pragma once
#include "weave/syntax/SyntaxNode.hxx"

namespace weave::syntax
{
    class Declaration : public SyntaxNode
    {
    public:
        explicit constexpr Declaration(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class CompilationUnitDeclaration final : public Declaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::CompilationUnitDeclaration;
        }

        static constexpr bool ClassOf(Declaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr CompilationUnitDeclaration()
            : Declaration{SyntaxKind::CompilationUnitDeclaration}
        {
        }

    public:
        std::span<MemberDeclaration*> Members{};
        std::span<UsingStatement*> Usings{};
        tokenizer::Token* EndOfFile{};
    };

    class MemberDeclaration : public Declaration
    {
    public:
        explicit constexpr MemberDeclaration(SyntaxKind kind)
            : Declaration{kind}
        {
        }
    };

    class IncompleteMemberDeclaration : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::IncompleteMemberDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr IncompleteMemberDeclaration(std::span<tokenizer::Token*> tokens)
            : MemberDeclaration{SyntaxKind::IncompleteMemberDeclaration}
            , Tokens{tokens}
        {
        }

    public:
        std::span<tokenizer::Token*> Tokens{};
    };

    class NamespaceDeclaration final : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::NamespaceDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr NamespaceDeclaration()
            : MemberDeclaration{SyntaxKind::NamespaceDeclaration}
        {
        }

    public:
        tokenizer::Token* NamespaceKeyword{};
        NameExpression* Name{};
        tokenizer::Token* OpenBraceToken{};
        std::span<UsingStatement*> Usings{};
        std::span<MemberDeclaration*> Members{};
        tokenizer::Token* CloseBraceToken{};
    };

    class StructDeclaration : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::StructDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr StructDeclaration()
            : MemberDeclaration{SyntaxKind::StructDeclaration}
        {
        }

    public:
        std::span<tokenizer::Token*> Modifiers{};
        tokenizer::Token* StructKeyword{};
        NameExpression* Name{};
        tokenizer::Token* OpenBraceToken{};
        std::span<MemberDeclaration*> Members{};
        tokenizer::Token* CloseBraceToken{};
        tokenizer::Token* SemicolonToken{};
    };
}
