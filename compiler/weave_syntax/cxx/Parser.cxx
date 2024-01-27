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

    CompilationUnitSyntax* Parser::Parse()
    {
        return this->ParseCompilationUnit();
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
        SyntaxToken* const rightBrace = this->Match(SyntaxKind::CloseBraceToken);

        CodeBlockSyntax* result = this->_factory->CreateNode<CodeBlockSyntax>();
        result->LeftBrace = leftBrace;
        result->Elements = SyntaxListView<CodeBlockItemSyntax>{this->_factory->CreateList(items)};
        result->RightBrace = rightBrace;
        return result;
    }

    CodeBlockItemSyntax* Parser::ParseCodeBlockItem()
    {
        SyntaxNode* element = nullptr;

        SyntaxListView<AttributeListSyntax> attributes = this->ParseAttributesList();
        SyntaxListView<SyntaxToken> modifiers = this->ParseMemberModifiersList();

        UnexpectedNodesSyntax* unexpected{};

        // Lookahead for one of expected tokens.
        SyntaxToken* current = this->MatchUntil(unexpected, [](SyntaxKind kind)
            {
                return SyntaxFacts::IsStartOfDeclaration(kind) or
                    SyntaxFacts::IsStartOfStatement(kind) or
                    SyntaxFacts::IsStartOfExpression(kind) or
                    kind == SyntaxKind::CloseBraceToken or
                    kind == SyntaxKind::CloseBracketToken or
                    kind == SyntaxKind::CloseParenToken or
                    kind == SyntaxKind::SemicolonToken;
            });

        if (SyntaxFacts::IsStartOfDeclaration(current->Kind))
        {
            element = this->ParseDeclaration(attributes, modifiers, unexpected);
        }
        else if (SyntaxFacts::IsStartOfStatement(current->Kind))
        {
            element = this->ParseStatement(attributes, modifiers, unexpected);
        }
        else if (SyntaxFacts::IsStartOfExpression(current->Kind))
        {
            ExpressionStatementSyntax* result = this->_factory->CreateNode<ExpressionStatementSyntax>();
            result->Attributes = attributes;
            result->Modifiers = modifiers;
            result->Unexpected = unexpected;
            result->Expression = this->ParseExpression();
            element = result;
        }
        else if ((attributes.GetNode() != nullptr) or (modifiers.GetNode() != nullptr))
        {
            IncompleteDeclarationSyntax* result = this->_factory->CreateNode<IncompleteDeclarationSyntax>();
            result->Attributes = attributes;
            result->Modifiers = modifiers;
            result->Unexpected = unexpected;
            element = result;
        }

        SyntaxToken* semicolon = this->TryMatch(SyntaxKind::SemicolonToken);
        UnexpectedNodesSyntax* unexpectedSemicolons = this->ConsumeUnexpected(SyntaxKind::SemicolonToken);

        if ((element != nullptr) or (semicolon != nullptr) or (unexpectedSemicolons))
        {
            CodeBlockItemSyntax* result = this->_factory->CreateNode<CodeBlockItemSyntax>();
            result->Item = element;
            result->Semicolon = semicolon;
            result->AfterSemicolon = unexpectedSemicolons;
            return result;
        }

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

        while (SyntaxToken* current = this->Current())
        {
            if (not SyntaxFacts::IsMemberModifier(current->Kind))
            {
                break;
            }

            elements.push_back(this->Next());
        }

        return SyntaxListView<SyntaxToken>{this->_factory->CreateList(elements)};
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
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        SyntaxKind const expected = (this->Current()->Kind == SyntaxKind::VarKeyword)
            ? SyntaxKind::VarKeyword
            : SyntaxKind::LetKeyword;

        VariableDeclarationSyntax* result = this->_factory->CreateNode<VariableDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->VarKeyword = this->Match(expected);
        result->Identifier = this->ParseIdentifierName();
        result->TypeClause = this->ParseOptionalTypeClause();
        result->Initializer = this->ParseOptionalEqualsValueClause();
        return result;
    }

    FunctionDeclarationSyntax* Parser::ParseFunctionDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        FunctionDeclarationSyntax* result = this->_factory->CreateNode<FunctionDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->FunctionKeyword = this->Match(SyntaxKind::FunctionKeyword);
        result->Name = this->ParseIdentifierName();
        result->Parameters = this->ParseParameterList();

        if (this->Current()->Is(SyntaxKind::MinusGreaterThanToken))
        {
            result->ReturnType = this->ParseReturnTypeClause();
        }

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

    NamespaceDeclarationSyntax* Parser::ParseNamespaceDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        NamespaceDeclarationSyntax* result = this->_factory->CreateNode<NamespaceDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->NamespaceKeyword = this->Match(SyntaxKind::NamespaceKeyword);
        result->Name = this->ParseQualifiedName();
        result->Members = this->ParseCodeBlock();
        return result;
    }

    ConceptDeclarationSyntax* Parser::ParseConceptDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        SyntaxToken* tokenConcept = this->Match(SyntaxKind::ConceptKeyword);
        NameSyntax* name = this->ParseSimpleName();
        CodeBlockSyntax* members = this->ParseCodeBlock();

        std::vector<ConstraintSyntax*> constraints{};

        ConceptDeclarationSyntax* result = this->_factory->CreateNode<ConceptDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->ConceptKeyword = tokenConcept;
        result->Name = name;
        result->Members = members;
        return result;
    }

    ExtendDeclarationSyntax* Parser::ParseExtendDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        SyntaxToken* tokenExtend = this->Match(SyntaxKind::ExtendKeyword);
        NameSyntax* name = this->ParseSimpleName();
        CodeBlockSyntax* members = this->ParseCodeBlock();

        std::vector<ConstraintSyntax*> constraints{};

        ExtendDeclarationSyntax* result = this->_factory->CreateNode<ExtendDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->ExtendKeyword = tokenExtend;
        result->Name = name;
        result->Members = members;
        return result;
    }

    StructDeclarationSyntax* Parser::ParseStructDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        SyntaxToken* tokenStruct = this->Match(SyntaxKind::StructKeyword);
        NameSyntax* name = this->ParseSimpleName();
        CodeBlockSyntax* members = this->ParseCodeBlock();

        std::vector<ConstraintSyntax*> constraints{};

        StructDeclarationSyntax* result = this->_factory->CreateNode<StructDeclarationSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->StructKeyword = tokenStruct;
        result->Name = name;
        result->Members = members;
        return result;
    }

    DeclarationSyntax* Parser::ParseDeclaration(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        switch (this->Current()->Kind)
        {
        case SyntaxKind::FunctionKeyword:
            return this->ParseFunctionDeclaration(attributes, modifiers, unexpected);

        case SyntaxKind::VarKeyword:
        case SyntaxKind::LetKeyword:
            return this->ParseVariableDeclaration(attributes, modifiers, unexpected);

        case SyntaxKind::StructKeyword:
            return this->ParseStructDeclaration(attributes, modifiers, unexpected);

        case SyntaxKind::ConceptKeyword:
            return this->ParseConceptDeclaration(attributes, modifiers, unexpected);

        case SyntaxKind::ExtendKeyword:
            return this->ParseExtendDeclaration(attributes, modifiers, unexpected);

        case SyntaxKind::NamespaceKeyword:
            return this->ParseNamespaceDeclaration(attributes, modifiers, unexpected);

        default:
            break;
        }

        WEAVE_BUGCHECK("Declaration not handled");
    }

    StatementSyntax* Parser::ParseStatement(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        switch (this->Current()->Kind)
        {
        case SyntaxKind::ReturnKeyword:
            return this->ParseReturnStatement(attributes, modifiers, unexpected);

        case SyntaxKind::IfKeyword:
            return this->ParseIfStatement(attributes, modifiers, unexpected);

        case SyntaxKind::ElseKeyword:
            return this->ParseMisplacedElseClause(attributes, modifiers, unexpected);

        case SyntaxKind::OpenBraceToken:
            return this->ParseBlockStatement(attributes, modifiers, unexpected);

        case SyntaxKind::SemicolonToken:
            return this->ParseEmptyStatement(attributes, modifiers, unexpected);

        default:
            break;
        }

        WEAVE_BUGCHECK("Statement not handled");
    }

    BlockStatementSyntax* Parser::ParseBlockStatement(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        BlockStatementSyntax* result = this->_factory->CreateNode<BlockStatementSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->Members = this->ParseCodeBlock();
        return result;
    }

    EmptyStatementSyntax* Parser::ParseEmptyStatement(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        EmptyStatementSyntax* result = this->_factory->CreateNode<EmptyStatementSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
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

    // *** --------------------------------------------------------------------------------------------------------- ***
    CompilationUnitSyntax* Parser::ParseCompilationUnit()
    {
        std::vector<UsingDirectiveSyntax*> usings{};
        std::vector<MemberDeclarationSyntax*> members{};

        ParseNamespaceBody(nullptr, usings, members);
        SyntaxToken* tokenEndOfFile = this->Match(SyntaxKind::EndOfFileToken);

        // FIXME: Any tokens left after parsing compilation unit should be reported as well.

        CompilationUnitSyntax* result = this->_factory->CreateNode<CompilationUnitSyntax>();
        result->AttributeLists = {};
        result->Usings = SyntaxListView<UsingDirectiveSyntax>{this->_factory->CreateList(usings)};
        result->Members = SyntaxListView<MemberDeclarationSyntax>{this->_factory->CreateList(members)};
        result->EndOfFileToken = tokenEndOfFile;
        return result;
    }

    void Parser::ParseTypeBody(
        std::vector<ConstraintSyntax*>& constraints,
        std::vector<MemberDeclarationSyntax*>& members)
    {
        (void)constraints;
        std::vector<SyntaxToken*> modifiers{};
        std::vector<AttributeListSyntax*> attributes{};

        while (SyntaxToken* current = this->Current())
        {
            if (current->Is(SyntaxKind::CloseBraceToken))
            {
                break;
            }

            if (current->Is(SyntaxKind::EndOfFileToken))
            {
                break;
            }

            this->ParseAttributesList(attributes);

            this->ParseMemberModifiers(modifiers);

            auto* member = this->ParseMemberDeclaration(attributes, modifiers);

            if (member == nullptr)
            {
                break;
            }

            members.push_back(member);
        }
    }

    void Parser::ParseNamespaceBody(
        SyntaxToken* openBraceOrSemicolon,
        std::vector<UsingDirectiveSyntax*>& usings,
        std::vector<MemberDeclarationSyntax*>& members)
    {
        bool global = (openBraceOrSemicolon == nullptr);
        std::vector<SyntaxToken*> modifiers{};
        std::vector<AttributeListSyntax*> attributes{};

        while (SyntaxToken* current = this->Current())
        {
            if (current->Is(SyntaxKind::CloseBraceToken))
            {
                if (not global)
                {
                    return;
                }
            }
            else if (current->Is(SyntaxKind::EndOfFileToken))
            {
                break;
            }

            this->ParseAttributesList(attributes);

            this->ParseMemberModifiers(modifiers);

            if (current->Is(SyntaxKind::NamespaceKeyword))
            {
                members.push_back(this->ParseNamespaceDeclaration(attributes, modifiers));
            }
            else if (current->Is(SyntaxKind::UsingKeyword))
            {
                if (not members.empty())
                {
                    this->_diagnostic->AddError(
                        current->Source,
                        "using statement must precede all other elements defined in namespace");
                }

                usings.push_back(this->ParseUsingDirective());
            }
            else
            {
                auto* member = this->ParseMemberDeclaration(attributes, modifiers);
                members.push_back(member);
            }
        }
    }


    void Parser::ParseMemberModifiers(std::vector<SyntaxToken*>& modifiers)
    {
        modifiers.clear();

        while (SyntaxToken* current = this->Current())
        {
            if (not SyntaxFacts::IsMemberModifier(current->Kind))
            {
                break;
            }

            modifiers.push_back(this->Next());
        }
    }

    void Parser::ParseFunctionParameterModifiers(std::vector<SyntaxToken*>& modifiers)
    {
        modifiers.clear();

        while (SyntaxToken* current = this->Current())
        {
            if (not SyntaxFacts::IsFunctionParameterModifier(current->Kind))
            {
                break;
            }

            modifiers.push_back(this->Next());
        }
    }

    void Parser::ParseAttributesList(std::vector<AttributeListSyntax*>& attributes)
    {
        attributes.clear();

        while (AttributeListSyntax* current = this->ParseAttributeList())
        {
            attributes.push_back(current);
        }
    }

    MemberDeclarationSyntax* Parser::ParseMemberDeclaration(std::span<AttributeListSyntax*> attributes, std::span<SyntaxToken*> modifiers)
    {
        SyntaxToken* current = this->Current();

        switch (current->Kind) // NOLINT(clang-diagnostic-switch)
        {
        case SyntaxKind::NamespaceKeyword:
            return this->ParseNamespaceDeclaration(attributes, modifiers);

        case SyntaxKind::StructKeyword:
            return this->ParseStructDeclaration(attributes, modifiers);

        case SyntaxKind::ConceptKeyword:
            return this->ParseConceptDeclaration(attributes, modifiers);

        case SyntaxKind::ExtendKeyword:
            return this->ParseExtendDeclaration(attributes, modifiers);

        case SyntaxKind::FunctionKeyword:
            return this->ParseFunctionDeclaration(attributes, modifiers);

        case SyntaxKind::ConstKeyword:
            return this->ParseConstantDeclaration(attributes, modifiers);

        default:
            break;
        }

        // Member is incomplete. We don't know what it is, so report it as incomplete unknown member.
        // NOTE: We can do report this error with more details, but we don't have enough information right now.
        this->_diagnostic->AddError(
            current->Source,
            fmt::format(
                "unexpected token '{}', expected member declaration",
                GetSpelling(current->Kind)));

        // Fixme: do we need to parse this as identifier?
        // SyntaxNode* name = this->Next();

        // FIXME: Report this using function.
        // IncompleteDeclarationSyntax* result = this->_factory->CreateNode<IncompleteDeclarationSyntax>();
        // result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        // result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};

        return nullptr;
        // return result;
    }

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

    NamespaceDeclarationSyntax* Parser::ParseNamespaceDeclaration(
        std::span<AttributeListSyntax*> attributes,
        std::span<SyntaxToken*> modifiers)
    {
        SyntaxToken* tokenNamespace = this->Match(SyntaxKind::NamespaceKeyword);
        NameSyntax* name = this->ParseQualifiedName();

        NamespaceDeclarationSyntax* result = this->_factory->CreateNode<NamespaceDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->NamespaceKeyword = tokenNamespace;
        result->Name = name;
        return result;
    }

    StructDeclarationSyntax* Parser::ParseStructDeclaration(
        std::span<AttributeListSyntax*> attributes,
        std::span<SyntaxToken*> modifiers)
    {
        SyntaxToken* tokenStruct = this->Match(SyntaxKind::StructKeyword);
        NameSyntax* name = this->ParseSimpleName();

        std::vector<ConstraintSyntax*> constraints{};
        std::vector<MemberDeclarationSyntax*> members{};

        this->ParseTypeBody(constraints, members);

        StructDeclarationSyntax* result = this->_factory->CreateNode<StructDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->StructKeyword = tokenStruct;
        result->Name = name;
        // result->OpenBraceToken = tokenOpenBrace;
        // result->Members = SyntaxListView<MemberDeclarationSyntax>{this->_factory->CreateList(members)};
        // result->CloseBraceToken = tokenCloseBrace;
        // result->SemicolonToken = tokenSemicolon;
        return result;
    }

    ExtendDeclarationSyntax* Parser::ParseExtendDeclaration(
        std::span<AttributeListSyntax*> attributes,
        std::span<SyntaxToken*> modifiers)
    {
        SyntaxToken* tokenExtend = this->Match(SyntaxKind::ExtendKeyword);
        NameSyntax* name = this->ParseSimpleName();

        std::vector<ConstraintSyntax*> constraints{};

        ExtendDeclarationSyntax* result = this->_factory->CreateNode<ExtendDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->ExtendKeyword = tokenExtend;
        result->Name = name;
        return result;
    }

    ConceptDeclarationSyntax* Parser::ParseConceptDeclaration(
        std::span<AttributeListSyntax*> attributes,
        std::span<SyntaxToken*> modifiers)
    {
        SyntaxToken* tokenConcept = this->Match(SyntaxKind::ConceptKeyword);
        NameSyntax* name = this->ParseSimpleName();

        std::vector<ConstraintSyntax*> constraints{};

        ConceptDeclarationSyntax* result = this->_factory->CreateNode<ConceptDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->ConceptKeyword = tokenConcept;
        result->Name = name;
        return result;
    }

    UsingDirectiveSyntax* Parser::ParseUsingDirective()
    {
        SyntaxToken* tokenUsing = this->Match(SyntaxKind::UsingKeyword);
        NameSyntax* name = this->ParseQualifiedName();

        UsingDirectiveSyntax* result = this->_factory->CreateNode<UsingDirectiveSyntax>();
        result->UsingKeyword = tokenUsing;
        result->Name = name;
        return result;
    }

    ReturnTypeClauseSyntax* Parser::ParseReturnTypeClause()
    {
        ReturnTypeClauseSyntax* result = this->_factory->CreateNode<ReturnTypeClauseSyntax>();
        result->ArrowToken = this->Match(SyntaxKind::MinusGreaterThanToken);
        result->Type = this->ParseQualifiedName();
        return result;
    }

    ArrowExpressionClauseSyntax* Parser::ParseArrowExpressionClause()
    {
        ArrowExpressionClauseSyntax* result = this->_factory->CreateNode<ArrowExpressionClauseSyntax>();
        result->ArrowToken = this->Match(SyntaxKind::EqualsGreaterThanToken);
        result->Expression = this->ParseExpression();
        return result;
    }

    FunctionDeclarationSyntax* Parser::ParseFunctionDeclaration(std::span<AttributeListSyntax*> attributes, std::span<SyntaxToken*> modifiers)
    {
        FunctionDeclarationSyntax* result = this->_factory->CreateNode<FunctionDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->FunctionKeyword = this->Match(SyntaxKind::FunctionKeyword);
        result->Name = this->ParseIdentifierName();
        result->Parameters = this->ParseParameterList();

        if (this->Current()->Is(SyntaxKind::MinusGreaterThanToken))
        {
            result->ReturnType = this->ParseReturnTypeClause();
        }

        if (this->Current()->Is(SyntaxKind::OpenBraceToken))
        {
        }
        else if (this->Current()->Is(SyntaxKind::EqualsGreaterThanToken))
        {
            result->ExpressionBody = this->ParseArrowExpressionClause();
        }

        return result;
    }

    TypeClauseSyntax* Parser::ParseTypeClause()
    {
        SyntaxToken* tokenColon = this->Match(SyntaxKind::ColonToken);
        NameSyntax* type = this->ParseQualifiedName();

        TypeClauseSyntax* result = this->_factory->CreateNode<TypeClauseSyntax>();
        result->ColonToken = tokenColon;
        result->Identifier = type;
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

    // ParameterSyntax* Parser::ParseParameter(std::span<AttributeListSyntax*> attributes, std::span<SyntaxToken*> modifiers)
    //{
    //     if (this->Current()->Is(SyntaxKind::IdentifierToken) or not attributes.empty() or not modifiers.empty())
    //     {
    //         NameSyntax* name = this->ParseIdentifierName();
    //         TypeClauseSyntax* typeClause = this->ParseTypeClause();
    //         SyntaxToken* trailingComma = this->TryMatch(SyntaxKind::CommaToken);
    //
    //         ParameterSyntax* result = this->_factory->CreateNode<ParameterSyntax>();
    //         result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
    //         result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
    //         result->Identifier = name;
    //         result->Type = typeClause;
    //         result->TrailingComma = trailingComma;
    //         return result;
    //     }
    //
    //     return nullptr;
    // }

    // ParameterListSyntax* Parser::ParseParameterList()
    //{
    //     SyntaxToken* tokenOpenParen = this->Match(SyntaxKind::OpenParenToken);
    //     std::vector<AttributeListSyntax*> attributes{};
    //     std::vector<SyntaxNode*> nodes{};
    //     std::vector<SyntaxToken*> modifiers{};

    //    while ((this->Current()->Kind != SyntaxKind::CloseParenToken) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
    //    {
    //        this->ParseAttributesList(attributes);
    //        this->ParseFunctionParameterModifiers(modifiers);

    //        if (ParameterSyntax* parameter = this->ParseParameter(attributes, modifiers))
    //        {
    //            nodes.push_back(parameter);
    //        }
    //        else
    //        {
    //            break;
    //        }
    //    }

    //    // NOTE:    Trailing token validation could be implemented using syntax tree walker with
    //    //          matcher testing trailing token for last node of syntax list.

    //    std::vector<SyntaxNode*> unexpected{};
    //    SyntaxToken* tokenCloseParen = this->MatchUntil(unexpected, SyntaxKind::CloseParenToken);

    //    ParameterListSyntax* result = this->_factory->CreateNode<ParameterListSyntax>();
    //    result->OpenParenToken = tokenOpenParen;
    //    result->Parameters = SyntaxListView<ParameterSyntax>{this->_factory->CreateList(nodes)};
    //    result->BeforeParenToken = this->CreateUnexpectedNodes(unexpected);
    //    result->CloseParenToken = tokenCloseParen;
    //    return result;
    //}

    ConstantDeclarationSyntax* Parser::ParseConstantDeclaration(std::span<AttributeListSyntax*> attributes, std::span<SyntaxToken*> modifiers)
    {
        SyntaxToken* tokenConst = this->Match(SyntaxKind::ConstKeyword);
        NameSyntax* name = this->ParseIdentifierName();
        TypeClauseSyntax* typeClause = this->ParseOptionalTypeClause();
        EqualsValueClauseSyntax* initializer = this->ParseOptionalEqualsValueClause();

        ConstantDeclarationSyntax* result = this->_factory->CreateNode<ConstantDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->ConstKeyword = tokenConst;
        result->Name = name;
        result->Type = typeClause;
        result->Initializer = initializer;
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
        return this->ParseIdentifierName();
    }

    IdentifierNameSyntax* Parser::ParseIdentifierName()
    {
        SyntaxToken* identifier = this->Match(SyntaxKind::IdentifierToken);
        IdentifierNameSyntax* result = this->_factory->CreateNode<IdentifierNameSyntax>();
        result->Identifier = identifier;
        return result;
    }

    SelfExpressionSyntax* Parser::ParseSelfExpression()
    {
        SyntaxToken* tokenSelf = this->Match(SyntaxKind::SelfKeyword);
        SelfExpressionSyntax* result = this->_factory->CreateNode<SelfExpressionSyntax>();
        result->SelfKeyword = tokenSelf;
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
                    result->Name = this->ParseIdentifierName();
                    expression = result;
                    continue;
                }

            case SyntaxKind::PlusPlusToken:
            case SyntaxKind::MinusMinusToken:
                {
                    PostfixUnaryExpression* result = this->_factory->CreateNode<PostfixUnaryExpression>();
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
            return this->ParseParenthesizedExpression();

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
            return this->ParseIdentifierName();

        case SyntaxKind::SelfKeyword:
            return this->ParseSelfExpression();

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

    ExpressionSyntax* Parser::ParseParenthesizedExpression()
    {
        std::vector<SyntaxNode*> unexpected{};

        SyntaxToken* tokenOpenParen = this->Match(SyntaxKind::OpenParenToken);
        ExpressionSyntax* expression = this->ParseExpression();

        SyntaxToken* tokenCloseParen = this->MatchUntil(unexpected, SyntaxKind::CloseParenToken);
        UnexpectedNodesSyntax* unexpectedNodesBetweenExpressionAndCloseParen = this->CreateUnexpectedNodes(unexpected);

        ParenthesizedExpressionSyntax* result = this->_factory->CreateNode<ParenthesizedExpressionSyntax>();
        result->OpenParenToken = tokenOpenParen;
        result->Expression = expression;
        result->BetweenExpressionAndCloseParen = unexpectedNodesBetweenExpressionAndCloseParen;
        result->CloseParenToken = tokenCloseParen;
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

    StatementSyntax* Parser::ParseIfStatement(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        SyntaxToken* tokenIf = this->Match(SyntaxKind::IfKeyword);
        ExpressionSyntax* condition = this->ParseExpression();
        StatementSyntax* thenStatement = this->ParseBlockStatement({}, {}, {});
        ElseClauseSyntax* elseClause = this->ParseOptionalElseClause();

        IfStatementSyntax* result = this->_factory->CreateNode<IfStatementSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->IfKeyword = tokenIf;
        result->Condition = condition;
        result->ThenStatement = thenStatement;
        result->ElseClause = elseClause;
        return result;
    }

    StatementSyntax* Parser::ParseMisplacedElseClause(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        WEAVE_ASSERT(this->Current()->Kind == SyntaxKind::ElseKeyword);

        SyntaxToken* tokenIf = this->Match(SyntaxKind::IfKeyword);
        ExpressionSyntax* condition = this->ParseExpression();
        StatementSyntax* thenStatement = this->ParseExpressionStatement();
        ElseClauseSyntax* elseClause = this->ParseOptionalElseClause();

        IfStatementSyntax* result = this->_factory->CreateNode<IfStatementSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->IfKeyword = tokenIf;
        result->Condition = condition;
        result->ThenStatement = thenStatement;
        result->ElseClause = elseClause;
        return result;
    }

    ElseClauseSyntax* Parser::ParseOptionalElseClause()
    {
        if (this->Current()->Is(SyntaxKind::ElseKeyword))
        {
            SyntaxToken* tokenElse = this->Next();

            StatementSyntax* elseStatement =
                this->Current()->Is(SyntaxKind::IfKeyword)
                ? this->ParseIfStatement({}, {}, {})
                : this->ParseBlockStatement({}, {}, {});

            ElseClauseSyntax* result = this->_factory->CreateNode<ElseClauseSyntax>();
            result->ElseKeyword = tokenElse;
            result->Statement = elseStatement;
            return result;
        }

        return nullptr;
    }

    StatementSyntax* Parser::ParseReturnStatement(
        SyntaxListView<AttributeListSyntax> attributes,
        SyntaxListView<SyntaxToken> modifiers,
        UnexpectedNodesSyntax* unexpected)
    {
        SyntaxToken* tokenReturn = this->Match(SyntaxKind::ReturnKeyword);
        ExpressionSyntax* expression = nullptr;

        if (not this->Current()->Is(SyntaxKind::SemicolonToken))
        {
            expression = this->ParseExpression();
        }

        ReturnStatementSyntax* result = this->_factory->CreateNode<ReturnStatementSyntax>();
        result->Attributes = attributes;
        result->Modifiers = modifiers;
        result->Unexpected = unexpected;
        result->ReturnKeyword = tokenReturn;
        result->Expression = expression;
        return result;
    }

    ExpressionStatementSyntax* Parser::ParseExpressionStatement()
    {
        if (ExpressionSyntax* expression = this->ParseExpression())
        {
            ExpressionStatementSyntax* result = this->_factory->CreateNode<ExpressionStatementSyntax>();
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

    SyntaxToken* Parser::MatchBalancedTokenSequence(SyntaxKind terminator, std::vector<SyntaxToken*>& tokens, std::vector<SyntaxNode*>& unexpected)
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
}
