// ReSharper disable CppClangTidyMiscNoRecursion
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
        : _diagnostic{diagnostic}
        , _factory{factory}
    {
        Lexer lexer{*this->_diagnostic, source, LexerTriviaMode::All};

        while (SyntaxToken* token = lexer.Lex(*factory))
        {
            this->_tokens.push_back(token);

            if (token->Is(SyntaxKind::EndOfFileToken) or token->Is(SyntaxKind::None))
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
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        if (this->Current()->Is(SyntaxKind::EndOfFileToken))
        {
            this->_diagnostic->AddError(
                this->Current()->Source,
                fmt::format("expected '{}'",
                    GetSpelling(kind)));
        }
        else
        {
            this->_diagnostic->AddError(
                this->Current()->Source,
                fmt::format("unexpected token '{}', expected '{}'",
                    GetSpelling(this->Current()->Kind),
                    GetSpelling(kind)));
        }

        return this->_factory->CreateMissingToken(kind, this->Current()->Source);
    }

    SyntaxToken* Parser::TryMatch(SyntaxKind kind)
    {
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        return nullptr;
    }

    SyntaxToken* Parser::MatchUntil(std::vector<SyntaxNode*>& unexpected, SyntaxKind kind)
    {
        unexpected.clear();

        // TODO: Process tokens until started depth is reached. This way we won't process whole file as-is.
        while ((not this->Current()->Is(kind)) and (not this->Current()->Is(SyntaxKind::EndOfFileToken)))
        {
            unexpected.push_back(this->Next());
        }

        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        // TODO: If we reached starting depth, then report error and return missing token.
        // TODO: Combine all unexpected tokens into single source range and report them as-error.

        if (this->Current()->Kind != SyntaxKind::EndOfFileToken)
        {
            this->_diagnostic->AddError(
                this->Current()->Source,
                fmt::format("unexpected token '{}', expected '{}'",
                    GetSpelling(this->Current()->Kind),
                    GetSpelling(kind)));
        }
        else
        {
            this->_diagnostic->AddError(
                this->Current()->Source,
                fmt::format("unexpected end of file, expected '{}'",
                    GetSpelling(kind)));
        }

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

        while ((not this->Current()->Is(kind)) and (not this->Current()->Is(SyntaxKind::EndOfFileToken)))
        {
            nodes.push_back(this->Next());
        }

        if (this->Current()->Is(kind))
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
            if (current->Is(SyntaxKind::CloseBraceToken))
            {
                if (not global)
                {
                    break;
                }
            }
            else if (current->Is(SyntaxKind::EndOfFileToken))
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
        SyntaxListView<AttributeListSyntax> attributes = this->ParseAttributesList();

        // Parse modifiers and decide what to do next.
        ResetPoint const beforeModifiers = this->GetResetPoint();
        std::vector<SyntaxToken*> modifiersList{};
        ParseMemberModifiersList(modifiersList);

        if (SyntaxFacts::IsStartOfDeclaration(this->Current()->Kind))
        {
            SyntaxListView<SyntaxToken> modifiers{this->_factory->CreateList(modifiersList)};

            DeclarationSyntax* declaration = this->ParseDeclaration(attributes, modifiers);

            CodeBlockItemSyntax* result = this->_factory->CreateNode<CodeBlockItemSyntax>();

            result->Item = declaration;
            result->Semicolon = this->TryMatch(SyntaxKind::SemicolonToken);
            result->AfterSemicolon = this->ConsumeUnexpected(SyntaxKind::SemicolonToken);
            return result;
        }

        // It wasn't a declaration, so reset parser state and try to parse statement - maybe skipped tokens were part of statement.
        this->Reset(beforeModifiers);

        std::optional<Label> label = this->ParseOptionalLabel();

        if (StatementSyntax* statement = this->ParseStatement(attributes))
        {
            CodeBlockItemSyntax* result = this->_factory->CreateNode<CodeBlockItemSyntax>();

            if (label)
            {
                LabelStatementSyntax* wrapper = this->_factory->CreateNode<LabelStatementSyntax>();
                wrapper->Name = label->Name;
                wrapper->Colon = label->Colon;
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

        if (SyntaxFacts::IsStartOfExpression(this->Current()->Kind))
        {
            ExpressionStatementSyntax* statement = this->_factory->CreateNode<ExpressionStatementSyntax>();
            statement->Attributes = attributes;
            statement->Modifiers = {};
            statement->Unexpected = {};
            statement->Expression = this->ParseExpression();

            CodeBlockItemSyntax* result = this->_factory->CreateNode<CodeBlockItemSyntax>();

            if (label)
            {
                LabelStatementSyntax* wrapper = this->_factory->CreateNode<LabelStatementSyntax>();
                wrapper->Name = label->Name;
                wrapper->Colon = label->Colon;
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

        // No luck, reset parser to original state and stop.
        this->Reset(started);
        return nullptr;
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

    SyntaxListView<SyntaxToken> Parser::ParseMemberModifiersList()
    {
        std::vector<SyntaxToken*> elements{};
        ParseMemberModifiersList(elements);
        return SyntaxListView<SyntaxToken>{this->_factory->CreateList(elements)};
    }

    void Parser::ParseMemberModifiersList(std::vector<SyntaxToken*>& elements)
    {
        while (SyntaxToken* current = this->Current())
        {
            if (not SyntaxFacts::IsMemberModifier(current->Kind))
            {
                break;
            }

            elements.push_back(this->Next());
        }
    }

    SyntaxListView<SyntaxToken> Parser::ParseFunctionParameterModifiersList()
    {
        std::vector<SyntaxToken*> elements{};

        while (SyntaxToken* current = this->Current())
        {
            if (not SyntaxFacts::IsFunctionParameterModifier(current->Kind))
            {
                break;
            }

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
        result->VarKeyword = this->Match(expected);
        result->Identifier = this->ParseIdentifierName();
        result->TypeClause = this->ParseOptionalTypeClause();
        result->Initializer = this->ParseOptionalEqualsValueClause();
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

        if (this->Current()->Is(SyntaxKind::OpenBraceToken))
        {
            result->Body = this->ParseCodeBlock();
        }
        else if (this->Current()->Is(SyntaxKind::EqualsGreaterThanToken))
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
        std::vector<ConstraintSyntax*> constraints{};

        ConceptDeclarationSyntax* result = this->_factory->CreateNode<ConceptDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->ConceptKeyword = this->Match(SyntaxKind::ConceptKeyword);
        result->Name = this->ParseIdentifierName();
        result->GenericParameters = this->ParseOptionalGenericParameters();
        result->Members = this->ParseCodeBlock();
        return result;
    }

    ExtendDeclarationSyntax* Parser::ParseExtendDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        std::vector<ConstraintSyntax*> constraints{};

        ExtendDeclarationSyntax* result = this->_factory->CreateNode<ExtendDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->ExtendKeyword = this->Match(SyntaxKind::ExtendKeyword);
        result->Name = this->ParseIdentifierName();
        result->GenericParameters = this->ParseOptionalGenericParameters();
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

    DeclarationSyntax* Parser::ParseDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers)
    {
        switch (this->Current()->Kind)
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

        default:
            break;
        }

        WEAVE_BUGCHECK("Declaration '{}' not handled", GetName(this->Current()->Kind));
    }

    StatementSyntax* Parser::ParseStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        switch (this->Current()->Kind)
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

        default:
            break;
        }

        // return this->ParseExpressionStatement(attributes);
        // WEAVE_BUGCHECK("Statement '{}' not handled", GetName(this->Current()->Kind));
        return nullptr;
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

    YieldStatementSyntax* Parser::ParseYieldStatement(SyntaxListView<AttributeListSyntax> attributes)
    {
        YieldStatementSyntax* result = this->_factory->CreateNode<YieldStatementSyntax>();
        result->Attributes = attributes;
        result->YieldKeyword = this->Match(SyntaxKind::YieldKeyword);

        if (this->Current()->Kind == SyntaxKind::ReturnKeyword)
        {
            result->KindKeyword = this->Match(SyntaxKind::ReturnKeyword);
            result->Expression = this->ParseExpression();
        }
        else if (this->Current()->Kind == SyntaxKind::BreakKeyword)
        {
            result->KindKeyword = this->Match(SyntaxKind::BreakKeyword);
        }
        else
        {
            result->Expression = this->ParseExpression();
        }

        return result;
    }

    ParameterListSyntax* Parser::ParseParameterList()
    {
        ParameterListSyntax* result = this->_factory->CreateNode<ParameterListSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        std::vector<ParameterSyntax*> items{};

        while (SyntaxToken* current = this->Current())
        {
            if (current->Kind == SyntaxKind::CloseParenToken)
            {
                break;
            }
            else if (current->Kind == SyntaxKind::EndOfFileToken)
            {
                break;
            }

            if (ParameterSyntax* parameter = this->ParseParameter())
            {
                items.push_back(parameter);
            }
            else
            {
                break;
            }
        }

        result->Parameters = SyntaxListView<ParameterSyntax>{this->_factory->CreateList(items)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    ParameterSyntax* Parser::ParseParameter()
    {
        // TODO: Reset this state
        SyntaxListView<AttributeListSyntax> attributes = this->ParseAttributesList();
        SyntaxListView<SyntaxToken> modifiers = this->ParseFunctionParameterModifiersList();

        if ((this->Current()->Kind == SyntaxKind::IdentifierToken) or (attributes.GetNode() != nullptr) or (modifiers.GetNode() != nullptr))
        {
            NameSyntax* name = this->ParseIdentifierName();
            TypeClauseSyntax* typeClause = this->ParseTypeClause();
            SyntaxToken* trailingComma = this->TryMatch(SyntaxKind::CommaToken);

            ParameterSyntax* result = this->_factory->CreateNode<ParameterSyntax>();
            result->Attributes = attributes;
            result->Modifiers = modifiers;
            result->Unexpected = nullptr;
            result->Identifier = name;
            result->Type = typeClause;
            result->TrailingComma = trailingComma;
            return result;
        }

        return nullptr;
    }

    GenericParametersSyntax* Parser::ParseGenericParameters()
    {
        GenericParametersSyntax* result = this->_factory->CreateNode<GenericParametersSyntax>();

        this->MatchUntil(result->OpenToken, result->BeforeOpenToken, SyntaxKind::ColonColonOpenBracketToken);

        std::vector<GenericParameterSyntax*> parameters{};

        while (SyntaxToken* current = this->Current())
        {
            if (current->Kind == SyntaxKind::LessThanToken)
            {
                break;
            }

            if (current->Kind == SyntaxKind::EndOfFileToken)
            {
                break;
            }

            if (current->Kind == SyntaxKind::TypeKeyword)
            {
                if (TypeGenericParameterSyntax* parameter = this->ParseTypeGenericParameter())
                {
                    parameters.push_back(parameter);
                }
                else
                {
                    break;
                }
            }
            else if (current->Kind == SyntaxKind::ConstKeyword)
            {
                if (ConstGenericParameterSyntax* parameter = this->ParseConstGenericParameter())
                {
                    parameters.push_back(parameter);
                }

                break;
            }
            else
            {
                break;
            }
        }

        result->Parameters = SyntaxListView<GenericParameterSyntax>{this->_factory->CreateList(parameters)};

        this->MatchUntil(result->CloseToken, result->BeforeCloseToken, SyntaxKind::CloseBracketToken);
        return result;
    }

    GenericParametersSyntax* Parser::ParseOptionalGenericParameters()
    {
        if (this->Current()->Kind == SyntaxKind::ColonColonOpenBracketToken)
        {
            return this->ParseGenericParameters();
        }

        return nullptr;
    }

    TypeGenericParameterSyntax* Parser::ParseTypeGenericParameter()
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
        return result;
    }

    ConstGenericParameterSyntax* Parser::ParseConstGenericParameter()
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
        return result;
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
        this->MatchUntil(result->OpenToken, result->BeforeOpenToken, SyntaxKind::ColonColonOpenBracketToken);

        std::vector<GenericArgumentSyntax*> arguments{};

        while (SyntaxToken* current = this->Current())
        {
            if (current->Kind == SyntaxKind::CloseBracketToken)
            {
                break;
            }

            if (current->Kind == SyntaxKind::EndOfFileToken)
            {
                break;
            }

            if (GenericArgumentSyntax* argument = this->ParseGenericArgument())
            {
                arguments.push_back(argument);
            }
            else
            {
                break;
            }
        }

        result->Arguments = SyntaxListView<GenericArgumentSyntax>{this->_factory->CreateList(arguments)};

        this->MatchUntil(result->CloseToken, result->BeforeCloseToken, SyntaxKind::CloseBracketToken);
        return result;
    }

    // *** --------------------------------------------------------------------------------------------------------- ***

    AttributeSyntax* Parser::ParseAttribute()
    {
        if (this->Current()->Is(SyntaxKind::IdentifierToken))
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
        result->Type = this->ParseType();
        return result;
    }

    ReturnTypeClauseSyntax* Parser::ParseOptionalReturnTypeClause()
    {
        if (this->Current()->Is(SyntaxKind::MinusGreaterThanToken))
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
        SyntaxToken* tokenColon = this->Match(SyntaxKind::ColonToken);
        TypeSyntax* type = this->ParseType();

        TypeClauseSyntax* result = this->_factory->CreateNode<TypeClauseSyntax>();
        result->ColonToken = tokenColon;
        result->Type = type;
        return result;
    }

    TypeClauseSyntax* Parser::ParseOptionalTypeClause()
    {
        if (this->Current()->Is(SyntaxKind::ColonToken))
        {
            return this->ParseTypeClause();
        }

        return nullptr;
    }

    TypeSyntax* Parser::ParseType()
    {
        if (this->Current()->Is(SyntaxKind::IdentifierToken))
        {
            return this->ParseQualifiedName();
        }
        else if (this->Current()->Is(SyntaxKind::OpenParenToken))
        {
            return this->ParseTupleType();
        }
        // else if (this->Current()->Is(SyntaxKind::OpenBracketToken))
        //{
        //     return this->ParseArrayType();
        // })
        //{
        //
        // }

        return this->CreateMissingIdentifierName();
    }

    EqualsValueClauseSyntax* Parser::ParseEqualsValueClause()
    {
        SyntaxToken* tokenEquals = this->Match(SyntaxKind::EqualsToken);
        ExpressionSyntax* expression = this->ParseExpression();

        EqualsValueClauseSyntax* result = this->_factory->CreateNode<EqualsValueClauseSyntax>();
        result->EqualsToken = tokenEquals;
        result->Expression = expression;
        return result;
    }

    EqualsValueClauseSyntax* Parser::ParseOptionalEqualsValueClause()
    {
        if (this->Current()->Is(SyntaxKind::EqualsToken))
        {
            return this->ParseEqualsValueClause();
        }

        return nullptr;
    }

    ArgumentSyntax* Parser::ParseArgument()
    {
        SyntaxToken* tokenDirectionKind = nullptr;

        if (SyntaxFacts::IsFunctionArgumentDirectionKind(this->Current()->Kind))
        {
            tokenDirectionKind = this->Next();
        }

        ExpressionSyntax* expression = this->ParseExpression();

        ArgumentSyntax* result = this->_factory->CreateNode<ArgumentSyntax>();
        result->DirectionKindKeyword = tokenDirectionKind;
        result->Expression = expression;
        return result;
    }

    ArgumentListSyntax* Parser::ParseArgumentList()
    {
        SyntaxToken* tokenOpenParen = this->Match(SyntaxKind::OpenParenToken);

        std::vector<SyntaxNode*> nodes{};

        while ((this->Current()->Kind != SyntaxKind::CloseParenToken) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
        {
            ArgumentSyntax* argument = this->ParseArgument();
            nodes.push_back(argument);

            if (SyntaxToken* comma = this->TryMatch(SyntaxKind::CommaToken))
            {
                nodes.push_back(comma);
            }
            else
            {
                break;
            }
        }

        SyntaxToken* tokenCloseParen = this->Match(SyntaxKind::CloseParenToken);

        ArgumentListSyntax* result = this->_factory->CreateNode<ArgumentListSyntax>();
        result->OpenParenToken = tokenOpenParen;
        result->Arguments = SyntaxListView<ArgumentSyntax>{this->_factory->CreateList(nodes)};
        result->CloseParenToken = tokenCloseParen;
        return result;
    }

    BracketedArgumentListSyntax* Parser::ParseBracketedArgumentList()
    {
        SyntaxToken* tokenOpenBracket = this->Match(SyntaxKind::OpenBracketToken);

        std::vector<SyntaxNode*> nodes{};

        while ((this->Current()->Kind != SyntaxKind::CloseBracketToken) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
        {
            ArgumentSyntax* argument = this->ParseArgument();
            nodes.push_back(argument);

            if (SyntaxToken* comma = this->TryMatch(SyntaxKind::CommaToken))
            {
                nodes.push_back(comma);
            }
            else
            {
                break;
            }
        }

        SyntaxToken* tokenCloseBracket = this->Match(SyntaxKind::CloseBracketToken);

        BracketedArgumentListSyntax* result = this->_factory->CreateNode<BracketedArgumentListSyntax>();
        result->OpenBracketToken = tokenOpenBracket;
        result->Arguments = SyntaxListView<ArgumentSyntax>{this->_factory->CreateList(nodes)};
        result->CloseBracketToken = tokenCloseBracket;
        return result;
    }

    NameSyntax* Parser::ParseQualifiedName()
    {
        NameSyntax* left = this->ParseSimpleName();

        do
        {
            SyntaxToken* tokenDot = this->Peek(0);
            SyntaxToken* tokenRight = this->Peek(1);

            if ((tokenDot->Kind == SyntaxKind::DotToken) and (tokenRight->Kind == SyntaxKind::IdentifierToken))
            {
                // Consume dot token.
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
        } while (true);

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
            if (this->Peek(1)->Kind == SyntaxKind::ColonColonOpenBracketToken)
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

    SelfExpressionSyntax* Parser::ParseSelfExpression()
    {
        SyntaxToken* tokenSelf = this->Match(SyntaxKind::SelfKeyword);
        SelfExpressionSyntax* result = this->_factory->CreateNode<SelfExpressionSyntax>();
        result->SelfKeyword = tokenSelf;
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
        result->Body = this->ParseCodeBlock();
        return result;
    }

    ExpressionSyntax* Parser::ParseExpression(Precedence parentPrecedence)
    {
        if (SyntaxFacts::IsInvalidSubexpression(this->Current()->Kind))
        {
#if true
            return nullptr;
#else
            SyntaxKind const kind = this->Current()->Kind;
            ExpressionSyntax* result = CreateMissingIdentifierName();
            this->_diagnostic->AddError(
                this->Current()->Source,
                fmt::format("invalid expression term: {}", GetSpelling(kind)));
            return result;
#endif
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

        while (true)
        {
            SyntaxKind const kind = this->Current()->Kind;

            if (SyntaxKind const operation = SyntaxFacts::GetAssignmentExpression(kind); operation != SyntaxKind::None)
            {
                Precedence const precedence = SyntaxFacts::GetPrecedence(operation);

                if (precedence < parentPrecedence)
                {
                    // Check precedence
                    break;
                }

                if ((precedence == parentPrecedence) and (not SyntaxFacts::IsRightAssociative(operation)))
                {
                    // Check associativity
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

                // if ((precedence == Precedence::Expression) || (precedence <= parentPrecedence))
                //{
                //     break;
                // }


                if (precedence < parentPrecedence)
                {
                    // Check precedence
                    break;
                }

                if ((precedence == parentPrecedence) and (not SyntaxFacts::IsRightAssociative(operation)))
                {
                    // Check associativity
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

#if false
            SyntaxKind const binaryOperation = SyntaxFacts::GetBinaryExpression(this->Current()->Kind);

            if (binaryOperation == SyntaxKind::None)
            {
                break;
            }

            Precedence const binaryPrecedence = SyntaxFacts::GetPrecedence(binaryOperation);

            if ((binaryPrecedence == Precedence::Expression) || (binaryPrecedence <= parentPrecedence))
            {
                break;
            }

            SyntaxToken* operatorToken = this->Next();
            ExpressionSyntax* right = this->ParseBinaryExpression(binaryPrecedence);

            BinaryExpressionSyntax* left = this->_factory->CreateNode<BinaryExpressionSyntax>();
            left->Operation = binaryOperation;
            left->Left = result;
            left->OperatorToken = operatorToken;
            left->Right = right;
            result = left;
#endif
        }

#if false
        return result;
#else
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
#endif
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
            return this->ParseSimpleName();

        case SyntaxKind::SelfKeyword:
            return this->ParseSelfExpression();

        case SyntaxKind::UnreachableKeyword:
            return this->ParseUnreachableExpression();

        case SyntaxKind::EvalKeyword:
            return this->ParseEvalExpression();

        case SyntaxKind::IfKeyword:
            return this->ParseIfExpression();

        case SyntaxKind::MatchKeyword:
            return this->ParseMatchExpression();

        default:
            ExpressionSyntax* missing = this->CreateMissingIdentifierName();

            if (this->Current()->Kind == SyntaxKind::EndOfFileToken)
            {
                this->_diagnostic->AddError(
                    this->Current()->Source,
                    "expression expected");
            }
            else
            {
                this->_diagnostic->AddError(
                    this->Current()->Source,
                    fmt::format("invalid expression term '{}'", GetSpelling(this->Current()->Kind)));
            }


            return missing;
        }
    }

    TupleExpressionSyntax* Parser::ParseTupleExpression()
    {
        std::vector<LabeledExpressionSyntax*> elements{};

        TupleExpressionSyntax* result = this->_factory->CreateNode<TupleExpressionSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        while (SyntaxToken* current = this->Current())
        {
            if (current->Kind == SyntaxKind::CloseParenToken)
            {
                break;
            }
            else if (current->Kind == SyntaxKind::EndOfFileToken)
            {
                break;
            }

            if (LabeledExpressionSyntax* element = this->ParseLabeledExpression())
            {
                elements.push_back(element);
            }
            else
            {
                break;
            }
        }

        result->Elements = SyntaxListView<LabeledExpressionSyntax>{this->_factory->CreateList(elements)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    LabeledExpressionSyntax* Parser::ParseLabeledExpression()
    {
        SyntaxToken* label = nullptr;
        SyntaxToken* colon = nullptr;

        if ((this->Peek(0)->Kind == SyntaxKind::IdentifierToken) and (this->Peek(1)->Kind == SyntaxKind::ColonToken))
        {
            label = this->Match(SyntaxKind::IdentifierToken);
            colon = this->Match(SyntaxKind::ColonToken);
        }

        ExpressionSyntax* expression = nullptr;

        if (SyntaxFacts::IsStartOfExpression(this->Current()->Kind))
        {
            expression = this->ParseExpression();
        }

        SyntaxToken* trailingComma = this->TryMatch(SyntaxKind::CommaToken);

        if (label or colon or expression)
        {
            LabeledExpressionSyntax* result = this->_factory->CreateNode<LabeledExpressionSyntax>();
            result->Label = label;
            result->Colon = colon;
            result->Expression = expression;
            result->TrailingComma = trailingComma;
            return result;
        }

        return nullptr;
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

    ExpressionSyntax* Parser::ParseBooleanLiteral()
    {
        bool const isTrue = this->Current()->Is(SyntaxKind::TrueKeyword);

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
        result->Condition = this->ParseExpression();
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
        result->Condition = this->ParseExpression();
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
            result->Body = (this->Current()->Kind == SyntaxKind::IfKeyword)
                ? static_cast<SyntaxNode*>(this->ParseIfExpression())
                : static_cast<SyntaxNode*>(this->ParseCodeBlock());
            return result;
        }

        return nullptr;
    }

    StatementSyntax* Parser::ParseReturnStatement(
        SyntaxListView<AttributeListSyntax> attributes)
    {
        SyntaxToken* tokenReturn = this->Match(SyntaxKind::ReturnKeyword);
        ExpressionSyntax* expression = nullptr;

        if (not this->Current()->Is(SyntaxKind::SemicolonToken))
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
        if (ExpressionSyntax* expression = this->ParseExpression())
        {
            ExpressionStatementSyntax* result = this->_factory->CreateNode<ExpressionStatementSyntax>();
            result->Attributes = attributes;
            result->Expression = expression;
            return result;
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

    BalancedTokenSequneceSyntax* Parser::ParseBalancedTokenSequence(
        SyntaxKind open,
        SyntaxKind close)
    {
        if (SyntaxToken* openToken = this->TryMatch(open))
        {
            std::vector<SyntaxToken*> tokens{};
            std::vector<SyntaxNode*> unexpected{};
            SyntaxToken* closeToken = this->MatchBalancedTokenSequence(close, tokens, unexpected);

            BalancedTokenSequneceSyntax* result = this->_factory->CreateNode<BalancedTokenSequneceSyntax>();
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

            switch (current)
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

        // We got here only in two cases:
        // - matches terminator
        // - end of file

        // return this->Match(terminator); // verify
        return this->MatchUntil(unexpected, terminator);
    }

    TupleTypeSyntax* Parser::ParseTupleType()
    {
        TupleTypeSyntax* result = this->_factory->CreateNode<TupleTypeSyntax>();
        this->MatchUntil(result->OpenParenToken, result->BeforeOpenParenToken, SyntaxKind::OpenParenToken);

        std::vector<TupleTypeElementSyntax*> elements{};

        while (SyntaxToken* current = this->Current())
        {
            if (current->Kind == SyntaxKind::CloseParenToken)
            {
                break;
            }
            else if (current->Kind == SyntaxKind::EndOfFileToken)
            {
                break;
            }

            if (TupleTypeElementSyntax* element = this->ParseTupleTypeElement())
            {
                elements.push_back(element);
            }
            else
            {
                break;
            }
        }

        result->Elements = SyntaxListView<TupleTypeElementSyntax>{this->_factory->CreateList(elements)};

        this->MatchUntil(result->CloseParenToken, result->BeforeCloseParenToken, SyntaxKind::CloseParenToken);

        return result;
    }

    TupleTypeElementSyntax* Parser::ParseTupleTypeElement()
    {
        TupleTypeElementSyntax* result = this->_factory->CreateNode<TupleTypeElementSyntax>();

        if ((this->Peek(0)->Kind == SyntaxKind::IdentifierToken) and (this->Peek(1)->Kind == SyntaxKind::ColonToken))
        {
            result->Name = this->Match(SyntaxKind::IdentifierToken);
            result->Colon = this->Match(SyntaxKind::ColonToken);
        }

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

    MatchExpressionSyntax* Parser::ParseMatchExpression()
    {
        MatchExpressionSyntax* result = this->_factory->CreateNode<MatchExpressionSyntax>();
        result->MatchKeyword = this->Match(SyntaxKind::MatchKeyword);
        result->ConditionAttributes = this->ParseAttributesList();
        result->Condition = this->ParseExpression();
        this->MatchUntil(result->LeftBrace, result->BeforeLeftBrace, SyntaxKind::OpenBraceToken);

        {
            std::vector<MatchClauseSyntax*> items{};

            while (SyntaxToken* current = this->Current())
            {
                if (current->Kind == SyntaxKind::CloseBraceToken)
                {
                    break;
                }
                else if (current->Kind == SyntaxKind::EndOfFileToken)
                {
                    break;
                }

                if (MatchClauseSyntax* item = this->ParseMatchClause())
                {
                    items.push_back(item);
                }
                else
                {
                    break;
                }
            }

            result->Elements = SyntaxListView<MatchClauseSyntax>{this->_factory->CreateList(items)};
        }

        this->MatchUntil(result->RightBrace, result->BeforeRightBrace, SyntaxKind::CloseBraceToken);
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
        result->Pattern = this->ParseQualifiedName();
        result->ColonToken = this->Match(SyntaxKind::ColonToken);
        result->Body = this->ParseStatement({});
        result->TrailingSemicolon = this->Match(SyntaxKind::SemicolonToken);
        return result;
    }

    MatchDefaultClauseSyntax* Parser::ParseMatchDefaultClause()
    {
        MatchDefaultClauseSyntax* result = this->_factory->CreateNode<MatchDefaultClauseSyntax>();
        result->DefaultKeyword = this->Match(SyntaxKind::DefaultKeyword);
        result->ColonToken = this->Match(SyntaxKind::ColonToken);
        result->Body = this->ParseStatement({});
        result->TrailingSemicolon = this->Match(SyntaxKind::SemicolonToken);
        return result;
    }
}
