#include "weave/syntax/Visitor.hxx"

namespace weave::syntax
{
    void SyntaxWalker::OnCompilationUnitSyntax(CompilationUnitSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->AttributeLists.GetNode());
        this->Dispatch(node->Usings.GetNode());
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->EndOfFileToken);

        --this->Depth;
    }

    void SyntaxWalker::OnSyntaxList(SyntaxList const* node)
    {
        ++this->Depth;

        size_t const count = node->GetCount();
        SyntaxNode const** elements = node->GetElements();

        for (size_t i = 0; i < count; ++i)
        {
            this->Dispatch(elements[i]);
        }

        --this->Depth;
    }

    void SyntaxWalker::OnNamespaceDeclarationSyntax(NamespaceDeclarationSyntax const* node)
    {
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

    void SyntaxWalker::OnStructDeclarationSyntax(StructDeclarationSyntax const* node)
    {
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

    void SyntaxWalker::OnConceptDeclarationSyntax(ConceptDeclarationSyntax const* node)
    {
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

    void SyntaxWalker::OnExtendDeclarationSyntax(ExtendDeclarationSyntax const* node)
    {
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

    void SyntaxWalker::OnIncompleteDeclarationSyntax(IncompleteDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Type);

        --this->Depth;
    }
    void SyntaxWalker::OnQualifiedNameSyntax(QualifiedNameSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Left);
        this->Dispatch(node->DotToken);
        this->Dispatch(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::OnFunctionDeclarationSyntax(FunctionDeclarationSyntax const* node)
    {
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
    void SyntaxWalker::OnUsingDirectiveSyntax(UsingDirectiveSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->UsingKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }
    void SyntaxWalker::OnIdentifierNameSyntax(IdentifierNameSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Identifier);

        --this->Depth;
    }
    void SyntaxWalker::OnParameterListSyntax(ParameterListSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Parameters.GetNode());
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }
    void SyntaxWalker::OnParameterSyntax(ParameterSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Identifier);
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnTypeClauseSyntax(TypeClauseSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->ColonToken);
        this->Dispatch(node->Identifier);

        --this->Depth;
    }

    void SyntaxWalker::OnFieldDeclarationSyntax(FieldDeclarationSyntax const* node)
    {
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

    void SyntaxWalker::OnConstantDeclarationSyntax(ConstantDeclarationSyntax const* node)
    {
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

    void SyntaxWalker::OnEqualsValueClauseSyntax(EqualsValueClauseSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->EqualsToken);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnLiteralExpressionSyntax(LiteralExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->LiteralToken);

        --this->Depth;
    }

    void SyntaxWalker::OnAssignmentExpressionSyntax(AssignmentExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Left);
        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::OnBinaryExpressionSyntax(BinaryExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Left);
        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::OnUnaryExpressionSyntax(UnaryExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Operand);

        --this->Depth;
    }

    void SyntaxWalker::OnPostfixUnaryExpression(PostfixUnaryExpression const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Operand);
        this->Dispatch(node->OperatorToken);

        --this->Depth;
    }

    void SyntaxWalker::OnParenthesizedExpressionSyntax(ParenthesizedExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Expression);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnInvocationExpressionSyntax(InvocationExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->ArgumentList);

        --this->Depth;
    }

    void SyntaxWalker::OnMemberAccessExpressionSyntax(MemberAccessExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->OperatorToken);
        this->Dispatch(node->Name);

        --this->Depth;
    }

    void SyntaxWalker::OnArgumentListSyntax(ArgumentListSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Arguments.GetNode());
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnArgumentSyntax(ArgumentSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->DirectionKindKeyword);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnBracketedArgumentListSyntax(BracketedArgumentListSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->OpenBracketToken);
        this->Dispatch(node->Arguments.GetNode());
        this->Dispatch(node->CloseBracketToken);

        --this->Depth;
    }

    void SyntaxWalker::OnElementAccessExpressionSyntax(ElementAccessExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->ArgumentList);

        --this->Depth;
    }

    void SyntaxWalker::OnBlockStatementSyntax(BlockStatementSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Statements.GetNode());
        this->Dispatch(node->CloseBraceToken);

        --this->Depth;
    }

    void SyntaxWalker::OnExpressionStatementSyntax(ExpressionStatementSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnIfStatementSyntax(IfStatementSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->IfKeyword);
        this->Dispatch(node->Condition);
        this->Dispatch(node->ThenStatement);
        this->Dispatch(node->ElseClause);

        --this->Depth;
    }

    void SyntaxWalker::OnReturnStatementSyntax(ReturnStatementSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->ReturnKeyword);
        this->Dispatch(node->Expression);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnElseClauseSyntax(ElseClauseSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->ElseKeyword);
        this->Dispatch(node->Statement);

        --this->Depth;
    }

    void SyntaxWalker::OnVariableDeclarationSyntax(VariableDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->VarKeyword);
        this->Dispatch(node->Identifier);
        this->Dispatch(node->TypeClause);
        this->Dispatch(node->Initializer);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnConditionalExpressionSyntax(ConditionalExpressionSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Condition);
        this->Dispatch(node->QuestionToken);
        this->Dispatch(node->WhenTrue);
        this->Dispatch(node->ColonToken);
        this->Dispatch(node->WhenFalse);

        --this->Depth;
    }

    void SyntaxWalker::OnArrowExpressionClauseSyntax(ArrowExpressionClauseSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->ArrowToken);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnReturnTypeClauseSyntax(ReturnTypeClauseSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->ArrowToken);
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnDelegateDeclarationSyntax(DelegateDeclarationSyntax const* node)
    {
        ++this->Depth;

        (void)node;
        //this->Dispatch(node->

        --this->Depth;
    }
}
