// ReSharper disable CppClangTidyMiscNoRecursion
// ReSharper disable CppMemberFunctionMayBeConst
#include "weave/syntax/Parser.hxx"

#include "weave/bugcheck/BugCheck.hxx"
#include "weave/syntax/SyntaxTree.hxx"
#include "weave/syntax/SyntaxFacts.hxx"
#include "weave/syntax/Lexer.hxx"

// Implementation details:
// - nesting level is used to determine when to stop token recovery operation
//   - if parser reaches maximum nesting level, it should report all remaining tokens as unexpected tokens of invalid
//     syntax node currently being parsed

namespace weave::syntax
{
    Parser::Parser(
        source::DiagnosticSink* diagnostic,
        SyntaxFactory* factory,
        source::SourceText const& source)
        : _factory{factory}
    {
        Lexer lexer{*diagnostic, source, LexerTriviaMode::All};

        while (SyntaxToken* token = lexer.Lex(*factory))
        {
            this->_tokens.push_back(token);

            if ((token->Kind == SyntaxKind::EndOfFileToken) or (token->Kind == SyntaxKind::None))
            {
                break;
            }
        }

        this->_current = Peek(0);
    }

    SyntaxToken* Parser::Peek(size_t offset) const
    {
        size_t const index = this->_index + offset;

        if (index >= this->_tokens.size())
        {
            return this->_tokens.back();
        }

        return this->_tokens[index];
    }

    SyntaxToken* Parser::Current() const
    {
        WEAVE_ASSERT(this->_current != nullptr);
        return this->_current;
    }

    SyntaxToken* Parser::Next()
    {
        SyntaxToken* current = this->Current();
        ++this->_index;

        if (this->_index >= this->_tokens.size())
        {
            this->_current = this->_tokens.back();
        }
        else
        {
            this->_current = this->_tokens[this->_index];
        }

        return current;
    }

    SyntaxToken* Parser::Match(SyntaxKind kind)
    {
        if (this->Current()->Kind == kind)
        {
            return this->Next();
        }

        return this->_factory->CreateMissingToken(kind, this->Current()->Source);
    }

    SyntaxToken* Parser::TryMatch(SyntaxKind kind)
    {
        if (this->Current()->Kind == kind)
        {
            return this->Next();
        }

        return nullptr;
    }

    SyntaxToken* Parser::TryMatch(SyntaxKind k1, SyntaxKind k2)
    {
        if (this->Current()->Kind == k1)
        {
            return this->Next();
        }

        if (this->Current()->Kind == k2)
        {
            return this->Next();
        }

        return nullptr;
    }

    SyntaxToken* Parser::MatchContextualKeyword(SyntaxKind kind)
    {
        WEAVE_ASSERT(IsContextualKeyword(kind));

        if (syntax::IdentifierSyntaxToken* const id = this->Current()->TryCast<syntax::IdentifierSyntaxToken>())
        {
            if (id->ContextualKeyowrd == kind)
            {
                return this->Next();
            }
        }

        return this->_factory->CreateMissingContextualKeyword(kind, this->Current()->Source);
    }

    SyntaxToken* Parser::TryMatchContextualKeyword(SyntaxKind kind)
    {
        WEAVE_ASSERT(IsContextualKeyword(kind));

        if (syntax::IdentifierSyntaxToken* const id = this->Current()->TryCast<syntax::IdentifierSyntaxToken>())
        {
            if (id->ContextualKeyowrd == kind)
            {
                return this->Next();
            }
        }

        return nullptr;
    }

    SyntaxToken* Parser::MatchUntil(std::vector<SyntaxNode*>& unexpected, SyntaxKind kind)
    {
        unexpected.clear();

        // TODO: Process tokens until started depth is reached. This way we won't process whole file as-is.
        while ((this->Current()->Kind != kind) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
        {
            unexpected.push_back(this->Next());
        }

        if (this->Current()->Kind == kind)
        {
            return this->Next();
        }

        // TODO: If we reached starting depth, then report error and return missing token.
        // TODO: Combine all unexpected tokens into single source range and report them as-error.

        return this->_factory->CreateMissingToken(
            kind,
            this->Current()->Source /*.WithZeroLength()*/);
    }

    UnexpectedNodesSyntax* Parser::ConsumeUnexpected(SyntaxKind kind)
    {
        std::vector<SyntaxNode*> unexpected{};

        while (this->Current()->Kind == kind)
        {
            unexpected.push_back(this->Next());
        }

        return this->CreateUnexpectedNodes(unexpected);
    }

    void Parser::MatchUntil(SyntaxToken*& matched, UnexpectedNodesSyntax*& unexpected, SyntaxKind kind)
    {
        std::vector<SyntaxNode*> nodes{};

        // TODO:    Process tokens until started depth is reached. This way we won't process whole file as-is.
        //          For example, when matching ')' token we might get out of parent scope by matching '}'.

        while ((this->Current()->Kind != kind) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
        {
            nodes.push_back(this->Next());
        }

        if (this->Current()->Kind == kind)
        {
            matched = this->Next();
        }
        else
        {
            matched = this->_factory->CreateMissingToken(
                kind,
                this->Current()->Source);
        }

        unexpected = this->CreateUnexpectedNodes(nodes);
    }

    SourceFileSyntax* Parser::ParseSourceFile()
    {
        std::vector<CodeBlockItemSyntax*> children{};
        this->ParseCodeBlockItemList(children, true);

        SourceFileSyntax* result = this->_factory->CreateNode<SourceFileSyntax>();
        result->Elements = SyntaxListView<CodeBlockItemSyntax>{this->_factory->CreateList(children)};
        this->MatchUntil(result->EndOfFileToken, result->BeforeEndOfFileToken, SyntaxKind::EndOfFileToken);
        return result;
    }

    void Parser::ParseCodeBlockItemList(std::vector<CodeBlockItemSyntax*>& items, bool global)
    {
        items.clear();

        while (SyntaxToken const* current = this->Current())
        {
            if (current->Kind == SyntaxKind::CloseBraceToken)
            {
                if (not global)
                {
                    break;
                }
            }
            else if (current->Kind == SyntaxKind::EndOfFileToken)
            {
                break;
            }

            if (CodeBlockItemSyntax* result = this->ParseCodeBlockItem())
            {
                items.push_back(result);
            }
            else
            {
                break;
            }
        }
    }

    CodeBlockSyntax* Parser::ParseCodeBlock()
    {
        std::vector<CodeBlockItemSyntax*> items{};

        SyntaxToken* const leftBrace = this->Match(SyntaxKind::OpenBraceToken);
        this->ParseCodeBlockItemList(items, false);

        UnexpectedNodesSyntax* beforeRightBrace{};
        SyntaxToken* rightBrace{};
        this->MatchUntil(rightBrace, beforeRightBrace, SyntaxKind::CloseBraceToken);

        CodeBlockSyntax* result = this->_factory->CreateNode<CodeBlockSyntax>();
        result->LeftBrace = leftBrace;
        result->Elements = SyntaxListView<CodeBlockItemSyntax>{this->_factory->CreateList(items)};
        result->BeforeRightBrace = beforeRightBrace;
        result->RightBrace = rightBrace;
        return result;
    }

    CodeBlockItemSyntax* Parser::ParseCodeBlockItem()
    {
        // Get current state to be able to reset it in case of failure.
        ResetPoint const started = this->GetResetPoint();

        // Declarations, statements and expressions can have attributes.
        SyntaxListView<AttributeListSyntax> const attributes = this->ParseAttributesList();

        // Parse modifiers and decide what to do next.
        ResetPoint const beforeModifiers = this->GetResetPoint();
        std::vector<SyntaxToken*> modifiersList{};
        ParseMemberModifiersList(modifiersList);

        if (SyntaxFacts::IsStartOfDeclaration(this->Current()->Kind))
        {
            SyntaxListView<SyntaxToken> const modifiers{this->_factory->CreateList(modifiersList)};

            DeclarationSyntax* declaration = this->ParseDeclaration(attributes, modifiers);

            CodeBlockItemSyntax* result = this->_factory->CreateNode<CodeBlockItemSyntax>();

            result->Item = declaration;
            result->Semicolon = this->TryMatch(SyntaxKind::SemicolonToken);
            result->AfterSemicolon = this->ConsumeUnexpected(SyntaxKind::SemicolonToken);
            return result;
        }

        // It wasn't a declaration, so reset parser state and try to parse statement - maybe skipped tokens were part of statement.
        this->Reset(beforeModifiers);

        NameColonSyntax* label = this->ParseOptionalNameColon();
        StatementSyntax* statement = this->ParseStatement(attributes);

        if ((label == nullptr) and (statement == nullptr))
        {
            // No luck, reset parser to original state and stop.
            // FIXME: This leaks memory allocated for attributes.
            this->Reset(started);
            return nullptr;
        }

        CodeBlockItemSyntax* result = this->_factory->CreateNode<CodeBlockItemSyntax>();

        if (label != nullptr)
        {
            LabeledStatementSyntax* wrapper = this->_factory->CreateNode<LabeledStatementSyntax>();
            wrapper->Name = label;
            wrapper->Statement = statement;

            result->Item = wrapper;
        }
        else
        {
            result->Item = statement;
        }

        result->Semicolon = this->TryMatch(SyntaxKind::SemicolonToken);
        result->AfterSemicolon = this->ConsumeUnexpected(SyntaxKind::SemicolonToken);
        return result;
    }

    SyntaxListView<AttributeListSyntax> Parser::ParseAttributesList()
    {
        std::vector<AttributeListSyntax*> elements{};

        while (AttributeListSyntax* current = this->ParseAttributeList())
        {
            elements.push_back(current);
        }

        return SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(elements)};
    }

    void Parser::ParseMemberModifiersList(std::vector<SyntaxToken*>& elements)
    {
        while (SyntaxFacts::IsMemberModifier(this->Current()->Kind))
        {
            elements.push_back(this->Next());
        }
    }

    SyntaxListView<SyntaxToken> Parser::ParseFunctionParameterModifiersList()
    {
        std::vector<SyntaxToken*> elements{};

        while (SyntaxFacts::IsFunctionParameterModifier(this->Current()->Kind))
        {
            elements.push_back(this->Next());
        }

        return SyntaxListView<SyntaxToken>{this->_factory->CreateList(elements)};
    }

    SyntaxListView<SyntaxToken> Parser::ParseFunctionArgumentModifierList()
    {
        std::vector<SyntaxToken*> elements{};

        while (SyntaxFacts::IsFunctionParameterModifier(this->Current()->Kind))
        {
            elements.push_back(this->Next());
        }

        return SyntaxListView<SyntaxToken>{this->_factory->CreateList(elements)};
    }

    SyntaxListView<SyntaxToken> Parser::ParseTypeQualifiers()
    {
        std::vector<SyntaxToken*> elements{};

        while (SyntaxFacts::IsTypeQualifier(this->Current()->Kind))
        {
            elements.push_back(this->Next());
        }

        return SyntaxListView<SyntaxToken>{this->_factory->CreateList(elements)};
    }

    SyntaxListView<SyntaxToken> Parser::ParseTypeSpecifierList()
    {
        std::vector<SyntaxToken*> elements{};

        while (SyntaxFacts::IsTypeSpecifier(this->Current()->Kind))
        {
            elements.push_back(this->Next());
        }

        return SyntaxListView<SyntaxToken>{this->_factory->CreateList(elements)};
    }

    VariableDeclarationSyntax* Parser::ParseVariableDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        SyntaxKind const expected = (this->Current()->Kind == SyntaxKind::VarKeyword)
            ? SyntaxKind::VarKeyword
            : SyntaxKind::LetKeyword;

        VariableDeclarationSyntax* result = this->_factory->CreateNode<VariableDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;

        result->BindingSpecifier = this->Match(expected);
        result->Binding = this->ParsePatternBinding();

        return result;
    }

