#pragma once
#include "weave/source/SourceText.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/SyntaxToken.hxx"
#include "weave/syntax/SyntaxFactory.hxx"

namespace weave::syntax
{
    class Parser
    {
    private:
        source::DiagnosticSink* _diagnostic{};
        SyntaxFactory* _factory{};
        std::vector<SyntaxToken const*> _tokens{};
        size_t _index{};

    public:
        explicit Parser(
            source::DiagnosticSink* diagnostic,
            SyntaxFactory* factory,
            source::SourceText const& source);

    public:
        [[nodiscard]] CompilationUnitSyntax const* Parse();

    private:
        [[nodiscard]] SyntaxToken const* Peek(size_t offset) const;
        [[nodiscard]] SyntaxToken const* Current() const;
        SyntaxToken const* Next();
        [[nodiscard]] SyntaxToken const* Match(SyntaxKind kind);
        [[nodiscard]] SyntaxToken const* MatchOptional(SyntaxKind kind);
        [[nodiscard]] SyntaxToken const* TryMatch(SyntaxKind kind);
        [[nodiscard]] SyntaxToken const* SkipToken(SyntaxKind kind, bool consume = true);

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

    private:
        [[nodiscard]] CompilationUnitSyntax const* ParseCompilationUnit();

        void ParseTypeBody(
            std::vector<ConstraintSyntax const*>& constraints,
            std::vector<MemberDeclarationSyntax const*>& members);

        void ParseNamespaceBody(
            SyntaxToken const* openBraceOrSemicolon,
            std::vector<UsingDirectiveSyntax const*>& usings,
            std::vector<MemberDeclarationSyntax const*>& members);

        void ParseMemberModifiers(
            std::vector<SyntaxToken const*>& modifiers);

        void ParseFunctionParameterModifiers(
            std::vector<SyntaxToken const*>& modifiers);

        void ParseAttributesList(
            std::vector<AttributeListSyntax const*>& attributes);

        MemberDeclarationSyntax const* ParseMemberDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        AttributeListSyntax const* ParseAttributeList();

        NamespaceDeclarationSyntax const* ParseNamespaceDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        StructDeclarationSyntax const* ParseStructDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        ExtendDeclarationSyntax const* ParseExtendDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        ConceptDeclarationSyntax const* ParseConceptDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        UsingDirectiveSyntax const* ParseUsingDirective();

        FunctionDeclarationSyntax const* ParseFunctionDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        TypeClauseSyntax const* ParseOptionalTypeClause();

        TypeClauseSyntax const* ParseTypeClause();

        ParameterSyntax const* ParseParameter(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        ParameterListSyntax const* ParseParameterList();

        FieldDeclarationSyntax const* ParseFieldDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        ConstantDeclarationSyntax const* ParseConstantDeclaration(
            std::span<AttributeListSyntax const*> attributes,
            std::span<SyntaxToken const*> modifiers);

        NameSyntax const* ParseQualifiedName();

        SimpleNameSyntax const* ParseSimpleName();

        IdentifierNameSyntax const* ParseIdentifierName();

        void ReportIncompleteMember(
            std::span<SyntaxToken const*> modifiers,
            std::span<AttributeListSyntax const*> attributes,
            TypeSyntax const* type,
            std::vector<MemberDeclarationSyntax const*>& members);
    };
}
