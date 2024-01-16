#include "weave/syntax/Parser.hxx"

#include "weave/bugcheck/BugCheck.hxx"
#include "weave/syntax/SyntaxTree.hxx"
#include "weave/syntax/SyntaxFacts.hxx"
#include "weave/syntax/Lexer.hxx"

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

        while (SyntaxToken const* token = lexer.Lex(*factory))
        {
            this->_tokens.push_back(token);

            if (token->Is(SyntaxKind::EndOfFileToken) or token->Is(SyntaxKind::None))
            {
                break;
            }
        }
    }

    CompilationUnitSyntax const* Parser::Parse()
    {
        return this->ParseCompilationUnit();
    }

    SyntaxToken const* Parser::Peek(size_t offset) const
    {
        size_t const index = this->_index + offset;

        if (index >= this->_tokens.size())
        {
            return this->_tokens.back();
        }

        return this->_tokens[index];
    }

    SyntaxToken const* Parser::Current() const
    {
        return this->Peek(0);
    }

    SyntaxToken const* Parser::Next()
    {
        SyntaxToken const* current = this->Current();
        ++this->_index;
        return current;
    }

    SyntaxToken const* Parser::Match(SyntaxKind kind)
    {
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        this->_diagnostic->AddError(
            this->Current()->Source,
            fmt::format("unexpected token '{}', expected '{}'",
                SyntaxKindTraits::GetSpelling(this->Current()->Kind),
                SyntaxKindTraits::GetSpelling(kind)));

        // return this->_factory->CreateMissingToken(kind, this->Current()->Source);
        return this->SkipToken(kind);
    }

    SyntaxToken const* Parser::MatchOptional(SyntaxKind kind)
    {
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        // return this->_factory->CreateMissingToken(kind, this->Current()->Source);
        return this->SkipToken(kind);
    }

    SyntaxToken const* Parser::TryMatch(SyntaxKind kind)
    {
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        return nullptr;
    }

    SyntaxToken const* Parser::SkipToken(SyntaxKind kind, bool consume)
    {
        std::vector<SyntaxTrivia> leadingTrivia{};

        if (consume)
        {
            SyntaxToken const* bad = this->Next();

            // Copy leading trivia
            for (SyntaxTrivia const& trivia : bad->GetLeadingTrivia())
            {
                leadingTrivia.push_back(trivia);
            }

            // Push skipped token as trivia
            leadingTrivia.push_back(SyntaxTrivia{
                SyntaxKind::SkippedTokenTrivia,
                bad->Source,
            });

            // Copy trailing trivia
            for (SyntaxTrivia const& trivia : bad->GetTrailingTrivia())
            {
                leadingTrivia.push_back(trivia);
            }
        }

        source::SourceSpan source = this->Current()->Source;
        source.End = source.Start;
        return this->_factory->CreateMissingToken(kind, source, leadingTrivia, {});
    }

    CompilationUnitSyntax const* Parser::ParseCompilationUnit()
    {
        std::vector<UsingDirectiveSyntax const*> usings{};
        std::vector<MemberDeclarationSyntax const*> members{};

        ParseNamespaceBody(nullptr, usings, members);
        SyntaxToken const* tokenEndOfFile = this->Match(SyntaxKind::EndOfFileToken);

        // FIXME: Any tokens left after parsing compilation unit should be reported as well.

        CompilationUnitSyntax* result = this->_factory->CreateNode<CompilationUnitSyntax>();
        result->AttributeLists = {};
        result->Usings = SyntaxListView<UsingDirectiveSyntax>{this->_factory->CreateList(usings)};
        result->Members = SyntaxListView<MemberDeclarationSyntax>{this->_factory->CreateList(members)};
        result->EndOfFileToken = tokenEndOfFile;
        return result;
    }

    void Parser::ParseTypeBody(
        std::vector<ConstraintSyntax const*>& constraints,
        std::vector<MemberDeclarationSyntax const*>& members)
    {
        (void)constraints;
        std::vector<SyntaxToken const*> modifiers{};
        std::vector<AttributeListSyntax const*> attributes{};

        while (SyntaxToken const* current = this->Current())
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
            members.push_back(member);
        }
    }

    void Parser::ParseNamespaceBody(
        SyntaxToken const* openBraceOrSemicolon,
        std::vector<UsingDirectiveSyntax const*>& usings,
        std::vector<MemberDeclarationSyntax const*>& members)
    {
        bool global = (openBraceOrSemicolon == nullptr);
        std::vector<SyntaxToken const*> modifiers{};
        std::vector<AttributeListSyntax const*> attributes{};

        while (SyntaxToken const* current = this->Current())
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


    void Parser::ParseMemberModifiers(std::vector<SyntaxToken const*>& modifiers)
    {
        modifiers.clear();

        while (SyntaxToken const* current = this->Current())
        {
            if (not SyntaxFacts::IsMemberModifier(current->Kind))
            {
                break;
            }

            modifiers.push_back(this->Next());
        }
    }

    void Parser::ParseFunctionParameterModifiers(std::vector<SyntaxToken const*>& modifiers)
    {
        modifiers.clear();

        while (SyntaxToken const* current = this->Current())
        {
            if (not SyntaxFacts::IsFunctionParameterModifier(current->Kind))
            {
                break;
            }

            modifiers.push_back(this->Next());
        }
    }

    void Parser::ParseAttributesList(std::vector<AttributeListSyntax const*>& attributes)
    {
        attributes.clear();

        while (AttributeListSyntax const* current = this->ParseAttributeList())
        {
            attributes.push_back(current);
        }
    }

    MemberDeclarationSyntax const* Parser::ParseMemberDeclaration(std::span<AttributeListSyntax const*> attributes, std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* current = this->Current();

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

        case SyntaxKind::VarKeyword:
            return this->ParseFieldDeclaration(attributes, modifiers);

        case SyntaxKind::ConstKeyword:
            return this->ParseConstantDeclaration(attributes, modifiers);

        default:
            break;
        }

        // Member is incomplete.
        SyntaxNode const* name = nullptr;
        if (current->Is(SyntaxKind::IdentifierToken))
        {
            name = this->ParseIdentifierName();
        }
        else
        {
            name = this->Next();
        }

        // FIXME: Report this using function.
        IncompleteDeclarationSyntax* result = this->_factory->CreateNode<IncompleteDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->Type = name;
        return result;
    }

    AttributeListSyntax const* Parser::ParseAttributeList()
    {
        /*while (SyntaxToken const* current = this->Current())
        {
            if (current->Is(SyntaxKind::CloseBracketToken))
            {
                break;
            }
        }*/

        return nullptr;
    }

    NamespaceDeclarationSyntax const* Parser::ParseNamespaceDeclaration(
        std::span<AttributeListSyntax const*> attributes,
        std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* tokenNamespace = this->Match(SyntaxKind::NamespaceKeyword);
        NameSyntax const* name = this->ParseQualifiedName();
        SyntaxToken const* tokenOpenBrace = this->Match(SyntaxKind::OpenBraceToken);

        std::vector<UsingDirectiveSyntax const*> usings{};
        std::vector<MemberDeclarationSyntax const*> members{};

        ParseNamespaceBody(tokenOpenBrace, usings, members);

        SyntaxToken const* tokenCloseBrace = this->Match(SyntaxKind::CloseBraceToken);
        SyntaxToken const* tokenSemicolon = this->TryMatch(SyntaxKind::SemicolonToken);

        NamespaceDeclarationSyntax* result = this->_factory->CreateNode<NamespaceDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->NamespaceKeyword = tokenNamespace;
        result->Name = name;
        result->OpenBraceToken = tokenOpenBrace;
        result->Usings = SyntaxListView<UsingDirectiveSyntax>{this->_factory->CreateList(usings)};
        result->Members = SyntaxListView<MemberDeclarationSyntax>{this->_factory->CreateList(members)};
        result->CloseBraceToken = tokenCloseBrace;
        result->SemicolonToken = tokenSemicolon;
        return result;
    }

    StructDeclarationSyntax const* Parser::ParseStructDeclaration(
        std::span<AttributeListSyntax const*> attributes,
        std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* tokenStruct = this->Match(SyntaxKind::StructKeyword);
        NameSyntax const* name = this->ParseSimpleName();
        SyntaxToken const* tokenOpenBrace = this->Match(SyntaxKind::OpenBraceToken);

        std::vector<ConstraintSyntax const*> constraints{};
        std::vector<MemberDeclarationSyntax const*> members{};

        this->ParseTypeBody(constraints, members);

        SyntaxToken const* tokenCloseBrace = this->Match(SyntaxKind::CloseBraceToken);
        SyntaxToken const* tokenSemicolon = this->TryMatch(SyntaxKind::SemicolonToken);

        StructDeclarationSyntax* result = this->_factory->CreateNode<StructDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->StructKeyword = tokenStruct;
        result->Name = name;
        result->OpenBraceToken = tokenOpenBrace;
        result->Members = SyntaxListView<MemberDeclarationSyntax>{this->_factory->CreateList(members)};
        result->CloseBraceToken = tokenCloseBrace;
        result->SemicolonToken = tokenSemicolon;
        return result;
    }

    ExtendDeclarationSyntax const* Parser::ParseExtendDeclaration(
        std::span<AttributeListSyntax const*> attributes,
        std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* tokenExtend = this->Match(SyntaxKind::ExtendKeyword);
        NameSyntax const* name = this->ParseSimpleName();
        SyntaxToken const* tokenOpenBrace = this->Match(SyntaxKind::OpenBraceToken);

        std::vector<ConstraintSyntax const*> constraints{};
        std::vector<MemberDeclarationSyntax const*> members{};

        this->ParseTypeBody(constraints, members);

        SyntaxToken const* tokenCloseBrace = this->Match(SyntaxKind::CloseBraceToken);
        SyntaxToken const* tokenSemicolon = this->TryMatch(SyntaxKind::SemicolonToken);

        ExtendDeclarationSyntax* result = this->_factory->CreateNode<ExtendDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->ExtendKeyword = tokenExtend;
        result->Name = name;
        result->OpenBraceToken = tokenOpenBrace;
        result->Members = SyntaxListView<MemberDeclarationSyntax>{this->_factory->CreateList(members)};
        result->CloseBraceToken = tokenCloseBrace;
        result->SemicolonToken = tokenSemicolon;
        return result;
    }

    ConceptDeclarationSyntax const* Parser::ParseConceptDeclaration(
        std::span<AttributeListSyntax const*> attributes,
        std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* tokenConcept = this->Match(SyntaxKind::ConceptKeyword);
        NameSyntax const* name = this->ParseSimpleName();
        SyntaxToken const* tokenOpenBrace = this->Match(SyntaxKind::OpenBraceToken);

        std::vector<ConstraintSyntax const*> constraints{};
        std::vector<MemberDeclarationSyntax const*> members{};

        this->ParseTypeBody(constraints, members);

        SyntaxToken const* tokenCloseBrace = this->Match(SyntaxKind::CloseBraceToken);
        SyntaxToken const* tokenSemicolon = this->TryMatch(SyntaxKind::SemicolonToken);

        ConceptDeclarationSyntax* result = this->_factory->CreateNode<ConceptDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->ConceptKeyword = tokenConcept;
        result->Name = name;
        result->OpenBraceToken = tokenOpenBrace;
        result->Members = SyntaxListView<MemberDeclarationSyntax>{this->_factory->CreateList(members)};
        result->CloseBraceToken = tokenCloseBrace;
        result->SemicolonToken = tokenSemicolon;
        return result;
    }

    UsingDirectiveSyntax const* Parser::ParseUsingDirective()
    {
        SyntaxToken const* tokenUsing = this->Match(SyntaxKind::UsingKeyword);
        NameSyntax const* name = this->ParseQualifiedName();
        SyntaxToken const* tokenSemicolon = this->Match(SyntaxKind::SemicolonToken);

        UsingDirectiveSyntax* result = this->_factory->CreateNode<UsingDirectiveSyntax>();
        result->UsingKeyword = tokenUsing;
        result->Name = name;
        result->SemicolonToken = tokenSemicolon;
        return result;
    }

    FunctionDeclarationSyntax const* Parser::ParseFunctionDeclaration(std::span<AttributeListSyntax const*> attributes, std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* tokenFunction = this->Match(SyntaxKind::FunctionKeyword);
        NameSyntax const* name = this->ParseIdentifierName();
        ParameterListSyntax const* parameters = this->ParseParameterList();
        SyntaxToken const* tokenArrow = this->TryMatch(SyntaxKind::MinusGreaterThanToken);
        NameSyntax const* returnType = nullptr;

        if (tokenArrow != nullptr)
        {
            returnType = this->ParseQualifiedName();
        }

        [[maybe_unused]] SyntaxToken const* tokenOpenBrace = this->Match(SyntaxKind::OpenBraceToken);
        [[maybe_unused]] SyntaxToken const* tokenCloseBrace = this->Match(SyntaxKind::CloseBraceToken);

        FunctionDeclarationSyntax* result = this->_factory->CreateNode<FunctionDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->FunctionKeyword = tokenFunction;
        result->Name = name;
        result->Parameters = parameters;
        result->ArrowToken = tokenArrow;
        result->ReturnType = returnType;
        result->OpenBraceToken = tokenOpenBrace;
        result->CloseBraceToken = tokenCloseBrace;
        return result;
    }

    TypeClauseSyntax const* Parser::ParseTypeClause()
    {
        SyntaxToken const* tokenColon = this->Match(SyntaxKind::ColonToken);
        NameSyntax const* type = this->ParseQualifiedName();

        TypeClauseSyntax* result = this->_factory->CreateNode<TypeClauseSyntax>();
        result->ColonToken = tokenColon;
        result->Identifier = type;
        return result;
    }

    TypeClauseSyntax const* Parser::ParseOptionalTypeClause()
    {
        if (this->Current()->Is(SyntaxKind::ColonToken))
        {
            return this->ParseTypeClause();
        }

        return nullptr;
    }

    EqualsValueClauseSyntax const* Parser::ParseEqualsValueClause()
    {
        SyntaxToken const* tokenEquals = this->Match(SyntaxKind::EqualsToken);
        ExpressionSyntax const* expression = this->ParseExpression();

        EqualsValueClauseSyntax* result = this->_factory->CreateNode<EqualsValueClauseSyntax>();
        result->EqualsToken = tokenEquals;
        result->Expression = expression;
        return result;
    }

    EqualsValueClauseSyntax const* Parser::ParseOptionalEqualsValueClause()
    {
        if (this->Current()->Is(SyntaxKind::EqualsToken))
        {
            return this->ParseEqualsValueClause();
        }

        return nullptr;
    }

    ArgumentSyntax const* Parser::ParseArgument()
    {
        SyntaxToken const* tokenDirectionKind = nullptr;

        if (SyntaxFacts::IsFunctionArgumentDirectionKind(this->Current()->Kind))
        {
            tokenDirectionKind = this->Next();
        }

        ExpressionSyntax const* expression = this->ParseExpression();

        ArgumentSyntax* result = this->_factory->CreateNode<ArgumentSyntax>();
        result->DirectionKindKeyword = tokenDirectionKind;
        result->Expression = expression;
        return result;
    }

    ArgumentListSyntax const* Parser::ParseArgumentList()
    {
        SyntaxToken const* tokenOpenParen = this->Match(SyntaxKind::OpenParenToken);

        std::vector<SyntaxNode const*> nodes{};

        while ((this->Current()->Kind != SyntaxKind::CloseParenToken) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
        {
            ArgumentSyntax const* argument = this->ParseArgument();
            nodes.push_back(argument);

            if (SyntaxToken const* comma = this->TryMatch(SyntaxKind::CommaToken))
            {
                nodes.push_back(comma);
            }
            else
            {
                break;
            }
        }

        SyntaxToken const* tokenCloseParen = this->Match(SyntaxKind::CloseParenToken);

        ArgumentListSyntax* result = this->_factory->CreateNode<ArgumentListSyntax>();
        result->OpenParenToken = tokenOpenParen;
        result->Arguments = SeparatedSyntaxListView<ArgumentSyntax>{this->_factory->CreateList(nodes)};
        result->CloseParenToken = tokenCloseParen;
        return result;
    }

    BracketedArgumentListSyntax const* Parser::ParseBracketedArgumentList()
    {
        SyntaxToken const* tokenOpenBracket = this->Match(SyntaxKind::OpenBracketToken);

        std::vector<SyntaxNode const*> nodes{};

        while ((this->Current()->Kind != SyntaxKind::CloseBracketToken) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
        {
            ArgumentSyntax const* argument = this->ParseArgument();
            nodes.push_back(argument);

            if (SyntaxToken const* comma = this->TryMatch(SyntaxKind::CommaToken))
            {
                nodes.push_back(comma);
            }
            else
            {
                break;
            }
        }

        SyntaxToken const* tokenCloseBracket = this->Match(SyntaxKind::CloseBracketToken);

        BracketedArgumentListSyntax* result = this->_factory->CreateNode<BracketedArgumentListSyntax>();
        result->OpenBracketToken = tokenOpenBracket;
        result->Arguments = SeparatedSyntaxListView<ArgumentSyntax>{this->_factory->CreateList(nodes)};
        result->CloseBracketToken = tokenCloseBracket;
        return result;
    }

    ParameterSyntax const* Parser::ParseParameter(std::span<AttributeListSyntax const*> attributes, std::span<SyntaxToken const*> modifiers)
    {
        NameSyntax const* name = this->ParseIdentifierName();
        TypeClauseSyntax const* typeClause = this->ParseTypeClause();

        ParameterSyntax* result = this->_factory->CreateNode<ParameterSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->Identifier = name;
        result->Type = typeClause;
        return result;
    }

    ParameterListSyntax const* Parser::ParseParameterList()
    {
        SyntaxToken const* tokenOpenParen = this->Match(SyntaxKind::OpenParenToken);

        std::vector<SyntaxNode const*> nodes{};
        std::vector<SyntaxToken const*> modifiers{};

        std::vector<AttributeListSyntax const*> attributes{};

        while ((this->Current()->Kind != SyntaxKind::CloseParenToken) and (this->Current()->Kind != SyntaxKind::EndOfFileToken))
        {
            this->ParseAttributesList(attributes);

            this->ParseFunctionParameterModifiers(modifiers);

            ParameterSyntax const* parameter = this->ParseParameter(attributes, modifiers);
            nodes.push_back(parameter);

            if (SyntaxToken const* comma = this->TryMatch(SyntaxKind::CommaToken))
            {
                nodes.push_back(comma);
            }
            else
            {
                break;
            }
        }

        SyntaxToken const* tokenCloseParen = this->Match(SyntaxKind::CloseParenToken);

        ParameterListSyntax* result = this->_factory->CreateNode<ParameterListSyntax>();
        result->OpenParenToken = tokenOpenParen;
        result->Parameters = SeparatedSyntaxListView<ParameterSyntax>{this->_factory->CreateList(nodes)};
        result->CloseParenToken = tokenCloseParen;
        return result;
    }

    FieldDeclarationSyntax const* Parser::ParseFieldDeclaration(
        std::span<AttributeListSyntax const*> attributes,
        std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* tokenVar = this->Match(SyntaxKind::VarKeyword);
        NameSyntax const* name = this->ParseIdentifierName();
        TypeClauseSyntax const* typeClause = this->ParseOptionalTypeClause();
        EqualsValueClauseSyntax const* initializer = this->ParseOptionalEqualsValueClause();
        SyntaxToken const* tokenSemicolon = this->Match(SyntaxKind::SemicolonToken);

        FieldDeclarationSyntax* result = this->_factory->CreateNode<FieldDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->VarKeyword = tokenVar;
        result->Name = name;
        result->Type = typeClause;
        result->Initializer = initializer;
        result->SemicolonToken = tokenSemicolon;
        return result;
    }

    ConstantDeclarationSyntax const* Parser::ParseConstantDeclaration(std::span<AttributeListSyntax const*> attributes, std::span<SyntaxToken const*> modifiers)
    {
        SyntaxToken const* tokenConst = this->Match(SyntaxKind::ConstKeyword);
        NameSyntax const* name = this->ParseIdentifierName();
        TypeClauseSyntax const* typeClause = this->ParseOptionalTypeClause();
        EqualsValueClauseSyntax const* initializer = this->ParseOptionalEqualsValueClause();
        SyntaxToken const* tokenSemicolon = this->Match(SyntaxKind::SemicolonToken);

        ConstantDeclarationSyntax* result = this->_factory->CreateNode<ConstantDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->ConstKeyword = tokenConst;
        result->Name = name;
        result->Type = typeClause;
        result->Initializer = initializer;
        result->SemicolonToken = tokenSemicolon;
        return result;
    }

    NameSyntax const* Parser::ParseQualifiedName()
    {
        NameSyntax const* left = this->ParseSimpleName();

        do
        {
            SyntaxToken const* tokenDot = this->Peek(0);
            SyntaxToken const* tokenRight = this->Peek(1);

            if ((tokenDot->Kind == SyntaxKind::DotToken) and (tokenRight->Kind == SyntaxKind::IdentifierToken))
            {
                // Consume dot token.
                this->Next();

                SimpleNameSyntax const* right = this->ParseSimpleName();
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

    SimpleNameSyntax const* Parser::ParseSimpleName()
    {
        return this->ParseIdentifierName();
    }

    IdentifierNameSyntax const* Parser::ParseIdentifierName()
    {
        SyntaxToken const* identifier = this->Match(SyntaxKind::IdentifierToken);
        IdentifierNameSyntax* result = this->_factory->CreateNode<IdentifierNameSyntax>();
        result->Identifier = identifier;
        return result;
    }

    ExpressionSyntax const* Parser::ParseExpression()
    {
        return this->ParseAssignmentExpression();
    }

    ExpressionSyntax const* Parser::ParseAssignmentExpression()
    {
        if (this->Current()->Is(SyntaxKind::IdentifierToken))
        {
            switch (this->Peek(1)->Kind) // NOLINT(clang-diagnostic-switch-enum)
            {
            case SyntaxKind::AmpersandEqualsToken:
            case SyntaxKind::AsteriskEqualsToken:
            case SyntaxKind::BarEqualsToken:
            case SyntaxKind::CaretEqualsToken:
            case SyntaxKind::EqualsToken:
            case SyntaxKind::GreaterThanGreaterThanEqualsToken:
            case SyntaxKind::LessThanLessThanEqualsToken:
            case SyntaxKind::MinusEqualsToken:
            case SyntaxKind::PercentEqualsToken:
            case SyntaxKind::PlusEqualsToken:
            case SyntaxKind::SlashEqualsToken:
                {
                    NameSyntax const* name = this->ParseIdentifierName();
                    SyntaxToken const* tokenOperator = this->Next();
                    ExpressionSyntax const* right = this->ParseAssignmentExpression();

                    AssignmentExpressionSyntax* result = this->_factory->CreateNode<AssignmentExpressionSyntax>();
                    result->Identifier = name;
                    result->OperatorToken = tokenOperator;
                    result->Expression = right;
                    return result;
                }

            default:
                break;
            }
        }

        return this->ParseBinaryExpression(Precedence::Expression);
    }

    ExpressionSyntax const* Parser::ParseBinaryExpression(Precedence parentPrecedence)
    {
        // TODO: Handle postfix unary expressions.
        ExpressionSyntax const* result = nullptr;

        SyntaxKind const unaryOperation = SyntaxFacts::GetPrefixUnaryExpression(this->Current()->Kind);

        if (unaryOperation != SyntaxKind::None)
        {
            Precedence const unaryPrecedence = SyntaxFacts::GetPrecedence(unaryOperation);

            if ((unaryPrecedence != Precedence::Expression) && (unaryPrecedence >= parentPrecedence))
            {
                SyntaxToken const* operatorToken = this->Next();
                ExpressionSyntax const* operand = this->ParseBinaryExpression(unaryPrecedence);

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

            SyntaxToken const* operatorToken = this->Next();
            ExpressionSyntax const* right = this->ParseBinaryExpression(binaryPrecedence);

            BinaryExpressionSyntax* left = this->_factory->CreateNode<BinaryExpressionSyntax>();
            left->Operation = binaryOperation;
            left->Left = result;
            left->OperatorToken = operatorToken;
            left->Right = right;
            result = left;
        }

        return result;
    }

    ExpressionSyntax const* Parser::ParseTerm(Precedence precedence)
    {
        return this->ParsePostfixExpression(
            this->ParseTermWithoutPostfix(precedence));
    }

    ExpressionSyntax const* Parser::ParsePostfixExpression(ExpressionSyntax const* expression)
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

    ExpressionSyntax const* Parser::ParseTermWithoutPostfix(
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

        case SyntaxKind::IdentifierToken:
            return this->ParseIdentifierName();

        default:
            // FIXME: Should be reported as missing identifier expression.
            WEAVE_BUGCHECK("Unhandled primary expression");
        }
    }

    ExpressionSyntax const* Parser::ParseParenthesizedExpression()
    {
        SyntaxToken const* tokenOpenParen = this->Match(SyntaxKind::OpenParenToken);
        ExpressionSyntax const* expression = this->ParseExpression();
        SyntaxToken const* tokenCloseParen = this->Match(SyntaxKind::CloseParenToken);

        ParenthesizedExpressionSyntax* result = this->_factory->CreateNode<ParenthesizedExpressionSyntax>();
        result->OpenParenToken = tokenOpenParen;
        result->Expression = expression;
        result->CloseParenToken = tokenCloseParen;
        return result;
    }

    ExpressionSyntax const* Parser::ParseBooleanLiteral()
    {
        bool const isTrue = this->Current()->Is(SyntaxKind::TrueKeyword);

        SyntaxToken const* literalToken = isTrue ? this->Match(SyntaxKind::TrueKeyword) : this->Match(SyntaxKind::FalseKeyword);

        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    ExpressionSyntax const* Parser::ParseIntegerLiteral()
    {
        SyntaxToken const* literalToken = this->Match(SyntaxKind::IntegerLiteralToken);
        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    ExpressionSyntax const* Parser::ParseFloatLiteral()
    {
        SyntaxToken const* literalToken = this->Match(SyntaxKind::FloatLiteralToken);
        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    ExpressionSyntax const* Parser::ParseStringLiteral()
    {
        SyntaxToken const* literalToken = this->Match(SyntaxKind::StringLiteralToken);
        LiteralExpressionSyntax* result = this->_factory->CreateNode<LiteralExpressionSyntax>();
        result->LiteralToken = literalToken;
        return result;
    }

    void Parser::ReportIncompleteMember(
        std::span<SyntaxToken const*> modifiers,
        std::span<AttributeListSyntax const*> attributes,
        TypeSyntax const* type,
        std::vector<MemberDeclarationSyntax const*>& members)
    {
        IncompleteDeclarationSyntax* result = this->_factory->CreateNode<IncompleteDeclarationSyntax>();
        result->Attributes = SyntaxListView<AttributeListSyntax>{this->_factory->CreateList(attributes)};
        result->Modifiers = SyntaxListView<SyntaxToken>{this->_factory->CreateList(modifiers)};
        result->Type = type;
        members.emplace_back(result);
    }

}
