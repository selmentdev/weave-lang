#pragma once
#include "weave/tokenizer/Tokenizer.hxx"
#include "weave/tokenizer/TokenizerContext.hxx"

#include "weave/syntax/SyntaxNode.hxx"

namespace weave::syntax
{
    class ParserContext final
    {
    public:
        // Owns memory allocated by tokenizer.
        tokenizer::TokenizerContext TokenizerContext{};

        // Owns memory allocated by parser.
        memory::LinearAllocator NodesAllocator{128u << 10u};
    };

    class Parser final
    {
    private:
        std::vector<tokenizer::Token*> _tokens{};
        source::DiagnosticSink& _diagnostic;
        size_t _index{};
        ParserContext& _context;

    public:
        explicit Parser(
            source::DiagnosticSink& diagnostic,
            source::SourceText const& source,
            ParserContext& context);

        [[nodiscard]] CompilationUnitDeclaration* Parse();

    private:
        [[nodiscard]] tokenizer::Token* Peek(size_t offset) const;

        [[nodiscard]] tokenizer::Token* Current() const;

        tokenizer::Token* Next();

        [[nodiscard]] tokenizer::Token* Match(tokenizer::TokenKind kind);

        [[nodiscard]] tokenizer::Token* MatchOptional(tokenizer::TokenKind kind);

        [[nodiscard]] tokenizer::Token* TryMatch(tokenizer::TokenKind kind);

    private:
        CompilationUnitDeclaration* ParseCompilationUnit();

        void ParseNamespaceBody(
            std::vector<UsingStatement*>& usings,
            std::vector<MemberDeclaration*>& members);

        NamespaceDeclaration* ParseNamespaceDeclaration();

        StructDeclaration* ParseStructDeclaration();

        UsingStatement* ParseUsingStatement();

        IdentifierNameExpression* ParseIdentifierNameExpression();

        NameExpression* ParseQualifiedName();
    };
}
