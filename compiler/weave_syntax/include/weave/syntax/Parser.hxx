#pragma once
#include "weave/source/SourceText.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/SyntaxToken.hxx"
#include "weave/syntax/SyntaxFactory.hxx"
#include "weave/syntax/SyntaxFacts.hxx"

namespace weave::syntax
{
    struct ParserContext
    {
        SyntaxFactory* Factory;
        std::span<SyntaxToken*> Tokens{};
    };

    struct ParserState
    {
        ParserContext* Context;
        SyntaxToken* Current{};
        SyntaxToken* Last{};
    };

    constexpr ParserState Fork(ParserState const& state)
    {
        return ParserState{state.Context, state.Current, state.Last};
    }

    constexpr void Join(ParserState& state, ParserState const& other)
    {
        WEAVE_ASSERT(state.Context == other.Context, "Cannot join states from different contexts");
        WEAVE_ASSERT(state.Last == other.Last, "Cannot join states with different last tokens");
        state.Current = other.Current;
    }

    constexpr bool Advance(ParserState& self)
    {
        if (self.Current->Kind == SyntaxKind::EndOfFileToken)
        {
            return false;
        }

        self.Current = std::min(self.Current + 1, self.Last);

        return true;
    }

    constexpr SyntaxToken* Next(ParserState& self)
    {
        SyntaxToken* const current = self.Current;

        self.Current = std::min(self.Current + 1, self.Last);

        return current;
    }

    constexpr SyntaxToken* Current(ParserState const& self)
    {
        return self.Current;
    }

    constexpr SyntaxToken* Peek(ParserState const& self, size_t offset)
    {
        return std::min(self.Current + offset, self.Last);
    }

    inline SyntaxToken* Match(ParserState& self, SyntaxKind kind)
    {
        if (self.Current->Kind == kind)
        {
            return Next(self);
        }

        return self.Context->Factory->CreateMissingToken(kind, self.Current->Source);
    }

    inline SyntaxToken* TryMatch(ParserState& self, SyntaxKind kind)
    {
        if (self.Current->Kind == kind)
        {
            return Next(self);
        }

        return nullptr;
    }
}

namespace weave::syntax
{
    class Parser
    {
    private:
        SyntaxFactory* _factory{};
        std::vector<SyntaxToken*> _tokens{};
        size_t _index{};
        SyntaxToken* _current{};

    public:
        explicit Parser(
            source::DiagnosticSink* diagnostic,
            SyntaxFactory* factory,
            source::SourceText const& source);

    public:
        struct ResetPoint
        {
            friend class Parser;

        private:
            Parser const* _owner{};
            size_t _index{};

        private:
            explicit constexpr ResetPoint(
                Parser const* owner,
                size_t index)
                : _owner{owner}
                , _index{index}
            {
            }

        public:
            ResetPoint() = default;
            ResetPoint(ResetPoint const&) = default;
            ResetPoint(ResetPoint&&) = default;
            ResetPoint& operator=(ResetPoint const&) = default;
            ResetPoint& operator=(ResetPoint&&) = default;
        };

        [[nodiscard]] ResetPoint GetResetPoint() const
        {
            return ResetPoint{this, this->_index};
        }

        void Reset(ResetPoint const& resetPoint)
        {
            WEAVE_ASSERT(resetPoint._owner == this, "Invalid reset point");
            WEAVE_ASSERT(resetPoint._index < this->_tokens.size(), "Invalid reset point");

            this->_index = resetPoint._index;
            this->_current = this->_tokens[this->_index];
        }

    private:
        [[nodiscard]] SyntaxToken* Peek(size_t offset) const;
        [[nodiscard]] SyntaxToken* Current() const;
        SyntaxToken* Next();
        [[nodiscard]] SyntaxToken* Match(SyntaxKind kind);
        [[nodiscard]] SyntaxToken* TryMatch(SyntaxKind kind);

        [[nodiscard]] SyntaxToken* MatchContextualKeyword(SyntaxKind kind);
        [[nodiscard]] SyntaxToken* TryMatchContextualKeyword(SyntaxKind kind);

    private:
        [[nodiscard]] SyntaxToken* MatchUntil(std::vector<SyntaxNode*>& unexpected, SyntaxKind kind);

        [[nodiscard]] UnexpectedNodesSyntax* ConsumeUnexpected(SyntaxKind kind);

        template <typename CallbackT = bool(SyntaxKind)>
        [[nodiscard]] UnexpectedNodesSyntax* ConsumeUnexpectedIf(CallbackT callback)
        {
            std::vector<SyntaxNode*> tokens{};

            while (this->Current()->Kind != SyntaxKind::EndOfFileToken)
            {
                if (callback(this->Current()->Kind))
                {
                    tokens.push_back(this->Next());
                }
                else
                {
                    break;
                }
            }

            return this->CreateUnexpectedNodes(tokens);
        }

