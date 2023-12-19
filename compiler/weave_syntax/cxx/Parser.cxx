#include "weave/syntax/Parser.hxx"

#include "weave/syntax/Declaration.hxx"
#include "weave/syntax/Expression.hxx"
#include "weave/syntax/Statement.hxx"

namespace weave::syntax
{
    Parser::Parser(
        source::DiagnosticSink& diagnostic,
        source::SourceText const& source,
        ParserContext& context)
        : _diagnostic{diagnostic}
        , _context{context}
    {
        tokenizer::Tokenizer tokenizer{this->_diagnostic, source, tokenizer::TriviaMode::All};

        while (tokenizer::Token* current = this->_context.TokenizerContext.Lex(tokenizer))
        {
            this->_tokens.push_back(current);

            if (current->Is(tokenizer::TokenKind::EndOfFile) or current->Is(tokenizer::TokenKind::Error))
            {
                break;
            }
        }
    }

    CompilationUnitDeclaration* Parser::Parse()
    {
        return this->ParseCompilationUnit();
    }

    tokenizer::Token* Parser::Peek(size_t offset) const
    {
        size_t const index = this->_index + offset;

        if (index >= this->_tokens.size())
        {
            return this->_tokens.back();
        }

        return this->_tokens[index];
    }

    tokenizer::Token* Parser::Current() const
    {
        return this->Peek(0);
    }

    tokenizer::Token* Parser::Next()
    {
        tokenizer::Token* current = this->Current();
        this->_index++;
        return current;
    }

    tokenizer::Token* Parser::Match(tokenizer::TokenKind kind)
    {
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        this->_diagnostic.AddError(
            this->Current()->GetSourceSpan(),
            fmt::format("unexpected token '{}', expected '{}'",
                tokenizer::TokenKindTraits::GetSpelling(this->Current()->GetKind()),
                tokenizer::TokenKindTraits::GetSpelling(kind)));

        return this->_context.TokenizerContext.CreateMissing(kind, this->Current()->GetSourceSpan());
    }

    tokenizer::Token* Parser::MatchOptional(tokenizer::TokenKind kind)
    {
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        return this->_context.TokenizerContext.CreateMissing(kind, this->Current()->GetSourceSpan());
    }

    tokenizer::Token* Parser::TryMatch(tokenizer::TokenKind kind)
    {
        if (this->Current()->Is(kind))
        {
            return this->Next();
        }

        return nullptr;
    }

    CompilationUnitDeclaration* Parser::ParseCompilationUnit()
    {
        std::vector<UsingStatement*> usings{};
        std::vector<MemberDeclaration*> members{};

        ParseNamespaceBody(usings, members);

        tokenizer::Token* eof = this->Match(tokenizer::TokenKind::EndOfFile);

        CompilationUnitDeclaration* result = this->_context.NodesAllocator.Emplace<CompilationUnitDeclaration>();
        result->EndOfFile = eof;
        result->Usings = this->_context.NodesAllocator.EmplaceArray<UsingStatement*>(usings);
        result->Members = this->_context.NodesAllocator.EmplaceArray<MemberDeclaration*>(members);
        return result;
    }

    void Parser::ParseNamespaceBody(std::vector<UsingStatement*>& usings, std::vector<MemberDeclaration*>& members)
    {
        std::vector<tokenizer::Token*> incomplete{};

        while (tokenizer::Token* current = this->Current())
        {
            if (current->Is(tokenizer::TokenKind::CloseBraceToken))
            {
                break;
            }

            if (current->Is(tokenizer::TokenKind::EndOfFile))
            {
                break;
            }

            if (not incomplete.empty())
            {
                auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
                IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
                members.emplace_back(node);
                incomplete.clear();
            }

            if (current->Is(tokenizer::TokenKind::NamespaceKeyword))
            {
                members.emplace_back(this->ParseNamespaceDeclaration());
            }
            else if (current->Is(tokenizer::TokenKind::UsingKeyword))
            {
                if (not members.empty())
                {
                    this->_diagnostic.AddError(
                        current->GetSourceSpan(),
                        "using statement must precede all other elements defined in namespace");
                }

                usings.emplace_back(this->ParseUsingStatement());
            }
            else if (current->Is(tokenizer::TokenKind::StructKeyword))
            {
                members.emplace_back(this->ParseStructDeclaration());
            }

            if (this->Current() == current)
            {
                incomplete.emplace_back(current);

                if (this->Next() == current)
                {
                    // Cannot advance parsing.
                    break;
                }
            }
        }
    }

