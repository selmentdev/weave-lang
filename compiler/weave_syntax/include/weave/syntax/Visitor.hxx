#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/bugcheck/BugCheck.hxx"

#include "weave/syntax/Statement.hxx"
#include "weave/syntax/Expression.hxx"
#include "weave/syntax/Declaration.hxx"

namespace weave::syntax
{
    template <typename VisitorType, typename ResultType, typename... ArgTypes>
    class SyntaxVisitor
    {
    public:
        virtual ~SyntaxVisitor() = default;

    public:
        ResultType Visit(SyntaxNode* node, ArgTypes&&... args)
        {
            switch (node->Kind())
            {
#define WEAVE_SYNTAX_CONCRETE_NODE(name, spelling) \
    case SyntaxKind::name: \
        return static_cast<VisitorType*>(this)->Visit##name(static_cast<name*>(node), std::forward<ArgTypes>(args)...);
#include "weave/syntax/SyntaxKind.inl"

            default:
                WEAVE_BUGCHECK("Unknown SyntaxKind");
            }
        }

        virtual ResultType VisitDefault([[maybe_unused]] SyntaxNode* node, [[maybe_unused]] ArgTypes&&... args)
        {
            return ResultType{};
        }

#define WEAVE_SYNTAX_CONCRETE_NODE(name, spelling) \
    virtual ResultType Visit##name(name* node, ArgTypes&&... args) \
    { \
        return static_cast<VisitorType*>(this)->VisitDefault(node, std::forward<ArgTypes>(args)...); \
    }
#include "weave/syntax/SyntaxKind.inl"
    };

    class SyntaxWalker : public SyntaxVisitor<SyntaxWalker, void>
    {
    public:
        size_t Depth = 0;

    public:
        ~SyntaxWalker() override = 0;

        void VisitCompilationUnitDeclaration(CompilationUnitDeclaration* node) override;

        void VisitNamespaceDeclaration(NamespaceDeclaration* node) override;

        void VisitUsingStatement(UsingStatement* node) override;

        void VisitQualifiedNameExpression(QualifiedNameExpression* node) override;

        void VisitStructDeclaration(StructDeclaration* node) override;

        void VisitConceptDeclaration(ConceptDeclaration* node) override;

        void VisitExtendDeclaration(ExtendDeclaration* node) override;
    };
}
