#pragma once
#include "weave/source/SourceText.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/SyntaxToken.hxx"
#include "weave/syntax/SyntaxFactory.hxx"
#include "weave/syntax/SyntaxFacts.hxx"

namespace weave::syntax
{
    class Parser
    {
    private:
        source::DiagnosticSink* _diagnostic{};
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

        SyntaxListView<SyntaxToken> ParseMemberModifiersList();

        void ParseMemberModifiersList(
            std::vector<SyntaxToken*>& elements);

        SyntaxListView<SyntaxToken> ParseFunctionParameterModifiersList();

        VariableDeclarationSyntax* ParseVariableDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        FunctionDeclarationSyntax* ParseFunctionDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        NamespaceDeclarationSyntax* ParseNamespaceDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        ConceptDeclarationSyntax* ParseConceptDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        ExtendDeclarationSyntax* ParseExtendDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        StructDeclarationSyntax* ParseStructDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        DeclarationSyntax* ParseDeclaration(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        StatementSyntax* ParseStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        BlockStatementSyntax* ParseBlockStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        EmptyStatementSyntax* ParseEmptyStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        ParameterListSyntax* ParseParameterList();

        ParameterSyntax* ParseParameter();

    public:
        AttributeSyntax* ParseAttribute();

        AttributeListSyntax* ParseAttributeList();

        UsingDirectiveSyntax* ParseUsingDirective();

        ReturnTypeClauseSyntax* ParseReturnTypeClause();

        ArrowExpressionClauseSyntax* ParseArrowExpressionClause();

        TypeClauseSyntax* ParseTypeClause();

        TypeClauseSyntax* ParseOptionalTypeClause();

        TypeSyntax* ParseType();

        EqualsValueClauseSyntax* ParseEqualsValueClause();

        EqualsValueClauseSyntax* ParseOptionalEqualsValueClause();

        ArgumentSyntax* ParseArgument();

        ArgumentListSyntax* ParseArgumentList();

        BracketedArgumentListSyntax* ParseBracketedArgumentList();

        NameSyntax* ParseQualifiedName();

        SimpleNameSyntax* ParseSimpleName();

        IdentifierNameSyntax* ParseIdentifierName();

        TupleIndexSyntax* ParseTupleIndex();

        SelfExpressionSyntax* ParseSelfExpression();

        ExpressionSyntax* ParseExpression(
            Precedence parentPrecedence = Precedence::Expression);

        ExpressionSyntax* ParseTerm(
            Precedence precedence);

        ExpressionSyntax* ParsePostfixExpression(
            ExpressionSyntax* expression);

        ExpressionSyntax* ParseTermWithoutPostfix(
            Precedence precedence);

        ExpressionSyntax* ParseParenthesizedExpression();

        SizeOfExpressionSyntax* ParseSizeOfExpression();

        AlignOfExpressionSyntax* ParseAlignOfExpression();

        TypeOfExpressionSyntax* ParseTypeOfExpression();

        NameOfExpressionSyntax* ParseNameOfExpression();

        AddressOfExpressionSyntax* ParseAddressOfExpression();

        ExpressionSyntax* ParseBooleanLiteral();

        ExpressionSyntax* ParseIntegerLiteral();

        ExpressionSyntax* ParseFloatLiteral();

        ExpressionSyntax* ParseStringLiteral();

        ExpressionSyntax* ParseCharacterLiteral();

        StatementSyntax* ParseIfStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        StatementSyntax* ParseWhileStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        StatementSyntax* ParseGotoStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        StatementSyntax* ParseBreakStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        StatementSyntax* ParseContinueStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        StatementSyntax* ParseMisplacedElseClause(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

        ElseClauseSyntax* ParseOptionalElseClause();

        StatementSyntax* ParseReturnStatement(
            SyntaxListView<AttributeListSyntax> attributes,
            SyntaxListView<SyntaxToken> modifiers,
            UnexpectedNodesSyntax* unexpected);

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

        TupleTypeElementSyntax* ParseTupleTypeElement();
    };
}
