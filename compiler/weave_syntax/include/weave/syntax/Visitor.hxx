#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/SyntaxToken.hxx"
#include "weave/bugcheck/BugCheck.hxx"
#include "weave/syntax/SyntaxTree.hxx"

namespace weave::syntax
{
    template <typename ResultT, typename... ArgsT>
    class SyntaxVisitor
    {
    public:
        virtual ~SyntaxVisitor() = default;

    public:
        virtual ResultT Dispatch(SyntaxNode* node, ArgsT&&... args)
        {
            if (node != nullptr)
            {
                switch (node->Kind) // NOLINT(clang-diagnostic-switch-enum)
                {


#define WEAVE_SYNTAX_NODE(name, spelling) \
    case SyntaxKind::name: \
        return this->On##name(static_cast<name*>(node), std::forward<ArgsT>(args)...);
#include "weave/syntax/SyntaxKind.inl"

                default:
                    if (IsToken(node->Kind))
                    {
                        return this->OnToken(static_cast<SyntaxToken*>(node), std::forward<ArgsT>(args)...);
                    }

                    if (IsTrivia(node->Kind))
                    {
                        return this->OnTrivia(static_cast<SyntaxTrivia*>(node), std::forward<ArgsT>(args)...);
                    }

                    WEAVE_BUGCHECK("Invalid node kind");
                }
            }

            return ResultT{};
        }

        virtual ResultT OnToken(SyntaxToken* token, ArgsT&&... args)
        {
            return this->OnDefault(token, std::forward<ArgsT>(args)...);
        }

        virtual ResultT OnTrivia(
            [[maybe_unused]] SyntaxTrivia* trivia,
            [[maybe_unused]] ArgsT&&... args)
        {
            return this->OnDefault(trivia, std::forward<ArgsT>(args)...);
        }

        virtual ResultT OnDefault(
            [[maybe_unused]] SyntaxNode* node,
            [[maybe_unused]] ArgsT&&... args)
        {
            return ResultT{};
        }

#define WEAVE_SYNTAX_NODE(name, spelling) \
    virtual ResultT On##name(name* node, ArgsT&&... args) \
    { \
        return this->OnDefault(node, std::forward<ArgsT>(args)...); \
    }
#include "weave/syntax/SyntaxKind.inl"
    };
}

namespace weave::syntax
{
    class SyntaxWalker : public SyntaxVisitor<void>
    {
    public:
        size_t Depth = 0;
        bool WithTrivia = false;

        SyntaxWalker() = default;

        SyntaxWalker(bool withTrivia)
            : WithTrivia{withTrivia}
        {
        }

    public:
        void OnCompilationUnitSyntax(CompilationUnitSyntax* node) override;
        void OnSyntaxList(SyntaxList* node) override;
        void OnNamespaceDeclarationSyntax(NamespaceDeclarationSyntax* node) override;
        void OnStructDeclarationSyntax(StructDeclarationSyntax* node) override;
        void OnConceptDeclarationSyntax(ConceptDeclarationSyntax* node) override;
        void OnExtendDeclarationSyntax(ExtendDeclarationSyntax* node) override;
        void OnIncompleteDeclarationSyntax(IncompleteDeclarationSyntax* node) override;
        void OnQualifiedNameSyntax(QualifiedNameSyntax* node) override;
        void OnFunctionDeclarationSyntax(FunctionDeclarationSyntax* node) override;
        void OnUsingDeclarationSyntax(UsingDeclarationSyntax* node) override;
        void OnIdentifierNameSyntax(IdentifierNameSyntax* node) override;
        void OnTupleIndexSyntax(TupleIndexSyntax* node) override;
        void OnTupleTypeSyntax(TupleTypeSyntax* node) override;
        void OnTupleTypeElementSyntax(TupleTypeElementSyntax* node) override;
        void OnParameterListSyntax(ParameterListSyntax* node) override;
        void OnParameterSyntax(ParameterSyntax* node) override;
        void OnTypeClauseSyntax(TypeClauseSyntax* node) override;
        void OnConstantDeclarationSyntax(ConstantDeclarationSyntax* node) override;
        void OnEqualsValueClauseSyntax(EqualsValueClauseSyntax* node) override;
        void OnLiteralExpressionSyntax(LiteralExpressionSyntax* node) override;
        void OnAssignmentExpressionSyntax(AssignmentExpressionSyntax* node) override;
        void OnBinaryExpressionSyntax(BinaryExpressionSyntax* node) override;
        void OnUnaryExpressionSyntax(UnaryExpressionSyntax* node) override;
        void OnPostfixUnaryExpressionSyntax(PostfixUnaryExpressionSyntax* node) override;
        void OnInvocationExpressionSyntax(InvocationExpressionSyntax* node) override;
        void OnMemberAccessExpressionSyntax(MemberAccessExpressionSyntax* node) override;
        void OnArgumentListSyntax(ArgumentListSyntax* node) override;
        void OnArgumentSyntax(ArgumentSyntax* node) override;
        void OnBracketedArgumentListSyntax(BracketedArgumentListSyntax* node) override;
        void OnElementAccessExpressionSyntax(ElementAccessExpressionSyntax* node) override;

