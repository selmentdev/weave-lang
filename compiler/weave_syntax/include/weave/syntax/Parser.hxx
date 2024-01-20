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
        [[nodiscard]] SyntaxToken* MatchOptional(SyntaxKind kind);
        [[nodiscard]] SyntaxToken* TryMatch(SyntaxKind kind);
        [[nodiscard]] SyntaxToken* SkipToken(SyntaxKind kind, bool consume = true);

    private:
        struct ResetPoint
        {
            friend class Parser;

        private:
            size_t _index{};

        public:
            explicit constexpr ResetPoint(size_t index)
                : _index(index)
            {
            }
        };

        [[nodiscard]] constexpr ResetPoint GetResetPoint() const
        {
            return ResetPoint{this->_index};
        }

        void Reset(ResetPoint const& resetPoint)
        {
            this->_index = resetPoint._index;
        }

        // NOTE:
        //      This is public only for unit test purposes.
        //
        //      When this compiler will bootstrap itself, a proper unit testing support will be
        //      integrated as well.
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

        ParameterSyntax* ParseParameter(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

        ParameterListSyntax* ParseParameterList();

        FieldDeclarationSyntax* ParseFieldDeclaration(
            std::span<AttributeListSyntax*> attributes,
            std::span<SyntaxToken*> modifiers);

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

        StatementSyntax* ParseStatement();

        StatementSyntax* CreateMissingStatement(
            std::span<SyntaxToken*> tokens);

        BlockStatementSyntax* ParseBlockStatement();

        StatementSyntax* ParseVariableDeclaration();

        StatementSyntax* ParseIfStatement();

        StatementSyntax* ParseMisplacedElseClause();

        ElseClauseSyntax* ParseOptionalElseClause();

        StatementSyntax* ParseReturnStatement();

        ExpressionStatementSyntax* ParseExpressionStatement();

        IdentifierNameSyntax* CreateMissingIdentifierName();

        void ReportIncompleteMember(
            std::span<SyntaxToken*> modifiers,
            std::span<AttributeListSyntax*> attributes,
            TypeSyntax* type,
            std::vector<MemberDeclarationSyntax*>& members);
    };
}
