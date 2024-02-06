#include "weave/syntax/Visitor.hxx"

namespace weave::syntax
{
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
        this->Dispatch(node->Members);

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
        this->Dispatch(node->GenericParameters);
        this->Dispatch(node->Members);

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
        this->Dispatch(node->GenericParameters);
        this->Dispatch(node->Members);

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
        this->Dispatch(node->GenericParameters);
        this->Dispatch(node->Members);

        --this->Depth;
    }

    void SyntaxWalker::OnIncompleteDeclarationSyntax(IncompleteDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

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
        this->Dispatch(node->GenericParameters);
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

        this->Dispatch(node->ConstKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Type);
        this->Dispatch(node->Initializer);

        --this->Depth;
    }

    void SyntaxWalker::OnInitializerClauseSyntax(InitializerClauseSyntax* node)
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

        this->Dispatch(node->BeforeOpenParenToken);
        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Arguments.GetNode());
        this->Dispatch(node->BeforeCloseParenToken);
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }

    void SyntaxWalker::OnBracketedArgumentListSyntax(BracketedArgumentListSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenBracketToken);
        this->Dispatch(node->OpenBracketToken);
        this->Dispatch(node->Arguments.GetNode());
        this->Dispatch(node->BeforeCloseBracketToken);
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

    void SyntaxWalker::OnArgumentSyntax(ArgumentSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Label);
        this->Dispatch(node->Colon);
        this->Dispatch(node->Expression);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnBlockStatementSyntax(BlockStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->Members);

        --this->Depth;
    }

    void SyntaxWalker::OnExpressionStatementSyntax(ExpressionStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnReturnStatementSyntax(ReturnStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->ReturnKeyword);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnElseClauseSyntax(ElseClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ElseKeyword);
        this->Dispatch(node->Body);
        this->Dispatch(node->Continuation);

        --this->Depth;
    }

    void SyntaxWalker::OnWhileStatementSyntax(WhileStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->WhileKeyword);
        this->Dispatch(node->ConditionAttributes.GetNode());
        this->Dispatch(node->Condition);
        this->Dispatch(node->Body);

        --this->Depth;
    }

    void SyntaxWalker::OnBreakStatementSyntax(BreakStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

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

        this->Dispatch(node->ContinueKeyword);

        --this->Depth;
    }

    void SyntaxWalker::OnGotoStatementSyntax(GotoStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

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

        this->Dispatch(node->BindingSpecifier);
        this->Dispatch(node->Binding);

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

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->DelegateKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->GenericParameters);
        this->Dispatch(node->Parameters);
        this->Dispatch(node->ReturnType);

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

    void SyntaxWalker::OnLabelStatementSyntax(LabelStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->Name);
        this->Dispatch(node->Colon);
        this->Dispatch(node->Statement);

        --this->Depth;
    }

    void SyntaxWalker::OnTupleExpressionSyntax(TupleExpressionSyntax* node)
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

    void SyntaxWalker::OnLabeledExpressionSyntax(LabeledExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Label);
        this->Dispatch(node->Colon);
        this->Dispatch(node->Expression);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnTypeAliasDeclarationSyntax(TypeAliasDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->TypeKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->EqualsToken);
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnTypeGenericParameterSyntax(TypeGenericParameterSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->TypeKeyword);
        this->Dispatch(node->Name);

        // TODO Constraints

        this->Dispatch(node->EqualsToken);
        this->Dispatch(node->DefaultExpression);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnConstGenericParameterSyntax(ConstGenericParameterSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->ConstKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->ColonToken);
        this->Dispatch(node->Type);
        this->Dispatch(node->EqualsToken);
        this->Dispatch(node->DefaultExpression);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnGenericParametersSyntax(GenericParametersSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenToken);
        this->Dispatch(node->OpenToken);
        this->Dispatch(node->Parameters.GetNode());
        this->Dispatch(node->BeforeCloseToken);
        this->Dispatch(node->CloseToken);

        --this->Depth;
    }

    void SyntaxWalker::OnGenericArgumentSyntax(GenericArgumentSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Expression);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnGenericArgumentsSyntax(GenericArgumentsSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenToken);
        this->Dispatch(node->OpenToken);
        this->Dispatch(node->Arguments.GetNode());
        this->Dispatch(node->BeforeCloseToken);
        this->Dispatch(node->CloseToken);

        --this->Depth;
    }

    void SyntaxWalker::OnGenericNameSyntax(GenericNameSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Identifier);
        this->Dispatch(node->GenericArguments);

        --this->Depth;
    }

    void SyntaxWalker::OnEvalExpressionSyntax(EvalExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->EvalKeyword);
        this->Dispatch(node->Body);

        --this->Depth;
    }

    void SyntaxWalker::OnYieldStatementSyntax(YieldStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->YieldKeyword);
        this->Dispatch(node->KindKeyword);
        this->Dispatch(node->Expression);

        --this->Depth;
    }

    void SyntaxWalker::OnIfExpressionSyntax(IfExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->IfKeyword);
        this->Dispatch(node->ConditionAttributes.GetNode());
        this->Dispatch(node->Condition);
        this->Dispatch(node->Body);
        this->Dispatch(node->ElseClause);

        --this->Depth;
    }

    void SyntaxWalker::OnMatchCaseClauseSyntax(MatchCaseClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->CaseKeyword);
        this->Dispatch(node->Pattern);
        this->Dispatch(node->ColonToken);
        this->Dispatch(node->Body);
        this->Dispatch(node->TrailingSemicolon);

        --this->Depth;
    }

    void SyntaxWalker::OnMatchDefaultClauseSyntax(MatchDefaultClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->DefaultKeyword);
        this->Dispatch(node->ColonToken);
        this->Dispatch(node->Body);
        this->Dispatch(node->TrailingSemicolon);

        --this->Depth;
    }

    void SyntaxWalker::OnMatchExpressionSyntax(MatchExpressionSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->MatchKeyword);
        this->Dispatch(node->ConditionAttributes.GetNode());
        this->Dispatch(node->Condition);

        this->Dispatch(node->BeforeLeftBrace);
        this->Dispatch(node->LeftBrace);
        this->Dispatch(node->Elements.GetNode());
        this->Dispatch(node->BeforeRightBrace);
        this->Dispatch(node->RightBrace);

        --this->Depth;
    }

    void SyntaxWalker::OnArrayTypeSyntax(ArrayTypeSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenBracketToken);
        this->Dispatch(node->ElementType);
        this->Dispatch(node->ColonToken);
        this->Dispatch(node->LengthExpression);
        this->Dispatch(node->CloseBracketToken);

        --this->Depth;
    }

    void SyntaxWalker::OnSliceTypeSyntax(SliceTypeSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenBracketToken);
        this->Dispatch(node->ElementType);
        this->Dispatch(node->CloseBracketToken);

        --this->Depth;
    }

    void SyntaxWalker::OnBracketInitializerClauseSyntax(BracketInitializerClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenBracketToken);
        this->Dispatch(node->OpenBracketToken);
        this->Dispatch(node->Elements.GetNode());
        this->Dispatch(node->BeforeCloseBracketToken);
        this->Dispatch(node->CloseBracketToken);
    }

    void SyntaxWalker::OnBraceInitializerClauseSyntax(BraceInitializerClauseSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->BeforeOpenBraceToken);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Elements.GetNode());
        this->Dispatch(node->BeforeCloseBraceToken);
        this->Dispatch(node->CloseBraceToken);
    }

    void SyntaxWalker::OnCheckedStatementSyntax(CheckedStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->CheckedKeyword);
        this->Dispatch(node->Body);

        --this->Depth;
    }

    void SyntaxWalker::OnUncheckedStatementSyntax(UncheckedStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->UncheckedKeyword);
        this->Dispatch(node->Body);

        --this->Depth;
    }

    void SyntaxWalker::OnLoopStatementSyntax(LoopStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->LoopKeyword);
        this->Dispatch(node->Body);

        --this->Depth;
    }

    void SyntaxWalker::OnUnsafeStatementSyntax(UnsafeStatementSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->UnsafeKeyword);
        this->Dispatch(node->Body);

        --this->Depth;
    }

    ////////////////////////////////////////

    void SyntaxWalker::OnWildcardPatternSyntax(WildcardPatternSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->WildcardToken);

        --this->Depth;
    }

    void SyntaxWalker::OnLiteralPatternSyntax(LiteralPatternSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->LiteralToken);

        --this->Depth;
    }

    void SyntaxWalker::OnIdentifierPatternSyntax(IdentifierPatternSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Identifier);
        this->Dispatch(node->Tuple);

        --this->Depth;
    }

    void SyntaxWalker::OnSlicePatternSyntax(SlicePatternSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenBracketToken);
        this->Dispatch(node->Items.GetNode());
        this->Dispatch(node->CloseBracketToken);

        --this->Depth;
    }

    void SyntaxWalker::OnSlicePatternItemSyntax(SlicePatternItemSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Pattern);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnTuplePatternSyntax(TuplePatternSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Items.GetNode());
        this->Dispatch(node->CloseBraceToken);

        --this->Depth;
    }

    void SyntaxWalker::OnTuplePatternItemSyntax(TuplePatternItemSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Identifier);
        this->Dispatch(node->Colon);
        this->Dispatch(node->Pattern);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }

    void SyntaxWalker::OnPatternBindingSyntax(PatternBindingSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Pattern);
        this->Dispatch(node->Type);
        this->Dispatch(node->Initializer);

        --this->Depth;
    }

    void SyntaxWalker::OnEnumDeclarationSyntax(EnumDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());

        this->Dispatch(node->EnumKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->GenericParameters);
        this->Dispatch(node->BaseType);

        this->Dispatch(node->BeforeOpenBrace);
        this->Dispatch(node->OpenBraceToken);

        this->Dispatch(node->Members.GetNode());

        this->Dispatch(node->BeforeCloseBrace);
        this->Dispatch(node->CloseBraceToken);

        --this->Depth;
    }

    void SyntaxWalker::OnEnumMemberDeclarationSyntax(EnumMemberDeclarationSyntax* node)
    {
        this->OnDefault(node);

        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Identifier);
        this->Dispatch(node->Tuple);
        this->Dispatch(node->Discriminator);
        this->Dispatch(node->TrailingComma);

        --this->Depth;
    }
}
