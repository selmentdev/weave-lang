#pragma once
// SyntaxTree owns the root and nodes of tree.

namespace weave::syntax2
{
    class CompilationUnit : public SyntaxNode
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::CompilationUnit;
        }

        static constexpr bool ClassOf(CompilationUnit const* node)
        {
            return ClassOf(node->Kind);
        }

    public:
        explicit constexpr CompilationUnit()
            : SyntaxNode{SyntaxKind::CompilationUnit}
        {
        }

    public:
        std::span<MemberDeclaration*> Members{};
        std::span<UsingDirective*> Usings{};
        SyntaxToken* EndOfFile{};
    };

    class Declaration : public SyntaxNode
    {
    public:
        explicit constexpr Declaration(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class MemberDeclaration : public SyntaxNode
    {
    };
}
