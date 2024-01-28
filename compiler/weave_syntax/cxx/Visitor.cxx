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
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->NamespaceKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Members);

        --this->Depth;
    }

    void SyntaxWalker::OnStructDeclarationSyntax(StructDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->StructKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Members);

        --this->Depth;
    }

    void SyntaxWalker::OnConceptDeclarationSyntax(ConceptDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->ConceptKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Members);

        --this->Depth;
    }

    void SyntaxWalker::OnExtendDeclarationSyntax(ExtendDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->ExtendKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Members);

        --this->Depth;
    }

    void SyntaxWalker::OnIncompleteDeclarationSyntax(IncompleteDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

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
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->FunctionKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Parameters);
        this->Dispatch(node->ReturnType);
        this->Dispatch(node->Body);
        this->Dispatch(node->ExpressionBody);

        --this->Depth;
    }

    void SyntaxWalker::OnUsingDeclarationSyntax(UsingDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->UsingKeyword);
        this->Dispatch(node->Name);

        --this->Depth;
    }

    void SyntaxWalker::OnIdentifierNameSyntax(IdentifierNameSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Identifier);

        --this->Depth;
    }

    void SyntaxWalker::OnTupleIndexSyntax(TupleIndexSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Identifier);

        --this->Depth;
    }

    void SyntaxWalker::OnTupleTypeSyntax(TupleTypeSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenParenToken);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Elements.GetNode());
        this->Dispatch(node->BeforeCloseParenToken);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnTupleTypeElementSyntax(TupleTypeElementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Name);
        this->Dispatch(node->Colon);
        this->Dispatch(node->Type);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnParameterListSyntax(ParameterListSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenParenToken);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Parameters.GetNode());
        this->Dispatch(node->BeforeCloseParenToken);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnParameterSyntax(ParameterSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->Identifier);
        this->Dispatch(node->Type);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnTypeClauseSyntax(TypeClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ColonToken);
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnConstantDeclarationSyntax(ConstantDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->ConstKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Type);
        this->Dispatch(node->Initializer);

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

    void SyntaxWalker::OnPostfixUnaryExpressionSyntax(PostfixUnaryExpressionSyntax* node)
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

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->Members);

        --this->Depth;
    }

    void SyntaxWalker::OnExpressionStatementSyntax(ExpressionStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnIfStatementSyntax(IfStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->IfKeyword);
        this->Dispatch(node->ConditionAttributes.GetNode());
        this->Dispatch(node->Condition);
        this->Dispatch(node->ThenStatement);
        this->Dispatch(node->ElseClause);

        --this->Depth;
    }

    void SyntaxWalker::OnReturnStatementSyntax(ReturnStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->ReturnKeyword);
        this->Dispatch(node->Expression);

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

    void SyntaxWalker::OnWhileStatementSyntax(WhileStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->WhileKeyword);
        this->Dispatch(node->ConditionAttributes.GetNode());
        this->Dispatch(node->Condition);
        this->Dispatch(node->Statement);

        --this->Depth;
    }

    void SyntaxWalker::OnBreakStatementSyntax(BreakStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->BreakKeyword);
        this->Dispatch(node->Label);

        --this->Depth;
    }

    void SyntaxWalker::OnContinueStatementSyntax(ContinueStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->ContinueKeyword);

        --this->Depth;
    }

    void SyntaxWalker::OnGotoStatementSyntax(GotoStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->GotoKeyword);
        this->Dispatch(node->Label);

        --this->Depth;
    }

    void SyntaxWalker::OnVariableDeclarationSyntax(VariableDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->VarKeyword);
        this->Dispatch(node->Identifier);
        this->Dispatch(node->TypeClause);
        this->Dispatch(node->Initializer);

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

    void SyntaxWalker::OnBalancedTokenSequneceSyntax(BalancedTokenSequneceSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Tokens.GetNode());
        this->Dispatch(node->BeforeCloseParen);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnAttributeListSyntax(AttributeListSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenAttributeToken);
        this->Dispatch(node->Target);
        this->Dispatch(node->Attributes.GetNode());

        this->Dispatch(node->BeforeCloseAttributeToken);
        this->Dispatch(node->CloseAttributeToken);

        --this->Depth;
    }

    void SyntaxWalker::OnAttributeSyntax(AttributeSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Name);
        this->Dispatch(node->Tokens);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnSourceFileSyntax(SourceFileSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Elements.GetNode());
        this->Dispatch(node->BeforeEndOfFileToken);
        this->Dispatch(node->EndOfFileToken);

        --this->Depth;
    }

    void SyntaxWalker::OnEmptyStatementSyntax(EmptyStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        --this->Depth;
    }

    void SyntaxWalker::OnCodeBlockItemSyntax(CodeBlockItemSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Item);
        this->Dispatch(node->Semicolon);
        this->Dispatch(node->AfterSemicolon);

        --this->Depth;
    }

    void SyntaxWalker::OnCodeBlockSyntax(CodeBlockSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeLeftBrace);
        this->Dispatch(node->LeftBrace);
        this->Dispatch(node->Elements.GetNode());
        this->Dispatch(node->BeforeRightBrace);
        this->Dispatch(node->RightBrace);

        --this->Depth;
    }

    void SyntaxWalker::OnSizeOfExpressionSyntax(SizeOfExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->SizeOfKeyword);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Expression);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnAlignOfExpressionSyntax(AlignOfExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->AlignOfKeyword);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Expression);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnTypeOfExpressionSyntax(TypeOfExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->TypeOfKeyword);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Expression);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnNameOfExpressionSyntax(NameOfExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->NameOfKeyword);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Expression);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnAddressOfExpressionSyntax(AddressOfExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->AddressOfKeyword);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Expression);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnLabeledStatementSyntax(LabeledStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Unexpected);

        this->Dispatch(node->Name);
        this->Dispatch(node->Colon);
        this->Dispatch(node->Statement);

        --this->Depth;
    }

}