        void OnBlockStatementSyntax(BlockStatementSyntax* node) override;
        void OnExpressionStatementSyntax(ExpressionStatementSyntax* node) override;
        void OnReturnStatementSyntax(ReturnStatementSyntax* node) override;
        void OnElseClauseSyntax(ElseClauseSyntax* node) override;
        void OnWhileStatementSyntax(WhileStatementSyntax* node) override;
        void OnBreakStatementSyntax(BreakStatementSyntax* node) override;
        void OnContinueStatementSyntax(ContinueStatementSyntax* node) override;
        void OnGotoStatementSyntax(GotoStatementSyntax* node) override;
        void OnVariableDeclarationSyntax(VariableDeclarationSyntax* node) override;
        void OnConditionalExpressionSyntax(ConditionalExpressionSyntax* node) override;
        void OnArrowExpressionClauseSyntax(ArrowExpressionClauseSyntax* node) override;
        void OnReturnTypeClauseSyntax(ReturnTypeClauseSyntax* node) override;
        void OnDelegateDeclarationSyntax(DelegateDeclarationSyntax* node) override;
        void OnSelfExpressionSyntax(SelfExpressionSyntax* node) override;
        void OnToken(SyntaxToken* token) override;
        void OnUnexpectedNodesSyntax(UnexpectedNodesSyntax* node) override;
        void OnBalancedTokenSequneceSyntax(BalancedTokenSequneceSyntax* node) override;
        void OnAttributeListSyntax(AttributeListSyntax* node) override;
        void OnAttributeSyntax(AttributeSyntax* node) override;
        void OnSourceFileSyntax(SourceFileSyntax* node) override;
        void OnEmptyStatementSyntax(EmptyStatementSyntax* node) override;
        void OnCodeBlockItemSyntax(CodeBlockItemSyntax* node) override;
        void OnCodeBlockSyntax(CodeBlockSyntax* node) override;
        void OnSizeOfExpressionSyntax(SizeOfExpressionSyntax* node) override;
        void OnAlignOfExpressionSyntax(AlignOfExpressionSyntax* node) override;
        void OnTypeOfExpressionSyntax(TypeOfExpressionSyntax* node) override;
        void OnNameOfExpressionSyntax(NameOfExpressionSyntax* node) override;
        void OnAddressOfExpressionSyntax(AddressOfExpressionSyntax* node) override;
        void OnLabelStatementSyntax(LabelStatementSyntax* node) override;
        void OnTupleExpressionSyntax(TupleExpressionSyntax* node) override;
        void OnLabeledExpressionSyntax(LabeledExpressionSyntax* node) override;
        void OnTypeAliasDeclarationSyntax(TypeAliasDeclarationSyntax* node) override;
        void OnTypeGenericParameterSyntax(TypeGenericParameterSyntax* node) override;
        void OnConstGenericParameterSyntax(ConstGenericParameterSyntax* node) override;
        void OnGenericParametersSyntax(GenericParametersSyntax* node) override;
        void OnGenericArgumentSyntax(GenericArgumentSyntax* node) override;
        void OnGenericArgumentsSyntax(GenericArgumentsSyntax* node) override;
        void OnGenericNameSyntax(GenericNameSyntax* node) override;
        void OnEvalExpressionSyntax(EvalExpressionSyntax* node) override;
        void OnYieldStatementSyntax(YieldStatementSyntax* node) override;
        void OnIfExpressionSyntax(IfExpressionSyntax* node) override;
        void OnMatchCaseClauseSyntax(MatchCaseClauseSyntax* node) override;
        void OnMatchDefaultClauseSyntax(MatchDefaultClauseSyntax* node) override;
        void OnMatchExpressionSyntax(MatchExpressionSyntax* node) override;
        void OnArrayTypeSyntax(ArrayTypeSyntax* node) override;
        void OnSliceTypeSyntax(SliceTypeSyntax* node) override;
        void OnArrayExpressionSyntax(ArrayExpressionSyntax* node) override;
    };
}
