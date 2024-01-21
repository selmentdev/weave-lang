#include "weave/syntax/Visitor.hxx"

namespace weave::syntax
{
    void SyntaxWalker::OnCompilationUnitSyntax(CompilationUnitSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->AttributeLists.GetNode());
        this->Dispatch(node->Usings.GetNode());
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->EndOfFileToken);

        --this->Depth;
    }

    void SyntaxWalker::OnSyntaxList(SyntaxList* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        size_t const count = node->GetCount();
        SyntaxNode** elements = node->GetElements();

        for (size_t i = 0; i < count; ++i)
        {
            this->Dispatch(elements[i]);
        }

        --this->Depth;
    }

    void SyntaxWalker::OnNamespaceDeclarationSyntax(NamespaceDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->NamespaceKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Usings.GetNode());
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnStructDeclarationSyntax(StructDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->StructKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnConceptDeclarationSyntax(ConceptDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->ConceptKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnExtendDeclarationSyntax(ExtendDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->ExtendKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnIncompleteDeclarationSyntax(IncompleteDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnQualifiedNameSyntax(QualifiedNameSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Left);
        this->Dispatch(node->DotToken);
        this->Dispatch(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::OnFunctionDeclarationSyntax(FunctionDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->FunctionKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Parameters);
        this->Dispatch(node->ReturnType);
        this->Dispatch(node->Body);
        this->Dispatch(node->ExpressionBody);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnUsingDirectiveSyntax(UsingDirectiveSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->UsingKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnIdentifierNameSyntax(IdentifierNameSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Identifier);

        --this->Depth;
    }

    void SyntaxWalker::OnParameterListSyntax(ParameterListSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Parameters.GetNode());
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnParameterSyntax(ParameterSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Identifier);
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnTypeClauseSyntax(TypeClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ColonToken);
        this->Dispatch(node->Identifier);

        --this->Depth;
    }

    void SyntaxWalker::OnFieldDeclarationSyntax(FieldDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->VarKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Type);
        this->Dispatch(node->Initializer);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnConstantDeclarationSyntax(ConstantDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->ConstKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Type);
        this->Dispatch(node->Initializer);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnEqualsValueClauseSyntax(EqualsValueClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->EqualsToken);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnLiteralExpressionSyntax(LiteralExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->LiteralToken);

        --this->Depth;
    }

    void SyntaxWalker::OnAssignmentExpressionSyntax(AssignmentExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Left);
        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::OnBinaryExpressionSyntax(BinaryExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Left);
        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::OnUnaryExpressionSyntax(UnaryExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Operand);

        --this->Depth;
    }

    void SyntaxWalker::OnPostfixUnaryExpression(PostfixUnaryExpression* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Operand);
        this->Dispatch(node->OperatorToken);

        --this->Depth;
    }

    void SyntaxWalker::OnParenthesizedExpressionSyntax(ParenthesizedExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenParen);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->BetweenOpenParenAndExpression);
        this->Dispatch(node->Expression);
        this->Dispatch(node->BetweenExpressionAndCloseParen);
        this->Dispatch(node->CloseParenToken);
        this->Dispatch(node->AfterCloseParen);

        --this->Depth;
    }

    void SyntaxWalker::OnInvocationExpressionSyntax(InvocationExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->ArgumentList);

        --this->Depth;
    }

    void SyntaxWalker::OnMemberAccessExpressionSyntax(MemberAccessExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Name);

        --this->Depth;
    }

    void SyntaxWalker::OnArgumentListSyntax(ArgumentListSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Arguments.GetNode());
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnArgumentSyntax(ArgumentSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->DirectionKindKeyword);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnBracketedArgumentListSyntax(BracketedArgumentListSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenBracketToken);
        this->Dispatch(node->Arguments.GetNode());
        this->Dispatch(node->CloseBracketToken);

        --this->Depth;
    }

    void SyntaxWalker::OnElementAccessExpressionSyntax(ElementAccessExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->ArgumentList);

        --this->Depth;
    }

    void SyntaxWalker::OnBlockStatementSyntax(BlockStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenBrace);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->BetweenOpenBraceAndStatements);
        this->Dispatch(node->Statements.GetNode());
        this->Dispatch(node->BetweenStatementsAndCloseBrace);
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->AfterCloseBrace);

        --this->Depth;
    }

    void SyntaxWalker::OnExpressionStatementSyntax(ExpressionStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnIfStatementSyntax(IfStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->IfKeyword);
        this->Dispatch(node->Condition);
        this->Dispatch(node->ThenStatement);
        this->Dispatch(node->ElseClause);

        --this->Depth;
    }

    void SyntaxWalker::OnReturnStatementSyntax(ReturnStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ReturnKeyword);
        this->Dispatch(node->Expression);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnElseClauseSyntax(ElseClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ElseKeyword);
        this->Dispatch(node->Statement);

        --this->Depth;
    }

    void SyntaxWalker::OnVariableDeclarationSyntax(VariableDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->VarKeyword);
        this->Dispatch(node->Identifier);
        this->Dispatch(node->TypeClause);
        this->Dispatch(node->Initializer);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnConditionalExpressionSyntax(ConditionalExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Condition);
        this->Dispatch(node->QuestionToken);
        this->Dispatch(node->WhenTrue);
        this->Dispatch(node->ColonToken);
        this->Dispatch(node->WhenFalse);

        --this->Depth;
    }

    void SyntaxWalker::OnArrowExpressionClauseSyntax(ArrowExpressionClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ArrowToken);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnReturnTypeClauseSyntax(ReturnTypeClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ArrowToken);
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnDelegateDeclarationSyntax(DelegateDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        (void)node;
        WEAVE_BUGCHECK("Not implemented");

        //--this->Depth;
    }

    void SyntaxWalker::OnSelfExpressionSyntax(SelfExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->SelfKeyword);

        --this->Depth;
    }

    void SyntaxWalker::OnToken(weave::syntax::SyntaxToken* token)
    {
        this->OnDefault(token);

        ++this->Depth;

        if (this->WithTrivia)
        {
            this->Dispatch(token->LeadingTrivia.GetNode());
            this->Dispatch(token->TrailingTrivia.GetNode());
        }

        --this->Depth;
    }

    void SyntaxWalker::OnUnexpectedNodesSyntax(UnexpectedNodesSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Nodes.GetNode());

        --this->Depth;
    }
}
