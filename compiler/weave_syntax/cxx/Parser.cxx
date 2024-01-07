#include "weave/syntax/Parser.hxx"

#include "weave/syntax/Declaration.hxx"
#include "weave/syntax/Expression.hxx"
#include "weave/syntax/Statement.hxx"

#include "weave/bitwise/Flag.hxx"

#include <array>

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

    void ReportDuplicatedModifier(
        source::DiagnosticSink& diagnostic,
        tokenizer::Token const* previous, tokenizer::Token const* current)
    {
        diagnostic.AddError(
            current->GetSourceSpan(),
            fmt::format("duplicate modifier '{}'",
                tokenizer::TokenKindTraits::GetSpelling(current->GetKind())));

        diagnostic.AddHint(
            previous->GetSourceSpan(),
            fmt::format("modifier '{}' previously specified here",
                tokenizer::TokenKindTraits::GetSpelling(previous->GetKind())));
    }

    template <typename KeyT, size_t N>
    void ParseModifiers(
        source::DiagnosticSink& diagnostic,
        bitwise::Flags<KeyT>& modifiers,
        std::array<std::pair<KeyT, tokenizer::TokenKind>, N> const& mapping,
        std::vector<tokenizer::Token*>& tokens)
    {
        std::array<tokenizer::Token const*, N> previous{};

        modifiers = {};

        for (auto it = tokens.begin(); it != tokens.end();)
        {
            tokenizer::Token const* token = *it;

            bool removed = false;

            for (size_t i = 0; i < N; ++i)
            {
                auto const [modifier, kind] = mapping[i];

                if (token->Is(kind))
                {
                    if (previous[i])
                    {
                        ReportDuplicatedModifier(diagnostic, previous[i], token);
                    }
                    else
                    {
                        previous[i] = token;
                        modifiers |= modifier;
                    }

                    it = tokens.erase(it);
                    removed = true;
                    break;
                }
            }

            if (not removed)
            {
                ++it;
            }
        }
    }

    void Parser::ParseFieldModifier(bitwise::Flags<FieldModifier>& modifiers, std::vector<tokenizer::Token*>& tokens) const
    {
        static constexpr std::array mapping{
            std::pair{FieldModifier::Public, tokenizer::TokenKind::PublicKeyword},
            std::pair{FieldModifier::Private, tokenizer::TokenKind::PrivateKeyword},
            std::pair{FieldModifier::Internal, tokenizer::TokenKind::InternalKeyword},
            std::pair{FieldModifier::Readonly, tokenizer::TokenKind::ReadonlyKeyword},
        };

        ParseModifiers(this->_diagnostic, modifiers, mapping, tokens);
    }

    void Parser::ParseStructModifier(bitwise::Flags<StructModifier>& modifiers, std::vector<tokenizer::Token*>& tokens) const
    {
        static constexpr std::array mapping{
            std::pair{StructModifier::Public, tokenizer::TokenKind::PublicKeyword},
            std::pair{StructModifier::Private, tokenizer::TokenKind::PrivateKeyword},
            std::pair{StructModifier::Internal, tokenizer::TokenKind::InternalKeyword},
            std::pair{StructModifier::Partial, tokenizer::TokenKind::PartialKeyword},
        };

        ParseModifiers(this->_diagnostic, modifiers, mapping, tokens);
    }

    void Parser::ParseExtendModifier(bitwise::Flags<ExtendModifier>& modifiers, std::vector<tokenizer::Token*>& tokens) const
    {
        static constexpr std::array mapping{
            std::pair{ExtendModifier::Public, tokenizer::TokenKind::PublicKeyword},
            std::pair{ExtendModifier::Private, tokenizer::TokenKind::PrivateKeyword},
            std::pair{ExtendModifier::Internal, tokenizer::TokenKind::InternalKeyword},
            std::pair{ExtendModifier::Unsafe, tokenizer::TokenKind::UnsafeKeyword},
        };

        ParseModifiers(this->_diagnostic, modifiers, mapping, tokens);
    }

    void Parser::ParseConceptModifier(bitwise::Flags<ConceptModifier>& modifiers, std::vector<tokenizer::Token*>& tokens) const
    {
        static constexpr std::array mapping{
            std::pair{ConceptModifier::Public, tokenizer::TokenKind::PublicKeyword},
            std::pair{ConceptModifier::Private, tokenizer::TokenKind::PrivateKeyword},
            std::pair{ConceptModifier::Internal, tokenizer::TokenKind::InternalKeyword},
            std::pair{ConceptModifier::Unsafe, tokenizer::TokenKind::UnsafeKeyword},
        };

        ParseModifiers(this->_diagnostic, modifiers, mapping, tokens);
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

            if (current->Is(tokenizer::TokenKind::SemicolonToken))
            {
                this->_diagnostic.AddHint(
                    current->GetSourceSpan(),
                    "unexpected semicolon");
                this->Next();
                continue;
            }

            if (current->Is(tokenizer::TokenKind::NamespaceKeyword))
            {
                if (not incomplete.empty())
                {
                    for (tokenizer::Token const* tk : incomplete)
                    {
                        this->_diagnostic.AddError(
                            tk->GetSourceSpan(),
                            fmt::format("unexpected token '{}'",
                                tokenizer::TokenKindTraits::GetSpelling(tk->GetKind())));
                    }

                    auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
                    IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
                    members.emplace_back(node);
                    incomplete.clear();
                }

                members.emplace_back(this->ParseNamespaceDeclaration());
            }
            else if (current->Is(tokenizer::TokenKind::UsingKeyword))
            {
                if (not incomplete.empty())
                {
                    for (tokenizer::Token const* tk : incomplete)
                    {
                        this->_diagnostic.AddError(
                            tk->GetSourceSpan(),
                            fmt::format("unexpected token '{}'",
                                tokenizer::TokenKindTraits::GetSpelling(tk->GetKind())));
                    }

                    auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
                    IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
                    members.emplace_back(node);
                    incomplete.clear();
                }

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
                bitwise::Flags<StructModifier> modifiers{};

                ParseStructModifier(modifiers, incomplete);

                if (not incomplete.empty())
                {
                    for (tokenizer::Token const* tk : incomplete)
                    {
                        this->_diagnostic.AddError(
                            tk->GetSourceSpan(),
                            fmt::format("unexpected token '{}'",
                                tokenizer::TokenKindTraits::GetSpelling(tk->GetKind())));
                    }

                    auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
                    IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
                    members.emplace_back(node);
                    incomplete.clear();
                }

                members.emplace_back(this->ParseStructDeclaration(modifiers));
            }
            else if (current->Is(tokenizer::TokenKind::ConceptKeyword))
            {
                bitwise::Flags<ConceptModifier> modifiers{};

                ParseConceptModifier(modifiers, incomplete);

                if (not incomplete.empty())
                {
                    for (tokenizer::Token const* tk : incomplete)
                    {
                        this->_diagnostic.AddError(
                            tk->GetSourceSpan(),
                            fmt::format("unexpected token '{}'",
                                tokenizer::TokenKindTraits::GetSpelling(tk->GetKind())));
                    }

                    auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
                    IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
                    members.emplace_back(node);
                    incomplete.clear();
                }

                members.emplace_back(this->ParseConceptDeclaration(modifiers));
            }
            else if (current->Is(tokenizer::TokenKind::ExtendKeyword))
            {
                bitwise::Flags<ExtendModifier> modifiers{};

                ParseExtendModifier(modifiers, incomplete);

                if (not incomplete.empty())
                {
                    for (tokenizer::Token const* tk : incomplete)
                    {
                        this->_diagnostic.AddError(
                            tk->GetSourceSpan(),
                            fmt::format("unexpected token '{}'",
                                tokenizer::TokenKindTraits::GetSpelling(tk->GetKind())));
                    }

                    auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
                    IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
                    members.emplace_back(node);
                    incomplete.clear();
                }

                members.emplace_back(this->ParseExtendDeclaration(modifiers));
            }
            else
            {
                // Cannot parse token.
                incomplete.push_back(current);

                // Advance past token.
                this->Next();

                if (this->Current() == current)
                {
                    // Cannot advance parsing.
                    break;
                }
            }
        }

        if (not incomplete.empty())
        {
            for (tokenizer::Token const* tk : incomplete)
            {
                this->_diagnostic.AddError(
                    tk->GetSourceSpan(),
                    fmt::format("unexpected token '{}'",
                        tokenizer::TokenKindTraits::GetSpelling(tk->GetKind())));
            }

            auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
            IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
            members.emplace_back(node);
            incomplete.clear();
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

    StructDeclaration* Parser::ParseStructDeclaration(bitwise::Flags<StructModifier> modifiers)
    {
        tokenizer::Token* tkStruct = this->Match(tokenizer::TokenKind::StructKeyword);
        NameExpression* exprName = this->ParseQualifiedName();

        std::vector<tokenizer::Token*> incomplete{};
        std::vector<MemberDeclaration*> members{};

        if (tokenizer::Token* tkExclamation = this->TryMatch(tokenizer::TokenKind::ExclamationToken))
        {
            tokenizer::Token* tkGenericStart = this->Match(tokenizer::TokenKind::OpenBracketToken);

            // Ignore everything between `![` and `]`.
            while (not this->Current()->Is(tokenizer::TokenKind::CloseBracketToken))
            {
                tokenizer::Token* current = this->Next();
                incomplete.push_back(current);
                if (current == this->Current())
                {
                    break;
                }
            }

            if (not incomplete.empty())
            {
                auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
                IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
                members.emplace_back(node);
                incomplete.clear();
            }

            tokenizer::Token* tkGenericEnd = this->Match(tokenizer::TokenKind::CloseBracketToken);

            (void)tkExclamation;
            (void)tkGenericStart;
            (void)tkGenericEnd;
        }

        // Ignore everything between struct and `{`.
        while (not this->Current()->Is(tokenizer::TokenKind::OpenBraceToken))
        {
            tokenizer::Token* current = this->Next();
            incomplete.push_back(current);
            if (current == this->Current())
            {
                break;
            }
        }

        if (not incomplete.empty())
        {
            auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
            IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
            members.emplace_back(node);
            incomplete.clear();
        }


        tokenizer::Token* tkOpenBrace = this->Match(tokenizer::TokenKind::OpenBraceToken);

        while (not this->Current()->Is(tokenizer::TokenKind::CloseBraceToken))
        {
            tokenizer::Token* current = this->Next();
            incomplete.push_back(current);
            if (current == this->Current())
            {
                break;
            }
        }

        if (not incomplete.empty())
        {
            auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
            IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
            members.emplace_back(node);
            incomplete.clear();
        }

        tokenizer::Token* tkCloseBrace = this->Match(tokenizer::TokenKind::CloseBraceToken);
        tokenizer::Token* tkSemicolon = this->TryMatch(tokenizer::TokenKind::SemicolonToken);

        StructDeclaration* result = this->_context.NodesAllocator.Emplace<StructDeclaration>();
        result->Modifiers = modifiers;
        result->StructKeyword = tkStruct;
        result->Name = exprName;
        result->Members = this->_context.NodesAllocator.EmplaceArray<MemberDeclaration*>(members);
        result->OpenBraceToken = tkOpenBrace;
        result->CloseBraceToken = tkCloseBrace;
        result->SemicolonToken = tkSemicolon;
        return result;
    }

    ExtendDeclaration* Parser::ParseExtendDeclaration(bitwise::Flags<ExtendModifier> modifiers)
    {
        tokenizer::Token* tkExtend = this->Match(tokenizer::TokenKind::ExtendKeyword);
        NameExpression* exprName = this->ParseQualifiedName();

        std::vector<tokenizer::Token*> incomplete{};
        std::vector<MemberDeclaration*> members{};

        tokenizer::Token* tkOpenBrace = this->Match(tokenizer::TokenKind::OpenBraceToken);

        //while (not this->Current()->Is(tokenizer::TokenKind::CloseBraceToken))
        //{
        //    tokenizer::Token* current = this->Next();
        //    incomplete.push_back(current);
        //    if (current == this->Current())
        //    {
        //        break;
        //    }
        //}
        //
        //if (not incomplete.empty())
        //{
        //    auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
        //    IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
        //    members.emplace_back(node);
        //    incomplete.clear();
        //}

        tokenizer::Token* tkCloseBrace = this->Match(tokenizer::TokenKind::CloseBraceToken);
        [[maybe_unused]] tokenizer::Token* tkSemicolon = this->TryMatch(tokenizer::TokenKind::SemicolonToken);

        ExtendDeclaration* result = this->_context.NodesAllocator.Emplace<ExtendDeclaration>();
        result->Modifiers = modifiers;
        result->ExtendKeyword = tkExtend;
        result->Name = exprName;
        result->Members = this->_context.NodesAllocator.EmplaceArray<MemberDeclaration*>(members);
        result->OpenBraceToken = tkOpenBrace;
        result->CloseBraceToken = tkCloseBrace;
        return result;
    }

    ConceptDeclaration* Parser::ParseConceptDeclaration(bitwise::Flags<ConceptModifier> modifiers)
    {
        tokenizer::Token* tkConcept = this->Match(tokenizer::TokenKind::ConceptKeyword);
        NameExpression* exprName = this->ParseQualifiedName();

        std::vector<tokenizer::Token*> incomplete{};
        std::vector<MemberDeclaration*> members{};

        tokenizer::Token* tkOpenBrace = this->Match(tokenizer::TokenKind::OpenBraceToken);

        //while (not this->Current()->Is(tokenizer::TokenKind::CloseBraceToken))
        //{
        //    tokenizer::Token* current = this->Next();
        //    incomplete.push_back(current);
        //    if (current == this->Current())
        //    {
        //        break;
        //    }
        //}
        //
        //if (not incomplete.empty())
        //{
        //    auto copyIncomplete = this->_context.NodesAllocator.EmplaceArray<tokenizer::Token*>(incomplete);
        //    IncompleteMemberDeclaration* node = this->_context.NodesAllocator.Emplace<IncompleteMemberDeclaration>(copyIncomplete);
        //    members.emplace_back(node);
        //    incomplete.clear();
        //}

        tokenizer::Token* tkCloseBrace = this->Match(tokenizer::TokenKind::CloseBraceToken);
        [[maybe_unused]] tokenizer::Token* tkSemicolon = this->TryMatch(tokenizer::TokenKind::SemicolonToken);

        ConceptDeclaration* result = this->_context.NodesAllocator.Emplace<ConceptDeclaration>();
        result->Modifiers = modifiers;
        result->ConceptKeyword = tkConcept;
        result->Name = exprName;
        result->Members = this->_context.NodesAllocator.EmplaceArray<MemberDeclaration*>(members);
        result->OpenBraceToken = tkOpenBrace;
        result->CloseBraceToken = tkCloseBrace;
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
        tokenizer::Token* tkRaw = this->Match(tokenizer::TokenKind::Identifier);
        tokenizer::IdentifierToken* tkIdentifier = tkRaw->TryCast<tokenizer::IdentifierToken>();
        return this->_context.NodesAllocator.Emplace<IdentifierNameExpression>(tkIdentifier);
    }

    NameExpression* Parser::ParseQualifiedName()
    {
        tokenizer::Token* tkLeftIdentifier = this->Match(tokenizer::TokenKind::Identifier);
        NameExpression* exprLeft = this->_context.NodesAllocator.Emplace<IdentifierNameExpression>(static_cast<tokenizer::IdentifierToken*>(tkLeftIdentifier));

        do
        {
            tokenizer::Token* const tkDot = this->Current();
            tokenizer::Token* const tkRightIdentifier = this->Peek(1);

            if (tkDot->Is(tokenizer::TokenKind::DotToken) and tkRightIdentifier->Is(tokenizer::TokenKind::Identifier))
            {
                this->Next();
                this->Next();

                SimpleNameExpression* exprRight = this->_context.NodesAllocator.Emplace<IdentifierNameExpression>(static_cast<tokenizer::IdentifierToken*>(tkRightIdentifier));

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