    FunctionDeclarationSyntax* Parser::ParseFunctionDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        FunctionDeclarationSyntax* result = this->_factory->CreateNode<FunctionDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->FunctionKeyword = this->Match(SyntaxKind::FunctionKeyword);
        result->Name = this->ParseIdentifierName();

        result->GenericParameters = this->ParseOptionalGenericParameters();

        result->Parameters = this->ParseParameterList();

        result->ReturnType = this->ParseOptionalReturnTypeClause();

        result->Constraints = this->ParseConstraintClauseSequence();

        // Function declaration will either have a body, expression body or semicolon.
        result->BeforeBody = this->ConsumeUnexpectedIf([](SyntaxKind kind)
            {
                return (kind != SyntaxKind::OpenBraceToken) && (kind != SyntaxKind::EqualsGreaterThanToken) && (kind != SyntaxKind::SemicolonToken);
            });

        if (this->Current()->Kind == SyntaxKind::OpenBraceToken)
        {
            result->Body = this->ParseCodeBlock();
        }
        else if (this->Current()->Kind == SyntaxKind::EqualsGreaterThanToken)
        {
            result->ExpressionBody = this->ParseArrowExpressionClause();
        }

        return result;
    }

    DelegateDeclarationSyntax* Parser::ParseDelegateDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        DelegateDeclarationSyntax* result = this->_factory->CreateNode<DelegateDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->DelegateKeyword = this->Match(SyntaxKind::DelegateKeyword);
        result->Name = this->ParseIdentifierName();

        result->GenericParameters = this->ParseOptionalGenericParameters();

        result->Parameters = this->ParseParameterList();
        result->ReturnType = this->ParseOptionalReturnTypeClause();

        result->Constraints = this->ParseConstraintClauseSequence();

        return result;
    }

    NamespaceDeclarationSyntax* Parser::ParseNamespaceDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        NamespaceDeclarationSyntax* result = this->_factory->CreateNode<NamespaceDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->NamespaceKeyword = this->Match(SyntaxKind::NamespaceKeyword);
        result->Name = this->ParseQualifiedName();
        result->Members = this->ParseCodeBlock();
        return result;
    }

    ConceptDeclarationSyntax* Parser::ParseConceptDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        ConceptDeclarationSyntax* result = this->_factory->CreateNode<ConceptDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->ConceptKeyword = this->Match(SyntaxKind::ConceptKeyword);
        result->Name = this->ParseIdentifierName();
        result->GenericParameters = this->ParseOptionalGenericParameters();
        result->Constraints = this->ParseConstraintClauseSequence();
        result->Members = this->ParseCodeBlock();
        return result;
    }

    ExtendDeclarationSyntax* Parser::ParseExtendDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        ExtendDeclarationSyntax* result = this->_factory->CreateNode<ExtendDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->ExtendKeyword = this->Match(SyntaxKind::ExtendKeyword);
        result->Name = this->ParseIdentifierName();
        result->GenericParameters = this->ParseOptionalGenericParameters();

        if (this->Current()->Kind == SyntaxKind::AsKeyword)
        {
            result->AsKeyword = this->Match(SyntaxKind::AsKeyword);
            result->ConceptType = this->ParseQualifiedName();
        }

        result->Constraints = this->ParseConstraintClauseSequence();

        result->Members = this->ParseCodeBlock();
        return result;
    }

    StructDeclarationSyntax* Parser::ParseStructDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        StructDeclarationSyntax* result = this->_factory->CreateNode<StructDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->StructKeyword = this->Match(SyntaxKind::StructKeyword);
        result->Name = this->ParseIdentifierName();
        result->GenericParameters = this->ParseOptionalGenericParameters();
        result->Constraints = this->ParseConstraintClauseSequence();
        result->Members = this->ParseCodeBlock();
        return result;
    }

    TypeAliasDeclarationSyntax* Parser::ParseTypeAliasDeclaration(SyntaxListView<AttributeListSyntax> attributes, SyntaxListView<SyntaxToken> modifiers)
    {
        SyntaxToken* tokenType = this->Match(SyntaxKind::TypeKeyword);
        NameSyntax* name = this->ParseIdentifierName();
        SyntaxToken* tokenEquals = this->Match(SyntaxKind::EqualsToken);
        TypeSyntax* type = this->ParseType();

        TypeAliasDeclarationSyntax* result = this->_factory->CreateNode<TypeAliasDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->TypeKeyword = tokenType;
        result->Name = name;
        result->EqualsToken = tokenEquals;
        result->Type = type;
        return result;
    }

    ConstantDeclarationSyntax* Parser::ParseConstantDeclaration(SyntaxListView<AttributeListSyntax> attributes, SyntaxListView<SyntaxToken> modifiers)
    {
        ConstantDeclarationSyntax* result = this->_factory->CreateNode<ConstantDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->ConstKeyword = this->Match(SyntaxKind::ConstKeyword);
        result->Name = this->ParseIdentifierName();
        result->Type = this->ParseOptionalTypeClause();
        result->Initializer = this->ParseOptionalInitializerClause();
        return result;
    }

    DeclarationSyntax* Parser::ParseDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        switch (this->Current()->Kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::UsingKeyword:
            return this->ParseUsingDeclaration();

        case SyntaxKind::FunctionKeyword:
            return this->ParseFunctionDeclaration(attributes, modifiers);

        case SyntaxKind::DelegateKeyword:
            return this->ParseDelegateDeclaration(attributes, modifiers);

        case SyntaxKind::VarKeyword:
        case SyntaxKind::LetKeyword:
            return this->ParseVariableDeclaration(attributes, modifiers);

        case SyntaxKind::ConstKeyword:
            return this->ParseConstantDeclaration(attributes, modifiers);

        case SyntaxKind::StructKeyword:
            return this->ParseStructDeclaration(attributes, modifiers);

        case SyntaxKind::ConceptKeyword:
            return this->ParseConceptDeclaration(attributes, modifiers);

        case SyntaxKind::ExtendKeyword:
            return this->ParseExtendDeclaration(attributes, modifiers);

        case SyntaxKind::NamespaceKeyword:
            return this->ParseNamespaceDeclaration(attributes, modifiers);

        case SyntaxKind::TypeKeyword:
            return this->ParseTypeAliasDeclaration(attributes, modifiers);

        case SyntaxKind::EnumKeyword:
            return this->ParseEnumDeclaration(attributes, modifiers);

        default:
            break;
        }

        WEAVE_BUGCHECK("Declaration '{}' not handled", GetName(this->Current()->Kind));
    }

    StatementSyntax* Parser::ParseStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        switch (this->Current()->Kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::ReturnKeyword:
            return this->ParseReturnStatement(attributes);

        case SyntaxKind::OpenBraceToken:
            return this->ParseBlockStatement(attributes);

        case SyntaxKind::SemicolonToken:
            return this->ParseEmptyStatement(attributes);

        case SyntaxKind::WhileKeyword:
            return this->ParseWhileStatement(attributes);

        case SyntaxKind::BreakKeyword:
            return this->ParseBreakStatement(attributes);

        case SyntaxKind::ContinueKeyword:
            return this->ParseContinueStatement(attributes);

        case SyntaxKind::GotoKeyword:
            return this->ParseGotoStatement(attributes);

        case SyntaxKind::YieldKeyword:
            return this->ParseYieldStatement(attributes);

        case SyntaxKind::LoopKeyword:
            return this->ParseLoopStatement(attributes);

        case SyntaxKind::ForKeyword:
            return this->ParseForStatement(attributes);

        case SyntaxKind::ForeachKeyword:
            return this->ParseForeachStatement(attributes);

        case SyntaxKind::CheckedKeyword:
            return this->ParseCheckedStatement(attributes);

        case SyntaxKind::UncheckedKeyword:
            return this->ParseUncheckedStatement(attributes);

        case SyntaxKind::UnsafeKeyword:
            return this->ParseUnsafeStatement(attributes);

        case SyntaxKind::LazyKeyword:
            return this->ParseLazyStatement(attributes);

        default:
            break;
        }

        return this->ParseExpressionStatement(attributes);
    }

    BlockStatementSyntax* Parser::ParseBlockStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        BlockStatementSyntax* result = this->_factory->CreateNode<BlockStatementSyntax>();
        result->Attributes = attributes;
        result->Members = this->ParseCodeBlock();
        return result;
    }

    EmptyStatementSyntax* Parser::ParseEmptyStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        EmptyStatementSyntax* result = this->_factory->CreateNode<EmptyStatementSyntax>();
        result->Attributes = attributes;
        return result;
    }

    YieldStatementSyntax* Parser::ParseYieldStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        YieldStatementSyntax* result = this->_factory->CreateNode<YieldStatementSyntax>();
        result->Attributes = attributes;
        result->YieldKeyword = this->Match(SyntaxKind::YieldKeyword);

        if (this->Current()->Kind == SyntaxKind::BreakKeyword)
        {
            result->KindKeyword = this->Match(SyntaxKind::BreakKeyword);
        }
        else
        {
            if (this->Current()->Kind == SyntaxKind::ReturnKeyword)
            {
                result->KindKeyword = this->Match(SyntaxKind::ReturnKeyword);
            }

            result->Expression = this->ParseExpression();
        }

        return result;
    }

    LoopStatementSyntax* Parser::ParseLoopStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        LoopStatementSyntax* result = this->_factory->CreateNode<LoopStatementSyntax>();
        result->Attributes = attributes;
        result->LoopKeyword = this->Match(SyntaxKind::LoopKeyword);
        result->Body = this->ParseCodeBlock();
        return result;
    }

    ForStatementSyntax* Parser::ParseForStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        ForStatementSyntax* result = this->_factory->CreateNode<ForStatementSyntax>();
        result->Attributes = attributes;
        result->ForKeyword = this->Match(SyntaxKind::ForKeyword);

        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        result->Initializer = this->ParseOptionalExpression();

        result->FirstSemicolonToken = this->Match(SyntaxKind::SemicolonToken);

        result->Condition = this->ParseOptionalExpression();

        result->SecondSemicolonToken = this->Match(SyntaxKind::SemicolonToken);

        result->Expression = this->ParseOptionalExpression();

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        result->Body = this->ParseCodeBlock();

        return result;
    }

    ForeachStatementSyntax* Parser::ParseForeachStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        ForeachStatementSyntax* result = this->_factory->CreateNode<ForeachStatementSyntax>();
        result->Attributes = attributes;
        result->ForeachKeyword = this->Match(SyntaxKind::ForeachKeyword);

        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        SyntaxListView<AttributeListSyntax> const variableAttributes = this->ParseAttributesList();

        result->Variable = this->ParseVariableDeclaration(variableAttributes, {});

        result->InKeyword = this->Match(SyntaxKind::InKeyword);

        result->Expression = this->ParseExpression();

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        result->Body = this->ParseCodeBlock();

        return result;
    }

    CheckedStatementSyntax* Parser::ParseCheckedStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        CheckedStatementSyntax* result = this->_factory->CreateNode<CheckedStatementSyntax>();
        result->Attributes = attributes;
        result->CheckedKeyword = this->Match(SyntaxKind::CheckedKeyword);
        result->Body = this->ParseCodeBlock();
        return result;
    }

    UncheckedStatementSyntax* Parser::ParseUncheckedStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        UncheckedStatementSyntax* result = this->_factory->CreateNode<UncheckedStatementSyntax>();
        result->Attributes = attributes;
        result->UncheckedKeyword = this->Match(SyntaxKind::UncheckedKeyword);
        result->Body = this->ParseCodeBlock();
        return result;
    }

    UnsafeStatementSyntax* Parser::ParseUnsafeStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        UnsafeStatementSyntax* result = this->_factory->CreateNode<UnsafeStatementSyntax>();
        result->Attributes = attributes;
        result->UnsafeKeyword = this->Match(SyntaxKind::UnsafeKeyword);
        result->Body = this->ParseCodeBlock();
        return result;
    }

    LazyStatementSyntax* Parser::ParseLazyStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        LazyStatementSyntax* result = this->_factory->CreateNode<LazyStatementSyntax>();
        result->Attributes = attributes;
        result->LazyKeyword = this->Match(SyntaxKind::LazyKeyword);
        result->Body = this->ParseCodeBlock();
        return result;
    }

    ParameterListSyntax* Parser::ParseParameterList()
    {
        ParameterListSyntax* result = this->_factory->CreateNode<ParameterListSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        std::vector<ParameterSyntax*> items{};

        if (this->Current()->Kind != SyntaxKind::CloseParenToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                ParameterSyntax* parameter = this->ParseParameter();
                items.emplace_back(parameter);

                if (parameter->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Parameters = SyntaxListView<ParameterSyntax>{this->_factory->CreateList(items)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    ParameterSyntax* Parser::ParseParameter()
    {
        SyntaxListView<AttributeListSyntax> const attributes = this->ParseAttributesList();
        SyntaxListView<SyntaxToken> const modifiers = this->ParseFunctionParameterModifiersList();

        NameSyntax* name = this->ParseIdentifierName();
        TypeClauseSyntax* typeClause = this->ParseOptionalTypeClause();
        SyntaxToken* trailingComma = this->TryMatch(SyntaxKind::CommaToken);

        ParameterSyntax* result = this->_factory->CreateNode<ParameterSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Identifier = name;
        result->Type = typeClause;
        result->TrailingComma = trailingComma;
        return result;
    }

    GenericParametersSyntax* Parser::ParseGenericParameters()
    {
        GenericParametersSyntax* result = this->_factory->CreateNode<GenericParametersSyntax>();

        this->MatchUntil(result->OpenToken, result->BeforeOpenToken, SyntaxKind::ExclamationOpenBracketToken);

        std::vector<GenericParameterSyntax*> parameters{};

        if (this->Current()->Kind != SyntaxKind::CloseBracketToken)
        {
            LoopProgressCondition progress{this->Current()};
            bool last = false;

            do
            {
                GenericParameterSyntax* parameter = this->ParseGenericParameter(last);
                parameters.emplace_back(parameter);

                if (last)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Parameters = SyntaxListView<GenericParameterSyntax>{this->_factory->CreateList(parameters)};

        this->MatchUntil(result->CloseToken, result->BeforeCloseToken, SyntaxKind::CloseBracketToken);
        return result;
    }

    GenericParametersSyntax* Parser::ParseOptionalGenericParameters()
    {
        if (this->Current()->Kind == SyntaxKind::ExclamationOpenBracketToken)
        {
            return this->ParseGenericParameters();
        }

        return nullptr;
    }

    TypeGenericParameterSyntax* Parser::ParseTypeGenericParameter(bool& last)
    {
        TypeGenericParameterSyntax* result = this->_factory->CreateNode<TypeGenericParameterSyntax>();
        result->TypeKeyword = this->Match(SyntaxKind::TypeKeyword);
        result->Name = this->ParseIdentifierName();

        result->EqualsToken = this->TryMatch(SyntaxKind::EqualsToken);

        if (result->EqualsToken)
        {
            result->DefaultExpression = this->ParseQualifiedName();
        }

        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
        last = result->TrailingComma == nullptr;
        return result;
    }

    ConstGenericParameterSyntax* Parser::ParseConstGenericParameter(bool& last)
    {
        ConstGenericParameterSyntax* result = this->_factory->CreateNode<ConstGenericParameterSyntax>();
        result->ConstKeyword = this->Match(SyntaxKind::ConstKeyword);
        result->Name = this->ParseIdentifierName();

        result->ColonToken = this->TryMatch(SyntaxKind::ColonToken);

        if (result->ColonToken)
        {
            result->Type = this->ParseQualifiedName();
        }

        result->EqualsToken = this->TryMatch(SyntaxKind::EqualsToken);

        if (result->EqualsToken)
        {
            result->DefaultExpression = this->ParseTerm(Precedence::Expression);
        }

        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
        last = result->TrailingComma == nullptr;
        return result;
    }

    GenericParameterSyntax* Parser::ParseGenericParameter(bool& last)
    {
        if (this->Current()->Kind == SyntaxKind::TypeKeyword)
        {
            return this->ParseTypeGenericParameter(last);
        }

        if (this->Current()->Kind == SyntaxKind::ConstKeyword)
        {
            return this->ParseConstGenericParameter(last);
        }

        return nullptr;
    }

    GenericArgumentSyntax* Parser::ParseGenericArgument()
    {
        GenericArgumentSyntax* result = this->_factory->CreateNode<GenericArgumentSyntax>();
        result->Expression = this->ParseExpression();
        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
        return result;
    }

    GenericArgumentsSyntax* Parser::ParseGenericArguments()
    {
        GenericArgumentsSyntax* result = this->_factory->CreateNode<GenericArgumentsSyntax>();
        this->MatchUntil(result->OpenToken, result->BeforeOpenToken, SyntaxKind::ExclamationOpenBracketToken);

        std::vector<GenericArgumentSyntax*> arguments{};

        if (this->Current()->Kind != SyntaxKind::CloseBracketToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                GenericArgumentSyntax* argument = this->ParseGenericArgument();
                arguments.emplace_back(argument);

                if (argument->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Arguments = SyntaxListView<GenericArgumentSyntax>{this->_factory->CreateList(arguments)};

        this->MatchUntil(result->CloseToken, result->BeforeCloseToken, SyntaxKind::CloseBracketToken);
        return result;
    }

    // *** --------------------------------------------------------------------------------------------------------- ***

    AttributeSyntax* Parser::ParseAttribute()
    {
        if (this->Current()->Kind == SyntaxKind::IdentifierToken)
        {
            AttributeSyntax* result = this->_factory->CreateNode<AttributeSyntax>();
            result->Name = this->ParseQualifiedName();
            result->Tokens = this->ParseBalancedTokenSequence(SyntaxKind::OpenParenToken, SyntaxKind::CloseParenToken);
            result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
            return result;
        }

        return nullptr;
    }

    AttributeListSyntax* Parser::ParseAttributeList()
    {
        if (SyntaxToken* openAttributeToken = this->TryMatch(SyntaxKind::HashOpenBracketToken))
        {
            std::vector<AttributeSyntax*> attributes{};

            while (AttributeSyntax* current = this->ParseAttribute())
            {
                attributes.emplace_back(current);
            }

            std::vector<SyntaxNode*> unexpected{};
            SyntaxToken* closeAttributeToken = this->MatchUntil(unexpected, SyntaxKind::CloseBracketToken);

            AttributeListSyntax* result = this->_factory->CreateNode<AttributeListSyntax>();
            result->OpenAttributeToken = openAttributeToken;
            result->Attributes = SyntaxListView<AttributeSyntax>{this->_factory->CreateList(attributes)};
            result->BeforeCloseAttributeToken = this->CreateUnexpectedNodes(unexpected);
            result->CloseAttributeToken = closeAttributeToken;
            return result;
        }

        return nullptr;
    }

    UsingDeclarationSyntax* Parser::ParseUsingDeclaration()
    {
        SyntaxToken* tokenUsing = this->Match(SyntaxKind::UsingKeyword);
        NameSyntax* name = this->ParseQualifiedName();

        UsingDeclarationSyntax* result = this->_factory->CreateNode<UsingDeclarationSyntax>();
        result->UsingKeyword = tokenUsing;
        result->Name = name;
        return result;
    }

    ReturnTypeClauseSyntax* Parser::ParseReturnTypeClause()
    {
        ReturnTypeClauseSyntax* result = this->_factory->CreateNode<ReturnTypeClauseSyntax>();
        result->ArrowToken = this->Match(SyntaxKind::MinusGreaterThanToken);

        result->Specifiers = this->ParseTypeSpecifierList();

        result->Name = this->ParseOptionalNameColon();

        result->Type = this->ParseType();
        return result;
    }

    ReturnTypeClauseSyntax* Parser::ParseOptionalReturnTypeClause()
    {
        if (this->Current()->Kind == SyntaxKind::MinusGreaterThanToken)
        {
            return this->ParseReturnTypeClause();
        }

        return nullptr;
    }

    ArrowExpressionClauseSyntax* Parser::ParseArrowExpressionClause()
    {
        ArrowExpressionClauseSyntax* result = this->_factory->CreateNode<ArrowExpressionClauseSyntax>();
        result->ArrowToken = this->Match(SyntaxKind::EqualsGreaterThanToken);
        result->Expression = this->ParseExpression();
        return result;
    }

    TypeClauseSyntax* Parser::ParseTypeClause()
    {
        TypeClauseSyntax* result = this->_factory->CreateNode<TypeClauseSyntax>();
        result->ColonToken = this->Match(SyntaxKind::ColonToken);
        result->Specifiers = this->ParseTypeSpecifierList();
        result->Type = this->ParseType();
        return result;
    }

    TypeClauseSyntax* Parser::ParseOptionalTypeClause()
    {
        if (this->Current()->Kind == SyntaxKind::ColonToken)
        {
            return this->ParseTypeClause();
        }

        return nullptr;
    }

    TypePointerSyntax* Parser::ParsePointerType()
    {
        TypePointerSyntax* result = this->_factory->CreateNode<TypePointerSyntax>();
        result->AsteriskToken = this->Match(SyntaxKind::AsteriskToken);
        result->Qualifiers = this->ParseTypeQualifiers();
        result->Type = this->ParseType();
        return result;
    }

    TypeReferenceSyntax* Parser::ParseTypeReference()
    {
        TypeReferenceSyntax* result = this->_factory->CreateNode<TypeReferenceSyntax>();
        result->AmpersandToken = this->Match(SyntaxKind::AmpersandToken);
        result->Qualifiers = this->ParseTypeQualifiers();
        result->Type = this->ParseType();
        return result;
    }

    ExpressionReferenceSyntax* Parser::ParseExpressionReference()
    {
        ExpressionReferenceSyntax* result = this->_factory->CreateNode<ExpressionReferenceSyntax>();
        result->AmpersandToken = this->Match(SyntaxKind::AmpersandToken);
        result->QualifierToken = this->TryMatch(SyntaxKind::LetKeyword, SyntaxKind::VarKeyword);
        result->Expression = this->ParseExpression();
        return result;
    }

    TypeSyntax* Parser::ParseType()
    {
        switch (this->Current()->Kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::IdentifierToken:
            return this->ParseQualifiedName();

        case SyntaxKind::OpenParenToken:
            return this->ParseTupleType();

        case SyntaxKind::OpenBracketToken:
            return this->ParseArrayOrSliceType();

        case SyntaxKind::AsteriskToken:
            return this->ParsePointerType();

        case SyntaxKind::AmpersandToken:
            return this->ParseTypeReference();

        default:
            break;
        }

        return this->CreateMissingIdentifierName();
    }

    InitializerClauseSyntax* Parser::ParseInitializerClause()
    {
        SyntaxToken* tokenEquals = this->Match(SyntaxKind::EqualsToken);
        ExpressionSyntax* expression = this->ParseExpression();

        InitializerClauseSyntax* result = this->_factory->CreateNode<InitializerClauseSyntax>();
        result->EqualsToken = tokenEquals;
        result->Expression = expression;
        return result;
    }

    InitializerClauseSyntax* Parser::ParseOptionalInitializerClause()
    {
        if (this->Current()->Kind == SyntaxKind::EqualsToken)
        {
            return this->ParseInitializerClause();
        }

        return nullptr;
    }

    ArgumentSyntax* Parser::ParseArgument()
    {
        SyntaxListView<AttributeListSyntax> const attributes = this->ParseAttributesList();
        SyntaxListView<SyntaxToken> const modifiers = this->ParseFunctionArgumentModifierList();

        NameColonSyntax* name = this->ParseOptionalNameColon();

        ExpressionSyntax* expression = this->ParseExpression();

        SyntaxToken* trailingComma = this->TryMatch(SyntaxKind::CommaToken);

        ArgumentSyntax* result = this->_factory->CreateNode<ArgumentSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Name = name;
        result->Expression = expression;
        result->TrailingComma = trailingComma;
        return result;
    }

    ArgumentListSyntax* Parser::ParseArgumentList()
    {
        ArgumentListSyntax* result = this->_factory->CreateNode<ArgumentListSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        std::vector<ArgumentSyntax*> children{};

        if (this->Current()->Kind != SyntaxKind::CloseParenToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                ArgumentSyntax* argument = this->ParseArgument();
                children.emplace_back(argument);

                if (argument->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Arguments = SyntaxListView<ArgumentSyntax>{this->_factory->CreateList(children)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    BracketedArgumentListSyntax* Parser::ParseBracketedArgumentList()
    {
        BracketedArgumentListSyntax* result = this->_factory->CreateNode<BracketedArgumentListSyntax>();
        this->MatchUntil(result->OpenBracketToken, result->BeforeOpenBracketToken, SyntaxKind::OpenBracketToken);

        std::vector<ArgumentSyntax*> children{};

        if (this->Current()->Kind != SyntaxKind::CloseBracketToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                ArgumentSyntax* argument = this->ParseArgument();
                children.emplace_back(argument);

                if (argument->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Arguments = SyntaxListView<ArgumentSyntax>{this->_factory->CreateList(children)};

        this->MatchUntil(result->CloseBracketToken, result->BeforeCloseBracketToken, SyntaxKind::CloseBracketToken);

        return result;
    }

    NameSyntax* Parser::ParseQualifiedName()
    {
        NameSyntax* left = this->ParseSimpleName();

        LoopProgressCondition progress{this->Current()};

        do
        {
            SyntaxToken* const tokenDot = this->Peek(0);
            SyntaxToken* const tokenRight = this->Peek(1);

            if ((tokenDot->Kind == SyntaxKind::ColonColonToken) and (tokenRight->Kind == SyntaxKind::IdentifierToken))
            {
                this->Next();

                SimpleNameSyntax* right = this->ParseSimpleName();
                QualifiedNameSyntax* qualified = this->_factory->CreateNode<QualifiedNameSyntax>();
                qualified->Left = left;
                qualified->DotToken = tokenDot;
                qualified->Right = right;

                left = qualified;
            }
            else
            {
                break;
            }
        } while (progress.Evaluate(this->Current()));

        return left;
    }

    SimpleNameSyntax* Parser::ParseSimpleName()
    {
        if (this->Current()->Kind == SyntaxKind::IntegerLiteralToken)
        {
            return this->ParseTupleIndex();
        }

        if (this->Current()->Kind == SyntaxKind::IdentifierToken)
        {
            if (this->Peek(1)->Kind == SyntaxKind::ExclamationOpenBracketToken)
            {
                GenericNameSyntax* result = this->_factory->CreateNode<GenericNameSyntax>();
                result->Identifier = this->Match(SyntaxKind::IdentifierToken);
                result->GenericArguments = this->ParseGenericArguments();
                return result;
            }
        }

        return this->ParseIdentifierName();
    }

    IdentifierNameSyntax* Parser::ParseIdentifierName()
    {
        SyntaxToken* identifier = this->Match(SyntaxKind::IdentifierToken);
        IdentifierNameSyntax* result = this->_factory->CreateNode<IdentifierNameSyntax>();
        result->Identifier = identifier;
        return result;
    }

    TupleIndexSyntax* Parser::ParseTupleIndex()
    {
        SyntaxToken* index = this->Match(SyntaxKind::IntegerLiteralToken);
        TupleIndexSyntax* result = this->_factory->CreateNode<TupleIndexSyntax>();
        result->Identifier = index;
        return result;
    }

    UnreachableExpressionSyntax* Parser::ParseUnreachableExpression()
    {
        UnreachableExpressionSyntax* result = this->_factory->CreateNode<UnreachableExpressionSyntax>();
        result->UnreachableKeyword = this->Match(SyntaxKind::UnreachableKeyword);
        return result;
    }

    EvalExpressionSyntax* Parser::ParseEvalExpression()
    {
        EvalExpressionSyntax* result = this->_factory->CreateNode<EvalExpressionSyntax>();
        result->EvalKeyword = this->Match(SyntaxKind::EvalKeyword);
        result->Body = this->ParseCodeBlockItem();
        return result;
    }

    ExpressionSyntax* Parser::ParseExpression(Precedence parentPrecedence)
    {
        if (SyntaxFacts::IsInvalidSubexpression(this->Current()->Kind))
        {
            return nullptr;
        }

        ExpressionSyntax* result = nullptr;

        SyntaxKind const unaryOperation = SyntaxFacts::GetPrefixUnaryExpression(this->Current()->Kind);

        if (unaryOperation != SyntaxKind::None)
        {
            Precedence const unaryPrecedence = SyntaxFacts::GetPrecedence(unaryOperation);

            if ((unaryPrecedence != Precedence::Expression) && (unaryPrecedence >= parentPrecedence))
            {
                SyntaxToken* operatorToken = this->Next();
                ExpressionSyntax* operand = this->ParseExpression(unaryPrecedence);

                UnaryExpressionSyntax* left = this->_factory->CreateNode<UnaryExpressionSyntax>();
                left->Operation = unaryOperation;
                left->OperatorToken = operatorToken;
                left->Operand = operand;

                result = left;
            }
        }

        if (result == nullptr)
        {
            result = this->ParseTerm(parentPrecedence);
        }

        auto checkPrecedence = [&](Precedence precedence, SyntaxKind operation)
        {
            if (precedence < parentPrecedence)
            {
                // Check precedence
                return true;
            }

            if ((precedence == parentPrecedence) and (not SyntaxFacts::IsRightAssociative(operation)))
            {
                // Check associativity
                return true;
            }

            return false;
        };

        while (true)
        {
            SyntaxKind const kind = this->Current()->Kind;

            if (SyntaxKind const operation = SyntaxFacts::GetAssignmentExpression(kind); operation != SyntaxKind::None)
            {
                Precedence const precedence = SyntaxFacts::GetPrecedence(operation);

                if (checkPrecedence(precedence, operation))
                {
                    break;
                }

                AssignmentExpressionSyntax* left = this->_factory->CreateNode<AssignmentExpressionSyntax>();
                left->Operation = operation;
                left->Left = result;
                left->OperatorToken = this->Next();
                left->Right = this->ParseExpression(precedence);
                result = left;
                continue;
            }

            if (SyntaxKind const operation = SyntaxFacts::GetBinaryExpression(kind); operation != SyntaxKind::None)
            {
                Precedence const precedence = SyntaxFacts::GetPrecedence(operation);

                if (checkPrecedence(precedence, operation))
                {
                    break;
                }

                // TODO: Figure out how to detect priority inversion when compared to left operand.
                //       Need to figure out how to store type of the node separately.
                //       Syntax tells us about syntactic kind, but later we will need to cast actual node types.
                //
                //       For example, we may store an AddAssignmentExpression as AssignmentExpressionSyntax, but
                //       we can't right now cast it to node type based on syntax kind.

                // if (Precedence leftPrecedence = SyntaxFacts::GetPrecedence(result->))

                BinaryExpressionSyntax* left = this->_factory->CreateNode<BinaryExpressionSyntax>();
                left->Operation = operation;
                left->Left = result;
                left->OperatorToken = this->Next();
                left->Right = this->ParseExpression(precedence);
                result = left;
                continue;
            }

            break;
        }

        if ((this->Current()->Kind != SyntaxKind::QuestionToken) or (parentPrecedence > Precedence::Conditional))
        {
            return result;
        }

        ConditionalExpressionSyntax* conditional = this->_factory->CreateNode<ConditionalExpressionSyntax>();
        conditional->Condition = result;
        conditional->QuestionToken = this->Match(SyntaxKind::QuestionToken);
        conditional->WhenTrue = this->ParseExpression(Precedence::Conditional);
        conditional->ColonToken = this->Match(SyntaxKind::ColonToken);
        conditional->WhenFalse = this->ParseExpression(Precedence::Conditional);
        return conditional;
    }

    ExpressionSyntax* Parser::ParseOptionalExpression(Precedence parentPrecedence)
    {
        if (SyntaxFacts::IsStartOfExpression(this->Current()->Kind))
        {
            return this->ParseExpression(parentPrecedence);
        }

        return nullptr;
    }

    ExpressionSyntax* Parser::ParseTerm(Precedence precedence)
    {
        return this->ParsePostfixExpression(
            this->ParseTermWithoutPostfix(precedence));
    }

    ExpressionSyntax* Parser::ParsePostfixExpression(ExpressionSyntax* expression)
    {
        WEAVE_ASSERT(expression != nullptr);

        while (true)
        {
            switch (this->Current()->Kind) // NOLINT(clang-diagnostic-switch-enum)
            {
            case SyntaxKind::OpenParenToken:
                {
                    InvocationExpressionSyntax* result = this->_factory->CreateNode<InvocationExpressionSyntax>();
                    result->Expression = expression;
                    result->ArgumentList = this->ParseArgumentList();
                    expression = result;
                    continue;
                }

            case SyntaxKind::OpenBracketToken:
                {
                    ElementAccessExpressionSyntax* result = this->_factory->CreateNode<ElementAccessExpressionSyntax>();
                    result->Expression = expression;
                    result->ArgumentList = this->ParseBracketedArgumentList();
                    expression = result;
                    continue;
                }

            case SyntaxKind::DotToken:
                {
                    MemberAccessExpressionSyntax* result = this->_factory->CreateNode<MemberAccessExpressionSyntax>();
                    result->OperationKind = SyntaxKind::SimpleMemberAccessExpression;
                    result->Expression = expression;
                    result->OperatorToken = this->Next();
                    result->Name = this->ParseSimpleName();
                    expression = result;
                    continue;
                }

            case SyntaxKind::PlusPlusToken:
            case SyntaxKind::MinusMinusToken:
                {
                    PostfixUnaryExpressionSyntax* result = this->_factory->CreateNode<PostfixUnaryExpressionSyntax>();
                    result->Operation = SyntaxFacts::GetPostfixUnaryExpression(this->Current()->Kind);
                    result->Operand = expression;
                    result->OperatorToken = this->Next();
                    expression = result;
                    continue;
                }

            default:
                return expression;
            }
        }
    }

    ExpressionSyntax* Parser::ParseTermWithoutPostfix(
        Precedence precedence)
    {
        (void)precedence; // might be used in subexpression

        switch (this->Current()->Kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::OpenParenToken:
            return this->ParseTupleExpression();

        case SyntaxKind::SizeOfKeyword:
            return this->ParseSizeOfExpression();
        case SyntaxKind::AlignOfKeyword:
            return this->ParseAlignOfExpression();
        case SyntaxKind::TypeOfKeyword:
            return this->ParseTypeOfExpression();
        case SyntaxKind::NameOfKeyword:
            return this->ParseNameOfExpression();

        case SyntaxKind::AddressOfKeyword:
            return this->ParseAddressOfExpression();

        case SyntaxKind::OldKeyword:
            return this->ParseOldExpression();

        case SyntaxKind::OutKeyword:
            return this->ParseOutExpression();

        case SyntaxKind::RefKeyword:
            return this->ParseRefExpression();

        case SyntaxKind::MoveKeyword:
            return this->ParseMoveExpression();

        case SyntaxKind::TrueKeyword:
        case SyntaxKind::FalseKeyword:
            return this->ParseBooleanLiteral();

        case SyntaxKind::IntegerLiteralToken:
            return this->ParseIntegerLiteral();

        case SyntaxKind::FloatLiteralToken:
            return this->ParseFloatLiteral();

        case SyntaxKind::StringLiteralToken:
            return this->ParseStringLiteral();

        case SyntaxKind::CharacterLiteralToken:
            return this->ParseCharacterLiteral();

        case SyntaxKind::IdentifierToken:
            {
                NameSyntax* name = this->ParseQualifiedName();

                if (this->Current()->Kind == SyntaxKind::OpenBraceToken)
                {
                    StructExpressionSyntax* result = this->_factory->CreateNode<StructExpressionSyntax>();
                    result->TypeName = name;
                    result->Initializer = this->ParseBraceInitializerClause();
                    return result;
                }
                return name;
            }

        case SyntaxKind::UnreachableKeyword:
            return this->ParseUnreachableExpression();

        case SyntaxKind::EvalKeyword:
            return this->ParseEvalExpression();

        case SyntaxKind::IfKeyword:
            return this->ParseIfExpression();

        case SyntaxKind::MatchKeyword:
            return this->ParseMatchExpression();

        case SyntaxKind::AssertKeyword:
            return this->ParseAssertExpression();

        case SyntaxKind::LetKeyword:
        case SyntaxKind::VarKeyword:
            return this->ParseLetExpression();

        case SyntaxKind::AmpersandToken:
            return this->ParseExpressionReference();

        case SyntaxKind::OpenBracketToken:
            return this->ParseBracketInitializerClause();

        case SyntaxKind::OpenBraceToken:
            WEAVE_BUGCHECK("Object initializer is not supported!");
            // return this->ParseBraceInitializerClause();

        default:
            ExpressionSyntax* missing = this->CreateMissingIdentifierName();
            return missing;
        }
    }

    TupleExpressionSyntax* Parser::ParseTupleExpression()
    {
        std::vector<LabeledExpressionSyntax*> elements{};

        TupleExpressionSyntax* result = this->_factory->CreateNode<TupleExpressionSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        if (this->Current()->Kind != SyntaxKind::CloseParenToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                LabeledExpressionSyntax* element = this->ParseLabeledExpression();
                elements.push_back(element);

                if (element->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Elements = SyntaxListView<LabeledExpressionSyntax>{this->_factory->CreateList(elements)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    BracketInitializerClauseSyntax* Parser::ParseBracketInitializerClause()
    {
        std::vector<LabeledExpressionSyntax*> elements{};

        BracketInitializerClauseSyntax* result = this->_factory->CreateNode<BracketInitializerClauseSyntax>();
        this->MatchUntil(result->OpenBracketToken, result->BeforeOpenBracketToken, SyntaxKind::OpenBracketToken);

        if (this->Current()->Kind != SyntaxKind::CloseBracketToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                if (this->Current()->Kind == SyntaxKind::CloseBracketToken)
                {
                    // Matched optional ',' and then ']'. This might be a trailing comma.
                    break;
                }

                LabeledExpressionSyntax* element = this->ParseLabeledExpression();
                elements.push_back(element);

                if (element->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Elements = SyntaxListView<LabeledExpressionSyntax>{this->_factory->CreateList(elements)};

        this->MatchUntil(result->CloseBracketToken, result->BeforeCloseBracketToken, SyntaxKind::CloseBracketToken);

        return result;
    }

    BraceInitializerClauseSyntax* Parser::ParseBraceInitializerClause()
    {
        std::vector<LabeledExpressionSyntax*> elements{};

        BraceInitializerClauseSyntax* result = this->_factory->CreateNode<BraceInitializerClauseSyntax>();
        this->MatchUntil(result->OpenBraceToken, result->BeforeOpenBraceToken, SyntaxKind::OpenBraceToken);

        if (this->Current()->Kind != SyntaxKind::CloseBraceToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                if (this->Current()->Kind == SyntaxKind::CloseBraceToken)
                {
                    break;
                }

                LabeledExpressionSyntax* element = this->ParseLabeledExpression();
                elements.push_back(element);

                if (element->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Elements = SyntaxListView<LabeledExpressionSyntax>{this->_factory->CreateList(elements)};

        this->MatchUntil(result->CloseBraceToken, result->BeforeCloseBraceToken, SyntaxKind::CloseBraceToken);

        return result;
    }

    LabeledExpressionSyntax* Parser::ParseLabeledExpression()
    {
        NameColonSyntax* name = this->ParseOptionalNameColon();

        ExpressionSyntax* expression = this->ParseOptionalExpression();

        SyntaxToken* trailingComma = this->TryMatch(SyntaxKind::CommaToken);

        LabeledExpressionSyntax* result = this->_factory->CreateNode<LabeledExpressionSyntax>();
        result->Name = name;
        result->Expression = expression;
        result->TrailingComma = trailingComma;
        return result;
    }

    SizeOfExpressionSyntax* Parser::ParseSizeOfExpression()
    {
        SizeOfExpressionSyntax* result = this->_factory->CreateNode<SizeOfExpressionSyntax>();
        result->SizeOfKeyword = this->Match(SyntaxKind::SizeOfKeyword);
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Expression = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        return result;
    }

    AlignOfExpressionSyntax* Parser::ParseAlignOfExpression()
    {
        AlignOfExpressionSyntax* result = this->_factory->CreateNode<AlignOfExpressionSyntax>();
        result->AlignOfKeyword = this->Match(SyntaxKind::AlignOfKeyword);
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Expression = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        return result;
    }

    TypeOfExpressionSyntax* Parser::ParseTypeOfExpression()
    {
        TypeOfExpressionSyntax* result = this->_factory->CreateNode<TypeOfExpressionSyntax>();
        result->TypeOfKeyword = this->Match(SyntaxKind::TypeOfKeyword);
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Expression = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        return result;
    }

    NameOfExpressionSyntax* Parser::ParseNameOfExpression()
    {
        NameOfExpressionSyntax* result = this->_factory->CreateNode<NameOfExpressionSyntax>();
        result->NameOfKeyword = this->Match(SyntaxKind::NameOfKeyword);
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Expression = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        return result;
    }

    AddressOfExpressionSyntax* Parser::ParseAddressOfExpression()
    {
        AddressOfExpressionSyntax* result = this->_factory->CreateNode<AddressOfExpressionSyntax>();
        result->AddressOfKeyword = this->Match(SyntaxKind::AddressOfKeyword);
        result->Expression = this->ParseExpression();
        return result;
    }

    OldExpressionSyntax* Parser::ParseOldExpression()
    {
        OldExpressionSyntax* result = this->_factory->CreateNode<OldExpressionSyntax>();
        result->OldKeyword = this->Match(SyntaxKind::OldKeyword);
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Expression = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        return result;
    }

    OutExpressionSyntax* Parser::ParseOutExpression()
    {
        OutExpressionSyntax* result = this->_factory->CreateNode<OutExpressionSyntax>();
        result->OutKeyword = this->Match(SyntaxKind::OutKeyword);
        result->Expression = this->ParseExpression();
        return result;
    }

    RefExpressionSyntax* Parser::ParseRefExpression()
    {
        RefExpressionSyntax* result = this->_factory->CreateNode<RefExpressionSyntax>();
        result->RefKeyword = this->Match(SyntaxKind::RefKeyword);
        result->Expression = this->ParseExpression();
        return result;
    }

    MoveExpressionSyntax* Parser::ParseMoveExpression()
    {
        MoveExpressionSyntax* result = this->_factory->CreateNode<MoveExpressionSyntax>();
        result->MoveKeyword = this->Match(SyntaxKind::MoveKeyword);
        result->Expression = this->ParseExpression();
        return result;
    }

    ExpressionSyntax* Parser::ParseBooleanLiteral()
    {
        bool const isTrue = this->Current()->Kind == SyntaxKind::TrueKeyword;

        SyntaxToken* literalToken = isTrue ? this->Match(SyntaxKind::TrueKeyword) : this->Match(SyntaxKind::FalseKeyword);

        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    ExpressionSyntax* Parser::ParseIntegerLiteral()
    {
        SyntaxToken* literalToken = this->Match(SyntaxKind::IntegerLiteralToken);
        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    ExpressionSyntax* Parser::ParseFloatLiteral()
    {
        SyntaxToken* literalToken = this->Match(SyntaxKind::FloatLiteralToken);
        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    ExpressionSyntax* Parser::ParseStringLiteral()
    {
        SyntaxToken* literalToken = this->Match(SyntaxKind::StringLiteralToken);
        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    ExpressionSyntax* Parser::ParseCharacterLiteral()
    {
        SyntaxToken* literalToken = this->Match(SyntaxKind::CharacterLiteralToken);
        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    IfExpressionSyntax* Parser::ParseIfExpression()
    {
        IfExpressionSyntax* result = this->_factory->CreateNode<IfExpressionSyntax>();
        result->IfKeyword = this->Match(SyntaxKind::IfKeyword);
        result->ConditionAttributes = this->ParseAttributesList();
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Condition = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        result->Body = this->ParseCodeBlock();
        result->ElseClause = this->ParseOptionalElseClause();

        return result;
    }

    WhileStatementSyntax* Parser::ParseWhileStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        WhileStatementSyntax* result = this->_factory->CreateNode<WhileStatementSyntax>();
        result->Attributes = attributes;
        result->WhileKeyword = this->Match(SyntaxKind::WhileKeyword);
        result->ConditionAttributes = this->ParseAttributesList();
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Condition = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        result->Body = this->ParseCodeBlock();
        return result;
    }

    StatementSyntax* Parser::ParseGotoStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        SyntaxToken* tokenGoto = this->Match(SyntaxKind::GotoKeyword);
        SyntaxToken* label = this->Match(SyntaxKind::IdentifierToken);

        GotoStatementSyntax* result = this->_factory->CreateNode<GotoStatementSyntax>();
        result->Attributes = attributes;
        result->GotoKeyword = tokenGoto;
        result->Label = label;
        return result;
    }

    StatementSyntax* Parser::ParseBreakStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        SyntaxToken* tokenBreak = this->Match(SyntaxKind::BreakKeyword);
        SyntaxToken* label = this->TryMatch(SyntaxKind::IdentifierToken);

        BreakStatementSyntax* result = this->_factory->CreateNode<BreakStatementSyntax>();
        result->Attributes = attributes;
        result->BreakKeyword = tokenBreak;
        result->Label = label;
        return result;
    }

    StatementSyntax* Parser::ParseContinueStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        SyntaxToken* tokenContinue = this->Match(SyntaxKind::ContinueKeyword);

        ContinueStatementSyntax* result = this->_factory->CreateNode<ContinueStatementSyntax>();
        result->Attributes = attributes;
        result->ContinueKeyword = tokenContinue;
        return result;
    }

    ElseClauseSyntax* Parser::ParseOptionalElseClause()
    {
        if (SyntaxToken* tokenElse = this->TryMatch(SyntaxKind::ElseKeyword))
        {
            ElseClauseSyntax* result = this->_factory->CreateNode<ElseClauseSyntax>();
            result->ElseKeyword = tokenElse;

            if (this->Current()->Kind == SyntaxKind::IfKeyword)
            {
                result->Continuation = this->ParseIfExpression();
            }
            else
            {
                result->Body = this->ParseCodeBlock();
            }

            return result;
        }

        return nullptr;
    }

    StatementSyntax* Parser::ParseReturnStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        SyntaxToken* tokenReturn = this->Match(SyntaxKind::ReturnKeyword);
        ExpressionSyntax* expression = nullptr;

        if (this->Current()->Kind != SyntaxKind::SemicolonToken)
        {
            expression = this->ParseExpression();
        }

        ReturnStatementSyntax* result = this->_factory->CreateNode<ReturnStatementSyntax>();
        result->Attributes = attributes;
        result->ReturnKeyword = tokenReturn;
        result->Expression = expression;
        return result;
    }

    ExpressionStatementSyntax* Parser::ParseExpressionStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        if (ExpressionSyntax* expression = this->ParseOptionalExpression())
        {
            ExpressionStatementSyntax* statement = this->_factory->CreateNode<ExpressionStatementSyntax>();
            statement->Attributes = attributes;
            statement->Modifiers = {};
            statement->Expression = expression;
            return statement;
        }

        return nullptr;
    }

    IdentifierNameSyntax* Parser::CreateMissingIdentifierName()
    {
        IdentifierNameSyntax* result = this->_factory->CreateNode<IdentifierNameSyntax>();
        result->Identifier = this->_factory->CreateMissingToken(
            SyntaxKind::IdentifierToken,
            this->Current()->Source /*.WithZeroLength()*/);
        return result;
    }

    UnexpectedNodesSyntax* Parser::CreateUnexpectedNodes(std::span<SyntaxNode*> nodes)
    {
        if (not nodes.empty())
        {
            UnexpectedNodesSyntax* result = this->_factory->CreateNode<UnexpectedNodesSyntax>();
            result->Nodes = SyntaxListView<SyntaxNode>{this->_factory->CreateList(nodes)};
            return result;
        }

        return nullptr;
    }

    BalancedTokenSequenceSyntax* Parser::ParseBalancedTokenSequence(
        SyntaxKind open,
        SyntaxKind close)
    {
        if (SyntaxToken* openToken = this->TryMatch(open))
        {
            std::vector<SyntaxToken*> tokens{};
            std::vector<SyntaxNode*> unexpected{};
            SyntaxToken* closeToken = this->MatchBalancedTokenSequence(close, tokens, unexpected);

            BalancedTokenSequenceSyntax* result = this->_factory->CreateNode<BalancedTokenSequenceSyntax>();
            result->OpenParenToken = openToken;
            result->Tokens = SyntaxListView<SyntaxToken>{this->_factory->CreateList(tokens)};
            result->BeforeCloseParen = this->CreateUnexpectedNodes(unexpected);
            result->CloseParenToken = closeToken;
            return result;
        }

        return nullptr;
    }

    SyntaxToken* Parser::MatchBalancedTokenSequence(
        SyntaxKind terminator,
        std::vector<SyntaxToken*>& tokens,
        std::vector<SyntaxNode*>& unexpected)
    {
        tokens.clear();
        std::vector<SyntaxKind> stack{};

        while (true)
        {
            SyntaxKind const current = this->Current()->Kind;

            if (current == SyntaxKind::EndOfFileToken)
            {
                break;
            }

            if (stack.empty() and current == terminator)
            {
                break;
            }

            switch (current) // NOLINT(clang-diagnostic-switch-enum)
            {
                // Consume opening tokens
            case SyntaxKind::OpenBraceToken:
                {
                    stack.push_back(SyntaxKind::CloseBraceToken);
                    break;
                }

            case SyntaxKind::OpenBracketToken:
                {
                    stack.push_back(SyntaxKind::CloseBracketToken);
                    break;
                }

            case SyntaxKind::OpenParenToken:
                {
                    stack.push_back(SyntaxKind::CloseParenToken);
                    break;
                }

                // Check if proper closing token is on the stack
            case SyntaxKind::CloseBraceToken:
            case SyntaxKind::CloseParenToken:
            case SyntaxKind::CloseBracketToken:
                {
                    if (stack.empty())
                    {
                        // Prematurely exit loop; match until terminator and report other tokens as invalid
                        return this->MatchUntil(unexpected, terminator);
                    }

                    if (stack.back() != current)
                    {
                        // Matched different closing token.
                        // Something like this: `(aaaaa{bbbbb]ccccc)` - `{` does not match `]`
                        return this->MatchUntil(unexpected, terminator);
                    }

                    stack.pop_back();
                    break;
                }

            default:
                break;
            }

            // Append to tokens list.
            tokens.push_back(this->Next());
        }

        return this->MatchUntil(unexpected, terminator);
    }

    TupleTypeSyntax* Parser::ParseTupleType()
    {
        TupleTypeSyntax* result = this->_factory->CreateNode<TupleTypeSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        std::vector<TupleTypeElementSyntax*> elements{};

        if (this->Current()->Kind != SyntaxKind::CloseParenToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                TupleTypeElementSyntax* element = this->ParseTupleTypeElement();
                elements.push_back(element);

                if (element->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Elements = SyntaxListView<TupleTypeElementSyntax>{this->_factory->CreateList(elements)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    TupleTypeSyntax* Parser::ParseOptionalTupleType()
    {
        if (this->Current()->Kind == SyntaxKind::OpenParenToken)
        {
            return this->ParseTupleType();
        }

        return nullptr;
    }

    TupleTypeElementSyntax* Parser::ParseTupleTypeElement()
    {
        TupleTypeElementSyntax* result = this->_factory->CreateNode<TupleTypeElementSyntax>();

        result->Name = this->ParseOptionalNameColon();

        result->Type = this->ParseType();

        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);

        return result;
    }

    std::optional<Parser::Label> Parser::ParseOptionalLabel()
    {
        SyntaxToken* name = this->Current();
        SyntaxToken* colon = this->Peek(1);

        if ((name->Kind == SyntaxKind::IdentifierToken) and (colon->Kind == SyntaxKind::ColonToken))
        {
            this->Next();
            this->Next();

            return Label{name, colon};
        }

        return std::nullopt;
    }

    NameColonSyntax* Parser::ParseNameColon()
    {
        NameColonSyntax* result = this->_factory->CreateNode<NameColonSyntax>();
        result->Name = this->ParseIdentifierName();
        result->ColonToken = this->Match(SyntaxKind::ColonToken);
        return result;
    }

    NameColonSyntax* Parser::ParseOptionalNameColon()
    {
        if ((this->Peek(0)->Kind == SyntaxKind::IdentifierToken) and (this->Peek(1)->Kind == SyntaxKind::ColonToken))
        {
            return this->ParseNameColon();
        }

        return nullptr;
    }

    MatchExpressionSyntax* Parser::ParseMatchExpression()
    {
        MatchExpressionSyntax* result = this->_factory->CreateNode<MatchExpressionSyntax>();
        result->MatchKeyword = this->Match(SyntaxKind::MatchKeyword);
        result->ConditionAttributes = this->ParseAttributesList();
        result->OpenParenToken = this->Match(SyntaxKind::OpenParenToken);
        result->Condition = this->ParseExpression();
        result->CloseParenToken = this->Match(SyntaxKind::CloseParenToken);
        this->MatchUntil(result->LeftBrace, result->BeforeLeftBrace, SyntaxKind::OpenBraceToken);

        {
            std::vector<MatchClauseSyntax*> items{};

            if (this->Current()->Kind != SyntaxKind::CloseBraceToken)
            {
                LoopProgressCondition progress{this->Current()};

                do
                {
                    if (MatchClauseSyntax* item = this->ParseMatchClause())
                    {
                        items.push_back(item);
                    }
                    else
                    {
                        break;
                    }
                } while (progress.Evaluate(this->Current()));
            }

            result->Elements = SyntaxListView<MatchClauseSyntax>{this->_factory->CreateList(items)};
        }

        this->MatchUntil(result->RightBrace, result->BeforeRightBrace, SyntaxKind::CloseBraceToken);
        return result;
    }

    AssertExpressionSyntax* Parser::ParseAssertExpression()
    {
        AssertExpressionSyntax* result = this->_factory->CreateNode<AssertExpressionSyntax>();
        result->AssertKeyword = this->Match(SyntaxKind::AssertKeyword);

        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        result->Level = this->ParseOptionalNameColon();

        result->Condition = this->ParseExpression();

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    LetExpressionSyntax* Parser::ParseLetExpression()
    {
        WEAVE_ASSERT(
            (this->Current()->Kind == SyntaxKind::LetKeyword) or
            (this->Current()->Kind == SyntaxKind::VarKeyword));

        LetExpressionSyntax* result = this->_factory->CreateNode<LetExpressionSyntax>();
        result->BindingSpecifier = this->Next();
        result->Binding = this->ParsePatternBinding();
        return result;
    }

    MatchClauseSyntax* Parser::ParseMatchClause()
    {
        if (this->Current()->Kind == SyntaxKind::CaseKeyword)
        {
            return this->ParseMatchCaseClause();
        }

        if (this->Current()->Kind == SyntaxKind::DefaultKeyword)
        {
            return this->ParseMatchDefaultClause();
        }

        return nullptr;
    }

    MatchCaseClauseSyntax* Parser::ParseMatchCaseClause()
    {
        MatchCaseClauseSyntax* result = this->_factory->CreateNode<MatchCaseClauseSyntax>();
        result->CaseKeyword = this->Match(SyntaxKind::CaseKeyword);
        result->Pattern = this->ParsePattern();
        result->ColonToken = this->Match(SyntaxKind::ColonToken);
        result->Body = this->ParseCodeBlockItem();
        result->TrailingSemicolon = this->TryMatch(SyntaxKind::SemicolonToken);
        return result;
    }

    MatchDefaultClauseSyntax* Parser::ParseMatchDefaultClause()
    {
        MatchDefaultClauseSyntax* result = this->_factory->CreateNode<MatchDefaultClauseSyntax>();
        result->DefaultKeyword = this->Match(SyntaxKind::DefaultKeyword);
        result->ColonToken = this->Match(SyntaxKind::ColonToken);
        result->Body = this->ParseCodeBlockItem();
        result->TrailingSemicolon = this->TryMatch(SyntaxKind::SemicolonToken);
        return result;
    }

    TypeSyntax* Parser::ParseArrayOrSliceType()
    {
        SyntaxToken* openBracket = this->Match(SyntaxKind::OpenBracketToken);
        TypeSyntax* elementType = this->ParseType();

        SyntaxToken* colonToken = this->TryMatch(SyntaxKind::ColonToken);

        ExpressionSyntax* lengthExpression = nullptr;

        if (colonToken != nullptr)
        {
            lengthExpression = this->ParseExpression();
        }

        SyntaxToken* closeBracket = this->Match(SyntaxKind::CloseBracketToken);

        if (colonToken != nullptr)
        {
            ArrayTypeSyntax* result = this->_factory->CreateNode<ArrayTypeSyntax>();
            result->OpenBracketToken = openBracket;
            result->ElementType = elementType;
            result->ColonToken = colonToken;
            result->LengthExpression = lengthExpression;
            result->CloseBracketToken = closeBracket;
            return result;
        }

        SliceTypeSyntax* result = this->_factory->CreateNode<SliceTypeSyntax>();
        result->OpenBracketToken = openBracket;
        result->ElementType = elementType;
        result->CloseBracketToken = closeBracket;
        return result;
    }

    TypeInheritanceClause* Parser::ParseTypeInheritanceClause()
    {
        TypeInheritanceClause* result = this->_factory->CreateNode<TypeInheritanceClause>();
        result->ColonToken = this->Match(SyntaxKind::ColonToken);
        result->BaseType = this->ParseType();
        return result;
    }

    TypeInheritanceClause* Parser::ParseOptionalTypeInheritanceClause()
    {
        if (this->Current()->Kind == SyntaxKind::ColonToken)
        {
            return this->ParseTypeInheritanceClause();
        }

        return nullptr;
    }

    EnumDeclarationSyntax* Parser::ParseEnumDeclaration(SyntaxListView<AttributeListSyntax> attributes, SyntaxListView<SyntaxToken> modifiers)
    {
        EnumDeclarationSyntax* result = this->_factory->CreateNode<EnumDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->EnumKeyword = this->Match(SyntaxKind::EnumKeyword);
        result->Name = this->ParseIdentifierName();
        result->GenericParameters = this->ParseOptionalGenericParameters();
        result->BaseType = this->ParseOptionalTypeInheritanceClause();

        this->MatchUntil(result->OpenBraceToken, result->BeforeOpenBrace, SyntaxKind::OpenBraceToken);

        {
            std::vector<EnumMemberDeclarationSyntax*> items{};

            if (this->Current()->Kind != SyntaxKind::CloseBraceToken)
            {
                LoopProgressCondition progress{this->Current()};

                do
                {
                    if (EnumMemberDeclarationSyntax* item = this->ParseEnumMemberDeclaration())
                    {
                        items.push_back(item);
                    }
                    else
                    {
                        break;
                    }
                } while (progress.Evaluate(this->Current()));
            }

            result->Members = SyntaxListView<EnumMemberDeclarationSyntax>{this->_factory->CreateList(items)};
        }

        this->MatchUntil(result->CloseBraceToken, result->BeforeCloseBrace, SyntaxKind::CloseBraceToken);

        return result;
    }

    EnumMemberDeclarationSyntax* Parser::ParseEnumMemberDeclaration()
    {
        SyntaxListView<AttributeListSyntax> const attributes = this->ParseAttributesList();

        if ((attributes.GetNode() != nullptr) or this->Current()->Kind == SyntaxKind::IdentifierToken)
        {
            EnumMemberDeclarationSyntax* result = this->_factory->CreateNode<EnumMemberDeclarationSyntax>();
            result->Attributes = attributes;
            result->Modifiers = {};
            result->Identifier = this->ParseIdentifierName();
            result->Tuple = this->ParseOptionalTupleType();
            result->Discriminator = this->ParseOptionalInitializerClause();
            result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
            return result;
        }

        return nullptr;
    }

    WhereClauseSyntax* Parser::ParseWhereClause()
    {
        WEAVE_ASSERT(this->Current()->TryCast<IdentifierSyntaxToken>()->ContextualKeyowrd == SyntaxKind::WhereKeyword, "Expected 'where' keyword");

        WhereClauseSyntax* result = this->_factory->CreateNode<WhereClauseSyntax>();
        result->WhereKeyword = this->Next();

        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        result->Type = this->ParseIdentifierName();

        this->MatchUntil(result->Colon, result->BeforeColon, SyntaxKind::ColonToken);

        if (this->Current()->Kind != SyntaxKind::CloseParenToken)
        {
            std::vector<WherePredicateSyntax*> predicates{};

            LoopProgressCondition progress{this->Current()};

            do
            {
                /*
                if (this->Current()->Kind == SyntaxKind::CloseParenToken)
                {
                    break;
                }
                */

                WherePredicateSyntax* item = this->ParseWherePredicate();
                predicates.emplace_back(item);

                if (item->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));

            result->Predicates = SyntaxListView<WherePredicateSyntax>{this->_factory->CreateList(predicates)};
        }

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    WherePredicateSyntax* Parser::ParseWherePredicate()
    {
        WherePredicateSyntax* result = this->_factory->CreateNode<WherePredicateSyntax>();
        result->Type = this->ParseQualifiedName();
        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
        return result;
    }

    ContractClauseSyntax* Parser::ParseContractClause()
    {
        ContractClauseSyntax* result = this->_factory->CreateNode<ContractClauseSyntax>();
        result->Introducer = this->Next();

        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        result->Level = this->ParseOptionalNameColon();

        result->Condition = this->ParseExpression();

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    ConstraintClauseSyntax* Parser::ParseConstraintClause()
    {
        if (IdentifierSyntaxToken* const token = this->Current()->TryCast<IdentifierSyntaxToken>())
        {
            switch (token->ContextualKeyowrd) // NOLINT(clang-diagnostic-switch-enum)
            {
            case SyntaxKind::WhereKeyword:
                return this->ParseWhereClause();

            case SyntaxKind::RequiresKeyword:
            case SyntaxKind::EnsuresKeyword:
            case SyntaxKind::InvariantKeyword:
                return this->ParseContractClause();

            default:
                break;
            }
        }

        return nullptr;
    }

    SyntaxListView<ConstraintClauseSyntax> Parser::ParseConstraintClauseSequence()
    {
        std::vector<ConstraintClauseSyntax*> constraints{};

        LoopProgressCondition progress{this->Current()};

        do
        {
            ConstraintClauseSyntax* constraint = this->ParseConstraintClause();

            if (constraint != nullptr)
            {
                constraints.emplace_back(constraint);
            }
            else
            {
                break;
            }

        } while (progress.Evaluate(this->Current()));

        return SyntaxListView<ConstraintClauseSyntax>{this->_factory->CreateList(constraints)};
    }

    PatternSyntax* Parser::ParsePattern()
    {
        switch (this->Current()->Kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::UnderscoreToken:
            return this->ParseWildcardPattern();

        case SyntaxKind::OpenBracketToken:
            return this->ParseSlicePattern();

        case SyntaxKind::OpenParenToken:
            return this->ParseTuplePattern();

        case SyntaxKind::IdentifierToken:
            // Default to identifier pattern
            break;

        case SyntaxKind::IntegerLiteralToken:
        case SyntaxKind::CharacterLiteralToken:
        case SyntaxKind::FloatLiteralToken:
        case SyntaxKind::StringLiteralToken:
        case SyntaxKind::TrueKeyword:
        case SyntaxKind::FalseKeyword:
            return this->ParseLiteralPattern();

        default:
            break;
        }

        return this->ParseIdentifierPattern();
    }

    WildcardPatternSyntax* Parser::ParseWildcardPattern()
    {
        WildcardPatternSyntax* result = this->_factory->CreateNode<WildcardPatternSyntax>();
        result->WildcardToken = this->Match(SyntaxKind::UnderscoreToken);
        return result;
    }

    LiteralPatternSyntax* Parser::ParseLiteralPattern()
    {
        LiteralPatternSyntax* result = this->_factory->CreateNode<LiteralPatternSyntax>();
        result->LiteralToken = this->Next();
        return result;
    }

    IdentifierPatternSyntax* Parser::ParseIdentifierPattern()
    {
        IdentifierPatternSyntax* result = this->_factory->CreateNode<IdentifierPatternSyntax>();
        result->Identifier = this->ParseQualifiedName();

        if (this->Current()->Kind == SyntaxKind::OpenParenToken)
        {
            result->Pattern = this->ParseTuplePattern();
        }
        else if (this->Current()->Kind == SyntaxKind::OpenBraceToken)
        {
            result->Pattern = this->ParseStructPattern();
        }

        return result;
    }

    SlicePatternItemSyntax* Parser::ParseSlicePatternItem()
    {
        SlicePatternItemSyntax* result = this->_factory->CreateNode<SlicePatternItemSyntax>();
        result->Pattern = this->ParsePattern();
        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
        return result;
    }

    SlicePatternSyntax* Parser::ParseSlicePattern()
    {
        SlicePatternSyntax* result = this->_factory->CreateNode<SlicePatternSyntax>();
        result->OpenBracketToken = this->Match(SyntaxKind::OpenBracketToken);

        std::vector<SlicePatternItemSyntax*> items{};

        if (this->Current()->Kind != SyntaxKind::CloseBracketToken)
        {
            LoopProgressCondition progress{this->Current()};

            do
            {
                SlicePatternItemSyntax* item = this->ParseSlicePatternItem();
                items.push_back(item);

                if (item->TrailingComma == nullptr)
                {
                    break;
                }
            } while (progress.Evaluate(this->Current()));
        }

        result->Items = SyntaxListView<SlicePatternItemSyntax>{this->_factory->CreateList(items)};

        result->CloseBracketToken = this->Match(SyntaxKind::CloseBracketToken);

        return result;
    }

    TuplePatternItemSyntax* Parser::ParseTuplePatternItem()
    {
        TuplePatternItemSyntax* result = this->_factory->CreateNode<TuplePatternItemSyntax>();

        result->Name = this->ParseOptionalNameColon();

        result->Pattern = this->ParsePattern();
        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);
        return result;
    }

    TuplePatternSyntax* Parser::ParseTuplePattern()
    {
        TuplePatternSyntax* result = this->_factory->CreateNode<TuplePatternSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        std::vector<TuplePatternItemSyntax*> items{};

        LoopProgressCondition progress{this->Current()};

        if (this->Current()->Kind != SyntaxKind::CloseParenToken)
        {
            do
            {
                TuplePatternItemSyntax* item = this->ParseTuplePatternItem();
                items.push_back(item);

                if (item->TrailingComma == nullptr)
                {
                    break;
                }

            } while (progress.Evaluate(this->Current()));
        }

        result->Items = SyntaxListView<TuplePatternItemSyntax>{this->_factory->CreateList(items)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    FieldPatternSyntax* Parser::ParseFieldPattern()
    {
        FieldPatternSyntax* result = this->_factory->CreateNode<FieldPatternSyntax>();
        result->Name = this->ParseIdentifierName();

        if (this->Current()->Kind == SyntaxKind::ColonToken)
        {
            result->ColonToken = this->Next();
            result->Pattern = this->ParsePattern();
        }

        result->TrailingComma = this->TryMatch(SyntaxKind::CommaToken);

        return result;
    }

    StructPatternSyntax* Parser::ParseStructPattern()
    {
        StructPatternSyntax* result = this->_factory->CreateNode<StructPatternSyntax>();
        this->MatchUntil(result->OpenBraceToken, result->BeforeOpenBraceToken, SyntaxKind::OpenBraceToken);

        std::vector<FieldPatternSyntax*> items{};

        LoopProgressCondition progress{this->Current()};

        if (this->Current()->Kind != SyntaxKind::CloseBraceToken)
        {
            do
            {
                FieldPatternSyntax* item = this->ParseFieldPattern();
                items.push_back(item);

                if (item->TrailingComma == nullptr)
                {
                    break;
                }

            } while (progress.Evaluate(this->Current()));
        }

        result->Fields = SyntaxListView<FieldPatternSyntax>{this->_factory->CreateList(items)};

        this->MatchUntil(result->CloseBraceToken, result->BeforeOpenBraceToken, SyntaxKind::CloseBraceToken);

        return result;
    }

    PatternBindingSyntax* Parser::ParsePatternBinding()
    {
        PatternSyntax* pattern = this->ParsePattern();
        TypeClauseSyntax* typeClause = this->ParseOptionalTypeClause();
        InitializerClauseSyntax* initializerClause = this->ParseOptionalInitializerClause();

        if (pattern or typeClause or initializerClause)
        {
            PatternBindingSyntax* result = this->_factory->CreateNode<PatternBindingSyntax>();
            result->Pattern = pattern;
            result->Type = typeClause;
            result->Initializer = initializerClause;
            return result;
        }

        return nullptr;
    }
}

#include "weave/syntax/Visitor.hxx"

namespace weave::syntax
{
    class SyntaxValidator : public SyntaxVisitor<void>
    {
    public:
        source::DiagnosticSink* _diagnostic;

    public:
        SyntaxValidator(source::DiagnosticSink* diagnostic)
            : _diagnostic(diagnostic)
        {
            (void)diagnostic;
        }

    public:
    };

    class SyntaxValidatorWalker : public SyntaxWalker
    {
    private:
        SyntaxValidator validator;

    public:
        SyntaxValidatorWalker(source::DiagnosticSink* diagnostic)
            : validator(diagnostic)
        {
        }

    public:
        void OnDefault(SyntaxNode* node) override
        {
            validator.Dispatch(node);
        }
    };

    void Validate(SourceFileSyntax* source, source::DiagnosticSink* diagnostic)
    {
        SyntaxValidatorWalker{diagnostic}.Dispatch(source);
    }
}
