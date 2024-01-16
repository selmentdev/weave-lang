#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/SyntaxToken.hxx"
#include "weave/bugcheck/BugCheck.hxx"
#include "weave/syntax/SyntaxTree.hxx"

namespace weave::syntax
{
    template <typename VisitorT, typename ResultT, typename... ArgsT>
    class SyntaxVisitor
    {
    public:
        virtual ~SyntaxVisitor() = default;

    public:
        virtual ResultT Dispatch(SyntaxNode const* node, ArgsT&&... args)
        {
            if (node != nullptr)
            {
                if (SyntaxKindTraits::IsSyntaxToken(node->Kind))
                {
                    return this->OnToken(static_cast<SyntaxToken const*>(node), std::forward<ArgsT>(args)...);
                }

                switch (node->Kind)
                {
#define WEAVE_SYNTAX_NODE_CONCRETE(name, value, spelling) \
    case SyntaxKind::name: \
        return this->On##name(static_cast<name const*>(node), std::forward<ArgsT>(args)...);
#include "weave/syntax/SyntaxKind.inl"

                default:
                    WEAVE_BUGCHECK("Invalid node kind");
                }
            }

            return ResultT{};
        }

        virtual ResultT OnToken(SyntaxToken const* token, ArgsT&&... args)
        {
            return static_cast<VisitorT*>(this)->OnDefault(token, std::forward<ArgsT>(args)...);
        }

        virtual ResultT OnTrivia(
            [[maybe_unused]] SyntaxTrivia const* trivia,
            [[maybe_unused]] ArgsT&&... args)
        {
        }

        virtual ResultT OnDefault(
            [[maybe_unused]] SyntaxNode const* node,
            [[maybe_unused]] ArgsT&&... args)
        {
            return ResultT{};
        }

#define WEAVE_SYNTAX_NODE(name, value, spelling) \
    virtual ResultT On##name(name const* node, ArgsT&&... args) \
    { \
        return static_cast<VisitorT*>(this)->OnDefault(node, std::forward<ArgsT>(args)...); \
    }
#include "weave/syntax/SyntaxKind.inl"
    };
}

namespace weave::syntax
{
    class SyntaxWalker : public SyntaxVisitor<SyntaxWalker, void>
    {
    public:
        size_t Depth = 0;

    public:
        void OnCompilationUnitSyntax(CompilationUnitSyntax const* node) override;
        void OnSyntaxList(SyntaxList const* node) override;
        void OnNamespaceDeclarationSyntax(NamespaceDeclarationSyntax const* node) override;
        void OnStructDeclarationSyntax(StructDeclarationSyntax const* node) override;
        void OnConceptDeclarationSyntax(ConceptDeclarationSyntax const* node) override;
        void OnExtendDeclarationSyntax(ExtendDeclarationSyntax const* node) override;
        void OnIncompleteDeclarationSyntax(IncompleteDeclarationSyntax const* node) override;
        void OnQualifiedNameSyntax(QualifiedNameSyntax const* node) override;
        void OnFunctionDeclarationSyntax(FunctionDeclarationSyntax const* node) override;
        void OnUsingDirectiveSyntax(UsingDirectiveSyntax const* node) override;
        void OnIdentifierNameSyntax(IdentifierNameSyntax const* node) override;
        void OnParameterListSyntax(ParameterListSyntax const* node) override;
        void OnParameterSyntax(ParameterSyntax const* node) override;
        void OnTypeClauseSyntax(TypeClauseSyntax const* node) override;
    };
}
