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
        [[nodiscard]] CompilationUnitSyntax* Parse();

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
        [[nodiscard]] UnexpectedNodesSyntax* ConsumeUnexpected(CallbackT callback)
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
        [[nodiscard]] CompilationUnitSyntax* ParseCompilationUnit();


        void ParseTypeBody(
            std::vector<ConstraintSyntax*>& constraints,
            std::vector<MemberDeclarationSyntax*>& members);

        void ParseNamespaceBody(
            SyntaxToken* openBraceOrSemicolon,
            std::vector<UsingDirectiveSyntax*>& usings,
            std::vector<MemberDeclarationSyntax*>& members);

        void ParseMemberModifiers(
            std::vector<SyntaxToken*>& modifiers);

        void ParseFunctionParameterModifiers(
            std::vector<SyntaxToken*>& modifiers);

        void ParseAttributesList(
            std::vector<AttributeListSyntax*>& attributes);

        MemberDeclarationSyntax* ParseMemberDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        AttributeSyntax* ParseAttribute();

        AttributeListSyntax* ParseAttributeList();

        NamespaceDeclarationSyntax* ParseNamespaceDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        StructDeclarationSyntax* ParseStructDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        ExtendDeclarationSyntax* ParseExtendDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        ConceptDeclarationSyntax* ParseConceptDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        UsingDirectiveSyntax* ParseUsingDirective();

        ReturnTypeClauseSyntax* ParseReturnTypeClause();

        ArrowExpressionClauseSyntax* ParseArrowExpressionClause();

        FunctionDeclarationSyntax* ParseFunctionDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        TypeClauseSyntax* ParseTypeClause();

        TypeClauseSyntax* ParseOptionalTypeClause();

        EqualsValueClauseSyntax* ParseEqualsValueClause();

        EqualsValueClauseSyntax* ParseOptionalEqualsValueClause();

        ArgumentSyntax* ParseArgument();

        ArgumentListSyntax* ParseArgumentList();

        BracketedArgumentListSyntax* ParseBracketedArgumentList();

        //ParameterSyntax* ParseParameter(
        //    std::span<AttributeListSyntax*> attributes,
        //    std::span<SyntaxToken*> modifiers);

        ConstantDeclarationSyntax* ParseConstantDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        NameSyntax* ParseQualifiedName();

        SimpleNameSyntax* ParseSimpleName();

        IdentifierNameSyntax* ParseIdentifierName();

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

        ExpressionSyntax* ParseBooleanLiteral();

        ExpressionSyntax* ParseIntegerLiteral();

        ExpressionSyntax* ParseFloatLiteral();

        ExpressionSyntax* ParseStringLiteral();

        ExpressionSyntax* ParseCharacterLiteral();

        StatementSyntax* CreateMissingStatement(
            std::span<SyntaxToken*> tokens);

        StatementSyntax* ParseIfStatement(
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

        ExpressionStatementSyntax* ParseExpressionStatement();

        IdentifierNameSyntax* CreateMissingIdentifierName();

        UnexpectedNodesSyntax* CreateUnexpectedNodes(std::span<SyntaxNode*> nodes);

        BalancedTokenSequneceSyntax* ParseBalancedTokenSequence(
            SyntaxKind open,
            SyntaxKind close);

        SyntaxToken* MatchBalancedTokenSequence(
            SyntaxKind terminator,
            std::vector<SyntaxToken*>& tokens,
            std::vector<SyntaxNode*>& unexpected);
    };
}