        void MatchUntil(SyntaxToken*& matched, UnexpectedNodesSyntax*& unexpected, SyntaxKind kind);

        template <typename CallbackT>
        [[nodiscard]] SyntaxToken* MatchUntil(UnexpectedNodesSyntax*& unexpected, CallbackT callback)
        {
            std::vector<SyntaxNode*> tokens{};

            while (this->Current()->Kind != SyntaxKind::EndOfFileToken)
            {
                if (callback(this->Current()->Kind))
                {
                    break;
                }

                tokens.push_back(this->Next());
            }

            unexpected = this->CreateUnexpectedNodes(tokens);
            return this->Current();
        }

        class LoopProgressCondition final
        {
        private:
            SyntaxToken* _current{};

        public:
            LoopProgressCondition(SyntaxToken* current)
                : _current{current}
            {
            }

            [[nodiscard]] bool Evaluate(SyntaxToken* current)
            {
                bool const advanced = this->_current != current;

                if (advanced)
                {
                    this->_current = current;
                }

                WEAVE_ASSERT(advanced, "Loop did not advance");
                return advanced;
            }
        };

    private:
        // NOTE:
        //      This is public only for unit test purposes.
        //
        //      When this compiler will bootstrap itself, a proper unit testing support will be
        //      integrated as well.
    public:
        [[nodiscard]] SourceFileSyntax* ParseSourceFile();

        void ParseCodeBlockItemList(
            std::vector<CodeBlockItemSyntax*>& items,
            bool global);

        CodeBlockSyntax* ParseCodeBlock();

        [[nodiscard]] CodeBlockItemSyntax* ParseCodeBlockItem();

        SyntaxListView<AttributeListSyntax> ParseAttributesList();

        void ParseMemberModifiersList(
            std::vector<SyntaxToken*>& elements);


        SyntaxListView<SyntaxToken> ParseFunctionParameterModifiersList();

        SyntaxListView<SyntaxToken> ParseFunctionArgumentModifierList();

        SyntaxListView<SyntaxToken> ParseTypeQualifiers();

        SyntaxListView<SyntaxToken> ParseTypeSpecifierList();

        VariableDeclarationSyntax* ParseVariableDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        FunctionDeclarationSyntax* ParseFunctionDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        DelegateDeclarationSyntax* ParseDelegateDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        NamespaceDeclarationSyntax* ParseNamespaceDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        ConceptDeclarationSyntax* ParseConceptDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        ExtendDeclarationSyntax* ParseExtendDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        StructDeclarationSyntax* ParseStructDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        TypeAliasDeclarationSyntax* ParseTypeAliasDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        ConstantDeclarationSyntax* ParseConstantDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        DeclarationSyntax* ParseDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        StatementSyntax* ParseStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        BlockStatementSyntax* ParseBlockStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        EmptyStatementSyntax* ParseEmptyStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        YieldStatementSyntax* ParseYieldStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        LoopStatementSyntax* ParseLoopStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        CheckedStatementSyntax* ParseCheckedStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        UncheckedStatementSyntax* ParseUncheckedStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        UnsafeStatementSyntax* ParseUnsafeStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        LazyStatementSyntax* ParseLazyStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        ParameterListSyntax* ParseParameterList();

        ParameterSyntax* ParseParameter();

        GenericParametersSyntax* ParseGenericParameters();

        GenericParametersSyntax* ParseOptionalGenericParameters();

        TypeGenericParameterSyntax* ParseTypeGenericParameter(bool& last);

        ConstGenericParameterSyntax* ParseConstGenericParameter(bool& last);

        GenericParameterSyntax* ParseGenericParameter(bool& last);

        GenericArgumentSyntax* ParseGenericArgument();

        GenericArgumentsSyntax* ParseGenericArguments();

    public:
        AttributeSyntax* ParseAttribute();

        AttributeListSyntax* ParseAttributeList();

        UsingDeclarationSyntax* ParseUsingDeclaration();

        ReturnTypeClauseSyntax* ParseReturnTypeClause();

        ReturnTypeClauseSyntax* ParseOptionalReturnTypeClause();

        ArrowExpressionClauseSyntax* ParseArrowExpressionClause();

        TypeClauseSyntax* ParseTypeClause();

        TypeClauseSyntax* ParseOptionalTypeClause();

        TypePointerSyntax* ParsePointerType();

        TypeSyntax* ParseType();

        InitializerClauseSyntax* ParseInitializerClause();

        InitializerClauseSyntax* ParseOptionalInitializerClause();

        ArgumentSyntax* ParseArgument();

        ArgumentListSyntax* ParseArgumentList();

        BracketedArgumentListSyntax* ParseBracketedArgumentList();

        NameSyntax* ParseQualifiedName();

        SimpleNameSyntax* ParseSimpleName();