    NamespaceDeclaration* Parser::ParseNamespaceDeclaration()
    {
        tokenizer::Token* tkNamespace = this->Match(tokenizer::TokenKind::NamespaceKeyword);
        NameExpression* exprName = this->ParseQualifiedName();
        tokenizer::Token* tkOpenBrace = this->Match(tokenizer::TokenKind::OpenBraceToken);

        std::vector<UsingStatement*> usings{};
        std::vector<MemberDeclaration*> members{};

        ParseNamespaceBody(usings, members);

        tokenizer::Token* tkCloseBrace = this->Match(tokenizer::TokenKind::CloseBraceToken);

        NamespaceDeclaration* result = this->_context.NodesAllocator.Emplace<NamespaceDeclaration>();
        result->NamespaceKeyword = tkNamespace;
        result->Name = exprName;
        result->OpenBraceToken = tkOpenBrace;
        result->Usings = this->_context.NodesAllocator.EmplaceArray<UsingStatement*>(usings);
        result->Members = this->_context.NodesAllocator.EmplaceArray<MemberDeclaration*>(members);
        result->CloseBraceToken = tkCloseBrace;
        return result;
    }

    StructDeclaration* Parser::ParseStructDeclaration()
    {
        tokenizer::Token* tkStruct = this->Match(tokenizer::TokenKind::StructKeyword);
        NameExpression* exprName = this->ParseQualifiedName();
        tokenizer::Token* tkOpenBrace = this->Match(tokenizer::TokenKind::OpenBraceToken);
        tokenizer::Token* tkCloseBrace = this->Match(tokenizer::TokenKind::CloseBraceToken);
        tokenizer::Token* tkSemicolon = this->Match(tokenizer::TokenKind::SemicolonToken);

        StructDeclaration* result = this->_context.NodesAllocator.Emplace<StructDeclaration>();
        result->StructKeyword = tkStruct;
        result->Name = exprName;
        result->OpenBraceToken = tkOpenBrace;
        result->CloseBraceToken = tkCloseBrace;
        result->SemicolonToken = tkSemicolon;
        return result;
    }

    UsingStatement* Parser::ParseUsingStatement()
    {
        tokenizer::Token* tkUsing = this->Match(tokenizer::TokenKind::UsingKeyword);
        NameExpression* exprName = this->ParseQualifiedName();
        tokenizer::Token* tkSemicolon = this->Match(tokenizer::TokenKind::SemicolonToken);

        UsingStatement* result = this->_context.NodesAllocator.Emplace<UsingStatement>();
        result->UsingKeyword = tkUsing;
        result->Name = exprName;
        result->Semicolon = tkSemicolon;
        return result;
    }

    IdentifierNameExpression* Parser::ParseIdentifierNameExpression()
    {
        tokenizer::Token* tkIdentifier = this->Match(tokenizer::TokenKind::Identifier);
        return this->_context.NodesAllocator.Emplace<IdentifierNameExpression>(tkIdentifier);
    }

    NameExpression* Parser::ParseQualifiedName()
    {
        tokenizer::Token* tkLeftIdentifier = this->Match(tokenizer::TokenKind::Identifier);
        NameExpression* exprLeft = this->_context.NodesAllocator.Emplace<IdentifierNameExpression>(tkLeftIdentifier);

        do
        {
            tokenizer::Token* const tkDot = this->Current();
            tokenizer::Token* const tkRightIdentifier = this->Peek(1);

            if (tkDot->Is(tokenizer::TokenKind::DotToken) and tkRightIdentifier->Is(tokenizer::TokenKind::Identifier))
            {
                this->Next();
                this->Next();

                SimpleNameExpression* exprRight = this->_context.NodesAllocator.Emplace<IdentifierNameExpression>(tkRightIdentifier);

                exprLeft = this->_context.NodesAllocator.Emplace<QualifiedNameExpression>(exprLeft, tkDot, exprRight);
            }
            else
            {
                break;
            }
        } while (true);

        return exprLeft;
    }
}
