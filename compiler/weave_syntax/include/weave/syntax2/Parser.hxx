#pragma once
#include "weave/syntax2/SyntaxNode.hxx"
#include "weave/syntax2/SyntaxToken.hxx"
#include "weave/syntax2/SyntaxFactory.hxx"

namespace weave::syntax2
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
            SyntaxFactory* factory);
    public:
        [[nodiscard]] CompilationUnit* Parse();

    private:
        [[nodiscard]] SyntaxToken* Peek(size_t offset) const;
        [[nodiscard]] SyntaxToken* Current() const;
        SyntaxToken* Next();
        [[nodiscard]] SyntaxToken* Match(SyntaxKind kind);
        [[nodiscard]] SyntaxToken* MatchOptional(SyntaxKind kind);
        [[nodiscard]] SyntaxToken* TryMatch(SyntaxKind kind);

    private:
        [[nodiscard]] CompilationUnit* ParseCompilationUnit();

        void ReportIncompleteMember(
            std::vector<SyntaxToken*>& tokens,
            std::vector<MemberDeclaration*>& members);
    };
}