        IdentifierNameSyntax* ParseIdentifierName();

        TupleIndexSyntax* ParseTupleIndex();

        UnreachableExpressionSyntax* ParseUnreachableExpression();

        EvalExpressionSyntax* ParseEvalExpression();

        ExpressionSyntax* ParseExpression(
            Precedence parentPrecedence = Precedence::Expression);

        ExpressionSyntax* ParseOptionalExpression(
            Precedence parentPrecedence = Precedence::Expression);

        ExpressionSyntax* ParseTerm(
            Precedence precedence);

        ExpressionSyntax* ParsePostfixExpression(
            ExpressionSyntax* expression);

        ExpressionSyntax* ParseTermWithoutPostfix(
            Precedence precedence);

        TupleExpressionSyntax* ParseTupleExpression();

        BracketInitializerClauseSyntax* ParseBracketInitializerClause();

        BraceInitializerClauseSyntax* ParseBraceInitializerClause();

        LabeledExpressionSyntax* ParseLabeledExpression();

        SizeOfExpressionSyntax* ParseSizeOfExpression();

        AlignOfExpressionSyntax* ParseAlignOfExpression();

        TypeOfExpressionSyntax* ParseTypeOfExpression();

        NameOfExpressionSyntax* ParseNameOfExpression();

        AddressOfExpressionSyntax* ParseAddressOfExpression();

        OldExpressionSyntax* ParseOldExpression();

        OutExpressionSyntax* ParseOutExpression();

        RefExpressionSyntax* ParseRefExpression();

        MoveExpressionSyntax* ParseMoveExpression();

        ExpressionSyntax* ParseBooleanLiteral();

        ExpressionSyntax* ParseIntegerLiteral();

        ExpressionSyntax* ParseFloatLiteral();

        ExpressionSyntax* ParseStringLiteral();

        ExpressionSyntax* ParseCharacterLiteral();

        IfExpressionSyntax* ParseIfExpression();

        WhileStatementSyntax* ParseWhileStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        StatementSyntax* ParseGotoStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        StatementSyntax* ParseBreakStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        StatementSyntax* ParseContinueStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        ElseClauseSyntax* ParseOptionalElseClause();

        StatementSyntax* ParseReturnStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        ExpressionStatementSyntax* ParseExpressionStatement(
            SyntaxListView<AttributeListSyntax> attributes);

        IdentifierNameSyntax* CreateMissingIdentifierName();

        UnexpectedNodesSyntax* CreateUnexpectedNodes(std::span<SyntaxNode*> nodes);

        BalancedTokenSequneceSyntax* ParseBalancedTokenSequence(
            SyntaxKind open,
            SyntaxKind close);

        SyntaxToken* MatchBalancedTokenSequence(
            SyntaxKind terminator,
            std::vector<SyntaxToken*>& tokens,
            std::vector<SyntaxNode*>& unexpected);

        TupleTypeSyntax* ParseTupleType();

        TupleTypeSyntax* ParseOptionalTupleType();

        TupleTypeElementSyntax* ParseTupleTypeElement();

        struct Label
        {
            SyntaxToken* Name{};
            SyntaxToken* Colon{};
        };

        std::optional<Label> ParseOptionalLabel();

        MatchExpressionSyntax* ParseMatchExpression();

        MatchClauseSyntax* ParseMatchClause();

        MatchCaseClauseSyntax* ParseMatchCaseClause();

        MatchDefaultClauseSyntax* ParseMatchDefaultClause();

        TypeSyntax* ParseArrayOrSliceType();

        TypeInheritanceClause* ParseTypeInheritanceClause();

        TypeInheritanceClause* ParseOptionalTypeInheritanceClause();

        EnumDeclarationSyntax* ParseEnumDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers);

        EnumMemberDeclarationSyntax* ParseEnumMemberDeclaration();

        RequiresClauseSyntax* ParseRequiresClause();

        EnsuresClauseSyntax* ParseEnsuresClause();

        InvariantClauseSyntax* ParseInvariantClause();

        WhereClauseSyntax* ParseWhereClause();

        WherePredicateSyntax* ParseWherePredicate();

        ConstraintClauseSyntax* ParseConstraintClause();

    public:
        PatternSyntax* ParsePattern();
        WildcardPatternSyntax* ParseWildcardPattern();
        LiteralPatternSyntax* ParseLiteralPattern();
        IdentifierPatternSyntax* ParseIdentifierPattern();
        SlicePatternItemSyntax* ParseSlicePatternItem();
        SlicePatternSyntax* ParseSlicePattern();
        TuplePatternItemSyntax* ParseTuplePatternItem();
        TuplePatternSyntax* ParseTuplePattern();
        PatternBindingSyntax* ParsePatternBinding();
    };

    void Validate(
        SourceFileSyntax* source,
        source::DiagnosticSink* diagnostic);
}
