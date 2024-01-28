#include "weave/platform/Compiler.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

#include "weave/syntax/Parser.hxx"
#include "weave/syntax/Visitor.hxx"

#include <compare>

class ParserHelper final
{
public:
    struct Entry
    {
        size_t Depth{};
        weave::syntax::SyntaxKind Kind{};
        bool Missing{};

        [[nodiscard]] constexpr auto operator<=>(const Entry&) const = default;
    };

    std::vector<Entry> Entries;
    weave::source::DiagnosticSink Diagnostic{};
    weave::syntax::SyntaxFactory Factory{};
    weave::syntax::SyntaxNode* Root{};
    weave::source::SourceText Text;

private:
    class Walker final : public weave::syntax::SyntaxWalker
    {
    public:
        std::vector<Entry>& _entries;

    public:
        explicit Walker(std::vector<Entry>& entries)
            : SyntaxWalker{false}
            , _entries(entries)
        {
        }

        void OnDefault(weave::syntax::SyntaxNode* node) override
        {
            auto& entry = this->_entries.emplace_back(this->Depth, node->Kind);

            // TODO: SyntaxNodeKind should be testable if we have a proper token.

            if (weave::syntax::IsToken(node->Kind))
            {
                auto const* const token = static_cast<weave::syntax::SyntaxToken*>(node);
                entry.Missing = token->IsMissing();
            }

            SyntaxWalker::OnDefault(node);
        }
    };

public:
    ParserHelper(std::string_view source)
        : Text{std::string{source}}
    {
        weave::syntax::Parser parser{&this->Diagnostic, &this->Factory, this->Text};
        Walker walker{this->Entries};

        this->Root = parser.ParseSourceFile();
        walker.Dispatch(this->Root);
    }
};

class Validator final
{
private:
    ParserHelper& _helper;
    size_t _index{};
    size_t _depth{};


public:
    explicit Validator(ParserHelper& helper)
        : _helper{helper}
        , _index{}
    {
    }

    void operator()(size_t depth, weave::syntax::SyntaxKind kind, bool missing = false)
    {
        CAPTURE(this->_index);
        REQUIRE(this->_index < this->_helper.Entries.size());
        CHECK(this->_helper.Entries[this->_index].Kind == kind);
        CHECK(this->_helper.Entries[this->_index].Depth == depth);
        CHECK(this->_helper.Entries[this->_index].Missing == missing);
        ++this->_index;
    }

    void operator()(weave::syntax::SyntaxKind kind, bool missing = false)
    {
        CAPTURE(this->_index);
        REQUIRE(this->_index < this->_helper.Entries.size());
        CHECK(this->_helper.Entries[this->_index].Kind == kind);
        CHECK(this->_helper.Entries[this->_index].Depth == this->_depth);
        CHECK(this->_helper.Entries[this->_index].Missing == missing);
        ++this->_index;
    }

    void Enter()
    {
        ++this->_depth;
    }

    void Leave()
    {
        --this->_depth;
    }
};

TEST_CASE("parser - valid 'if' statement with true")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if true { }

})___"};

    REQUIRE(helper.Entries.size() == 11);
    Validator N{helper};
    N(SyntaxKind::BlockStatementSyntax);
    N.Enter();
    {
        N(SyntaxKind::OpenBraceToken);
        N(SyntaxKind::SyntaxList);
        N.Enter();
        {
            N(SyntaxKind::IfStatementSyntax);
            N.Enter();
            {
                N(SyntaxKind::IfKeyword);
                N(SyntaxKind::LiteralExpressionSyntax);
                N.Enter();
                {
                    N(SyntaxKind::TrueKeyword);
                }
                N.Leave();
                N(SyntaxKind::BlockStatementSyntax);
                N.Enter();
                {
                    N(SyntaxKind::OpenBraceToken);
                    N(SyntaxKind::CloseBraceToken);
                }
                N.Leave();
            }
            N.Leave();
        }
        N.Leave();
        N(SyntaxKind::CloseBraceToken);
    }
}

TEST_CASE("parser - valid 'if' 'else' statement")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if true {
} else {
}

})___"};

    REQUIRE(helper.Entries.size() == 16);
    Validator N{helper};
    N(0, SyntaxKind::BlockStatementSyntax);
    N(1, SyntaxKind::OpenBraceToken);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::IfStatementSyntax);
    N(3, SyntaxKind::IfKeyword);
    N(3, SyntaxKind::LiteralExpressionSyntax);
    N(4, SyntaxKind::TrueKeyword);
    N(3, SyntaxKind::BlockStatementSyntax);
    N(4, SyntaxKind::OpenBraceToken);
    N(4, SyntaxKind::CloseBraceToken);
    N(3, SyntaxKind::ElseClauseSyntax);
    N(4, SyntaxKind::ElseKeyword);
    N(4, SyntaxKind::BlockStatementSyntax);
    N(5, SyntaxKind::OpenBraceToken);
    N(5, SyntaxKind::CloseBraceToken);
    N(1, SyntaxKind::CloseBraceToken);
}

TEST_CASE("parser - valid 'if' 'else if' statement")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if true {
} else if false {
}

})___"};

    REQUIRE(helper.Entries.size() == 20);
    Validator N{helper};

    N(0, SyntaxKind::BlockStatementSyntax);
    N(1, SyntaxKind::OpenBraceToken);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::IfStatementSyntax);
    N(3, SyntaxKind::IfKeyword);
    N(3, SyntaxKind::LiteralExpressionSyntax);
    N(4, SyntaxKind::TrueKeyword);
    N(3, SyntaxKind::BlockStatementSyntax);
    N(4, SyntaxKind::OpenBraceToken);
    N(4, SyntaxKind::CloseBraceToken);
    N(3, SyntaxKind::ElseClauseSyntax);
    N(4, SyntaxKind::ElseKeyword);
    N(4, SyntaxKind::IfStatementSyntax);
    N(5, SyntaxKind::IfKeyword);
    N(5, SyntaxKind::LiteralExpressionSyntax);
    N(6, SyntaxKind::FalseKeyword);
    N(5, SyntaxKind::BlockStatementSyntax);
    N(6, SyntaxKind::OpenBraceToken);
    N(6, SyntaxKind::CloseBraceToken);
    N(1, SyntaxKind::CloseBraceToken);
}

TEST_CASE("parser - misplaced 'else' statement - final else before if")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if true {
} else {
} else if false {
}

})___"};

    REQUIRE(helper.Entries.size() == 33);
    Validator N{helper};

    N(0, SyntaxKind::BlockStatementSyntax);
    {
        N(1, SyntaxKind::OpenBraceToken);
        N(1, SyntaxKind::SyntaxList);
        {
            N(2, SyntaxKind::IfStatementSyntax);
            {
                N(3, SyntaxKind::IfKeyword);
                N(3, SyntaxKind::LiteralExpressionSyntax);
                {
                    N(4, SyntaxKind::TrueKeyword);
                }
                N(3, SyntaxKind::BlockStatementSyntax);
                {
                    N(4, SyntaxKind::OpenBraceToken);
                    N(4, SyntaxKind::CloseBraceToken);
                }
                N(3, SyntaxKind::ElseClauseSyntax);
                {
                    N(4, SyntaxKind::ElseKeyword);
                    N(4, SyntaxKind::BlockStatementSyntax);
                    {
                        N(5, SyntaxKind::OpenBraceToken);
                        N(5, SyntaxKind::CloseBraceToken);
                    }
                }
            }
            // TODO: Verifiy if this should be parsed as broken if
            N(2, SyntaxKind::IfStatementSyntax);
            {
                N(3, SyntaxKind::IfKeyword, true);
                N(3, SyntaxKind::IdentifierNameSyntax);
                {
                    N(4, SyntaxKind::IdentifierToken, true);
                }
                N(3, SyntaxKind::ExpressionStatementSyntax);
                {
                    N(4, SyntaxKind::IdentifierNameSyntax);
                    {
                        N(5, SyntaxKind::IdentifierToken, true);
                    }
                    N(4, SyntaxKind::SemicolonToken, true);
                }
                N(3, SyntaxKind::ElseClauseSyntax);
                {
                    N(4, SyntaxKind::ElseKeyword);
                    N(4, SyntaxKind::IfStatementSyntax);
                    {
                        N(5, SyntaxKind::IfKeyword);
                        N(5, SyntaxKind::LiteralExpressionSyntax);
                        {
                            N(6, SyntaxKind::FalseKeyword);
                        }
                        N(5, SyntaxKind::BlockStatementSyntax);
                        {
                            N(6, SyntaxKind::OpenBraceToken);
                            N(6, SyntaxKind::CloseBraceToken);
                        }
                    }
                }
            }
        }
        N(1, SyntaxKind::CloseBraceToken);
    }
}

TEST_CASE("parser - misplaced 'else' statement - else at start")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

else if false {
}
else {
}

})___"};

    REQUIRE(helper.Entries.size() == 26);
    Validator N{helper};

    N(0, SyntaxKind::BlockStatementSyntax);
    {
        N(1, SyntaxKind::OpenBraceToken);
        N(1, SyntaxKind::SyntaxList);
        {
            N(2, SyntaxKind::IfStatementSyntax);
            {
                N(3, SyntaxKind::IfKeyword, true);
                N(3, SyntaxKind::IdentifierNameSyntax);
                {
                    N(4, SyntaxKind::IdentifierToken, true);
                }
                N(3, SyntaxKind::ExpressionStatementSyntax);
                {
                    N(4, SyntaxKind::IdentifierNameSyntax);
                    {
                        N(5, SyntaxKind::IdentifierToken, true);
                    }
                    N(4, SyntaxKind::SemicolonToken, true);
                }
                N(3, SyntaxKind::ElseClauseSyntax);
                {
                    N(4, SyntaxKind::ElseKeyword);
                    N(4, SyntaxKind::IfStatementSyntax);
                    {
                        N(5, SyntaxKind::IfKeyword);
                        N(5, SyntaxKind::LiteralExpressionSyntax);
                        {
                            N(6, SyntaxKind::FalseKeyword);
                        }
                        N(5, SyntaxKind::BlockStatementSyntax);
                        {
                            N(6, SyntaxKind::OpenBraceToken);
                            N(6, SyntaxKind::CloseBraceToken);
                        }
                        N(5, SyntaxKind::ElseClauseSyntax);
                        {
                            N(6, SyntaxKind::ElseKeyword);
                            N(6, SyntaxKind::BlockStatementSyntax);
                            {
                                N(7, SyntaxKind::OpenBraceToken);
                                N(7, SyntaxKind::CloseBraceToken);
                            }
                        }
                    }
                }
            }
        }
        N(1, SyntaxKind::CloseBraceToken);
    }
}

TEST_CASE("parser - misplaced 'else' statement - additional else at end")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if true {
} else if false {
} else {
} else {
}

})___"};

    REQUIRE(helper.Entries.size() == 38);
    Validator N{helper};

    N(0, SyntaxKind::BlockStatementSyntax);
    {
        N(1, SyntaxKind::OpenBraceToken);
        N(1, SyntaxKind::SyntaxList);
        {
            N(2, SyntaxKind::IfStatementSyntax);
            {
                N(3, SyntaxKind::IfKeyword);
                N(3, SyntaxKind::LiteralExpressionSyntax);
                {
                    N(4, SyntaxKind::TrueKeyword);
                }
                N(3, SyntaxKind::BlockStatementSyntax);
                {
                    N(4, SyntaxKind::OpenBraceToken);
                    N(4, SyntaxKind::CloseBraceToken);
                }
                N(3, SyntaxKind::ElseClauseSyntax);
                {
                    N(4, SyntaxKind::ElseKeyword);
                    N(4, SyntaxKind::IfStatementSyntax);
                    {
                        N(5, SyntaxKind::IfKeyword);
                        N(5, SyntaxKind::LiteralExpressionSyntax);
                        {
                            N(6, SyntaxKind::FalseKeyword);
                        }
                        N(5, SyntaxKind::BlockStatementSyntax);
                        {
                            N(6, SyntaxKind::OpenBraceToken);
                            N(6, SyntaxKind::CloseBraceToken);
                        }
                        N(5, SyntaxKind::ElseClauseSyntax);
                        {
                            N(6, SyntaxKind::ElseKeyword);
                            N(6, SyntaxKind::BlockStatementSyntax);
                            {
                                N(7, SyntaxKind::OpenBraceToken);
                                N(7, SyntaxKind::CloseBraceToken);
                            }
                        }
                    }
                }
            }
            // Misplaced `else`; parsed as `if`
            // TODO: Verify if it should be parsed as unexpected nodes.
            N(2, SyntaxKind::IfStatementSyntax);
            {
                N(3, SyntaxKind::IfKeyword, true);
                N(3, SyntaxKind::IdentifierNameSyntax);
                {
                    N(4, SyntaxKind::IdentifierToken, true);
                }
                N(3, SyntaxKind::ExpressionStatementSyntax);
                {
                    N(4, SyntaxKind::IdentifierNameSyntax);
                    {
                        N(5, SyntaxKind::IdentifierToken, true);
                    }
                    N(4, SyntaxKind::SemicolonToken, true);
                }
                N(3, SyntaxKind::ElseClauseSyntax);
                {
                    N(4, SyntaxKind::ElseKeyword);
                    N(4, SyntaxKind::BlockStatementSyntax);
                    {
                        N(5, SyntaxKind::OpenBraceToken);
                        N(5, SyntaxKind::CloseBraceToken);
                    }
                }
            }
        }
        N(1, SyntaxKind::CloseBraceToken);
    }
}

TEST_CASE("parser - empty compilation unit")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___()___"};

    REQUIRE(helper.Entries.size() == 2);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::EndOfFileToken);
}

TEST_CASE("parser - compilation unit with single namespace")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___(

namespace A
{
}

)___"};

    REQUIRE(helper.Entries.size() == 9);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::NamespaceDeclarationSyntax);
    N(3, SyntaxKind::NamespaceKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::OpenBraceToken);
    N(3, SyntaxKind::CloseBraceToken);
    N(1, SyntaxKind::EndOfFileToken);
}

TEST_CASE("parser - compilation unit with using and namespace")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___(

using A;

namespace B
{
}

)___"};

    REQUIRE(helper.Entries.size() == 15);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::UsingDeclarationSyntax);
    N(3, SyntaxKind::UsingKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::SemicolonToken);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::NamespaceDeclarationSyntax);
    N(3, SyntaxKind::NamespaceKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::OpenBraceToken);
    N(3, SyntaxKind::CloseBraceToken);
    N(1, SyntaxKind::EndOfFileToken);
}

TEST_CASE("parser - compilation unit with using and namespace with members")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___(

using A;

namespace B
{
    public struct C
    {
    }
}

)___"};

    REQUIRE(helper.Entries.size() == 24);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::UsingDeclarationSyntax);
    N(3, SyntaxKind::UsingKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::SemicolonToken);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::NamespaceDeclarationSyntax);
    N(3, SyntaxKind::NamespaceKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::OpenBraceToken);
    N(3, SyntaxKind::SyntaxList);
    N(4, SyntaxKind::StructDeclarationSyntax);
    N(5, SyntaxKind::SyntaxList);
    N(6, SyntaxKind::PublicKeyword);
    N(5, SyntaxKind::StructKeyword);
    N(5, SyntaxKind::IdentifierNameSyntax);
    N(6, SyntaxKind::IdentifierToken);
    N(5, SyntaxKind::OpenBraceToken);
    N(5, SyntaxKind::CloseBraceToken);
    N(3, SyntaxKind::CloseBraceToken);
    N(1, SyntaxKind::EndOfFileToken);
}

TEST_CASE("parser - compilation unit with misplaced using")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___(

public struct A { };

using B;

)___"};

    REQUIRE(helper.Entries.size() == 18);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::UsingDeclarationSyntax);
    N(3, SyntaxKind::UsingKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::SemicolonToken);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::StructDeclarationSyntax);
    N(3, SyntaxKind::SyntaxList);
    N(4, SyntaxKind::PublicKeyword);
    N(3, SyntaxKind::StructKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::OpenBraceToken);
    N(3, SyntaxKind::CloseBraceToken);
    N(3, SyntaxKind::SemicolonToken);
    N(1, SyntaxKind::EndOfFileToken);
}

TEST_CASE("parser - unrecognized member")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___(

public struct A { };

public unknown B;

)___"};

    REQUIRE(helper.Entries.size() == 20);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::StructDeclarationSyntax);
    N(3, SyntaxKind::SyntaxList);
    N(4, SyntaxKind::PublicKeyword);
    N(3, SyntaxKind::StructKeyword);
    N(3, SyntaxKind::IdentifierNameSyntax);
    N(4, SyntaxKind::IdentifierToken);
    N(3, SyntaxKind::OpenBraceToken);
    N(3, SyntaxKind::CloseBraceToken);
    N(3, SyntaxKind::SemicolonToken);
    N(2, SyntaxKind::IncompleteDeclarationSyntax);
    N(3, SyntaxKind::SyntaxList);
    N(4, SyntaxKind::PublicKeyword);
    N(3, SyntaxKind::IdentifierToken);
    N(2, SyntaxKind::IncompleteDeclarationSyntax);
    N(3, SyntaxKind::IdentifierToken);
    N(2, SyntaxKind::IncompleteDeclarationSyntax);
    N(3, SyntaxKind::SemicolonToken);
    N(1, SyntaxKind::EndOfFileToken);
}

TEST_CASE("parser - unrecognized 'return while true' statement")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

return while true;

})___"};


    REQUIRE(helper.Entries.size() == 14);
    Validator N{helper};

    N(0, SyntaxKind::BlockStatementSyntax);
    {
        N(1, SyntaxKind::OpenBraceToken);
        N(1, SyntaxKind::SyntaxList);
        {
            N(2, SyntaxKind::ReturnStatementSyntax);
            {
                N(3, SyntaxKind::ReturnKeyword);
                N(3, SyntaxKind::IdentifierNameSyntax);
                {
                    N(4, SyntaxKind::IdentifierToken, true);
                }
                N(3, SyntaxKind::SemicolonToken, true);
            }
        }
        N(1, SyntaxKind::UnexpectedNodesSyntax);
        {
            N(2, SyntaxKind::SyntaxList);
            {
                N(3, SyntaxKind::WhileKeyword);
                N(3, SyntaxKind::TrueKeyword);
                N(3, SyntaxKind::SemicolonToken);
            }
        }
        N(1, SyntaxKind::CloseBraceToken);
    }
}

TEST_CASE("parser - ")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if while true
{
    call1();
    @(*#%&*@#);
    call2();
}


})___"};


    REQUIRE(helper.Entries.size() == 41);
    Validator N{helper};

    N(0, SyntaxKind::BlockStatementSyntax);
    N.Enter();
    {
        N(1, SyntaxKind::OpenBraceToken);
        N(1, SyntaxKind::SyntaxList);
        N.Enter();
        {
            N(2, SyntaxKind::IfStatementSyntax);
            N.Enter();
            {
                N(3, SyntaxKind::IfKeyword);
                N(3, SyntaxKind::IdentifierNameSyntax);
                N.Enter();
                {
                    N(4, SyntaxKind::IdentifierToken, true);
                }
                N.Leave();
                N(3, SyntaxKind::BlockStatementSyntax);
                N.Enter();
                {
                    N(4, SyntaxKind::UnexpectedNodesSyntax);
                    N.Enter();
                    {
                        N(5, SyntaxKind::SyntaxList);
                        N.Enter();
                        {
                            N(6, SyntaxKind::WhileKeyword);
                            N(6, SyntaxKind::TrueKeyword);
                        }
                        N.Leave();
                    }
                    N.Leave();
                    N(4, SyntaxKind::OpenBraceToken);
                    N(4, SyntaxKind::SyntaxList);
                    N.Enter();
                    {
                        N(5, SyntaxKind::ExpressionStatementSyntax);
                        N.Enter();
                        {
                            N(6, SyntaxKind::InvocationExpressionSyntax);
                            N.Enter();
                            {
                                N(7, SyntaxKind::IdentifierNameSyntax);
                                N.Enter();
                                {
                                    N(8, SyntaxKind::IdentifierToken);
                                }
                                N.Leave();
                                N(7, SyntaxKind::ArgumentListSyntax);
                                N.Enter();
                                {
                                    N(8, SyntaxKind::OpenParenToken);
                                    N(8, SyntaxKind::CloseParenToken);
                                }
                                N.Leave();
                            }
                            N.Leave();
                            N(6, SyntaxKind::SemicolonToken);
                        }
                        N.Leave();
                    }
                    N.Leave();
                    N(4, SyntaxKind::UnexpectedNodesSyntax);
                    N.Enter();
                    {
                        N(5, SyntaxKind::SyntaxList);
                        N.Enter();
                        {
                            N(6, SyntaxKind::AtToken);
                            N(6, SyntaxKind::OpenParenToken);
                            N(6, SyntaxKind::AsteriskToken);
                            N(6, SyntaxKind::HashToken);
                            N(6, SyntaxKind::PercentToken);
                            N(6, SyntaxKind::AmpersandToken);
                            N(6, SyntaxKind::AsteriskToken);
                            N(6, SyntaxKind::AtToken);
                            N(6, SyntaxKind::HashToken);
                            N(6, SyntaxKind::CloseParenToken);
                            N(6, SyntaxKind::SemicolonToken);
                            N(6, SyntaxKind::IdentifierToken);
                            N(6, SyntaxKind::OpenParenToken);
                            N(6, SyntaxKind::CloseParenToken);
                            N(6, SyntaxKind::SemicolonToken);
                        }
                        N.Leave();
                    }
                    N(4, SyntaxKind::CloseBraceToken);
                    N.Leave();
                }
                N.Leave();
            }
            N.Leave();
        }
        N.Leave();
        N(1, SyntaxKind::CloseBraceToken);
    }
}

TEST_CASE("parser - balanced token sequence")
{
    using namespace weave::syntax;

    SECTION("valid - empty sequence")
    {
        ParserHelper helper{
            "()"};

        REQUIRE(helper.Entries.size() == 3);

        Validator N{helper};

        N(SyntaxKind::BalancedTokenSequneceSyntax);
        N.Enter();
        {
            N(SyntaxKind::OpenParenToken);
            N(SyntaxKind::CloseParenToken);
        }
        N.Leave();
    }

    SECTION("valid - premature")
    {
        ParserHelper helper{
            "((())"};

        REQUIRE(helper.Entries.size() == 8);
        Validator N{helper};

        N(SyntaxKind::BalancedTokenSequneceSyntax);
        N.Enter();
        {
            N(SyntaxKind::OpenParenToken);
            N(SyntaxKind::SyntaxList);
            N.Enter();
            {
                N(SyntaxKind::OpenParenToken);
                N(SyntaxKind::OpenParenToken);
                N(SyntaxKind::CloseParenToken);
                N(SyntaxKind::CloseParenToken);
            }
            N.Leave();
            N(SyntaxKind::CloseParenToken, true);
        }
        N.Leave();
    }

    SECTION("valid - balanced")
    {
        ParserHelper helper{
            "())"};

        REQUIRE(helper.Entries.size() == 3);
        Validator N{helper};

        N(SyntaxKind::BalancedTokenSequneceSyntax);
        N.Enter();
        {
            N(SyntaxKind::OpenParenToken);
            N(SyntaxKind::CloseParenToken);
        }
        N.Leave();
    }

    SECTION("invalid - premature end of file")
    {
        ParserHelper helper{
            "("};

        REQUIRE(helper.Entries.size() == 3);
    }

    SECTION("invalid - unbalanced")
    {
        ParserHelper helper{
            "(aaa'b'{cccc]420_69))"};

        REQUIRE(helper.Entries.size() == 12);
        Validator N{helper};


        N(SyntaxKind::BalancedTokenSequneceSyntax);
        N.Enter();
        {
            N(SyntaxKind::OpenParenToken);
            N(SyntaxKind::SyntaxList);
            N.Enter();
            {
                N(SyntaxKind::IdentifierToken);
                N(SyntaxKind::CharacterLiteralToken);
                N(SyntaxKind::OpenBraceToken);
                N(SyntaxKind::IdentifierToken);
            }
            N.Leave();
            N(SyntaxKind::UnexpectedNodesSyntax);
            N.Enter();
            {
                N(SyntaxKind::SyntaxList);
                N.Enter();
                {
                    N(SyntaxKind::CloseBracketToken);
                    N(SyntaxKind::IntegerLiteralToken);
                }
                N.Leave();
            }
            N.Leave();
            N(SyntaxKind::CloseParenToken);
        }
        N.Leave();
    }
}

TEST_CASE("parser - function - arguments")
{
    using namespace weave::syntax;

    SECTION("no arguments")
    {
        ParserHelper helper{
            R"___(

public function A() -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 17);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("one argument")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 25);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("single unexpected comma")
    {
        ParserHelper helper{
            R"___(

public function A(
    ,
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 20);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple unexpected commas")
    {
        ParserHelper helper{
            R"___(

public function A(
    ,,,,
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 23);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("unexpected comma before valid argument")
    {
        ParserHelper helper{
            R"___(

public function A(
    ,
    a: Int
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 23);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("unexpected comma before valid argument with trailing comma")
    {
        ParserHelper helper{
            R"___(

public function A(
    ,
    a: Int
    ,
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 24);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("unexpected trailing comma after valid argument")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int
    ,
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 26);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    b: Float
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 33);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments without comma between")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int
    b: Float
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 32);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments with attributes")
    {
        ParserHelper helper{
            R"___(

public function A(
    #[unused] a: Int,
    #[deprecated] b: Float
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 48);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one without name")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    : Float,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 34);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one without type")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    b: ,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 41);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one without name and type")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    : ,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 33);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one without name, type and comma")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    : 
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 32);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one with only attribute")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    #[unused] ,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 49);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one with attribute and name")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    #[unused] b,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 49);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one with attribute and type")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    #[unused] ,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 41);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one with attribute and type")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    #[unused] : Float,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 49);
        Validator N{helper};

        N(0, SyntaxKind::BlockStatementSyntax);
        N.Enter();
    }

    SECTION("multiple arguments, one with attribute and colon")
    {
        ParserHelper helper{
            R"___(

public function A(
    a: Int,
    #[unused] ,
    c: String
) -> Void;

)___"};

        REQUIRE(helper.Entries.size() == 49);
        Validator N{helper};

        N(SyntaxKind::CompilationUnitSyntax);
        N.Enter();
        {
            N(SyntaxKind::SyntaxList);
            N.Enter();
            {
                N(SyntaxKind::FunctionDeclarationSyntax);
                N.Enter();
                {
                    N(SyntaxKind::SyntaxList);
                    N.Enter();
                    {
                        N(SyntaxKind::PublicKeyword);
                    }
                    N.Leave();
                    N(SyntaxKind::FunctionKeyword);
                    N(SyntaxKind::IdentifierNameSyntax);
                    N.Enter();
                    {
                        N(SyntaxKind::IdentifierToken);
                    }
                    N.Leave();
                    N(SyntaxKind::ParameterListSyntax);
                    N.Enter();
                    {
                        N(SyntaxKind::OpenParenToken);
                        N(SyntaxKind::SyntaxList);
                        N.Enter();
                        {
                            N(SyntaxKind::ParameterSyntax);
                            N.Enter();
                            {
                                N(SyntaxKind::IdentifierNameSyntax);
                                N.Enter();
                                {
                                    N(SyntaxKind::IdentifierToken);
                                }
                                N.Leave();
                                N(SyntaxKind::TypeClauseSyntax);
                                N.Enter();
                                {
                                    N(SyntaxKind::ColonToken);
                                    N(SyntaxKind::IdentifierNameSyntax);
                                    N.Enter();
                                    {
                                        N(SyntaxKind::IdentifierToken);
                                    }
                                    N.Leave();
                                }
                                N.Leave();
                                N(SyntaxKind::CommaToken);
                            }
                            N.Leave();
                            N(SyntaxKind::ParameterSyntax);
                            N.Enter();
                            {
                                N(SyntaxKind::SyntaxList);
                                N.Enter();
                                {
                                    N(SyntaxKind::AttributeListSyntax);
                                    N.Enter();
                                    {
                                        N(SyntaxKind::HashOpenBracketToken);
                                        N(SyntaxKind::SyntaxList);
                                        N.Enter();
                                        {
                                            N(SyntaxKind::AttributeSyntax);
                                            N.Enter();
                                            {
                                                N(SyntaxKind::IdentifierNameSyntax);
                                                N.Enter();
                                                {
                                                    N(SyntaxKind::IdentifierToken);
                                                }
                                                N.Leave();
                                            }
                                            N.Leave();
                                        }
                                        N.Leave();
                                        N(SyntaxKind::CloseBracketToken);
                                    }
                                    N.Leave();
                                }
                                N.Leave();
                                N(SyntaxKind::IdentifierNameSyntax);
                                N.Enter();
                                {
                                    N(SyntaxKind::IdentifierToken);
                                }
                                N.Leave();
                                N(SyntaxKind::TypeClauseSyntax);
                                N.Enter();
                                {
                                    N(SyntaxKind::ColonToken);
                                    N(SyntaxKind::IdentifierNameSyntax);
                                    N.Enter();
                                    {
                                        N(SyntaxKind::IdentifierToken, true);
                                    }
                                    N.Leave();
                                }
                                N.Leave();
                                N(SyntaxKind::CommaToken);
                            }
                            N.Leave();
                            N(SyntaxKind::ParameterSyntax);
                            N.Enter();
                            {
                                N(SyntaxKind::IdentifierNameSyntax);
                                N.Enter();
                                {
                                    N(SyntaxKind::IdentifierToken);
                                }
                                N.Leave();
                                N(SyntaxKind::TypeClauseSyntax);
                                N.Enter();
                                {
                                    N(SyntaxKind::ColonToken);
                                    N(SyntaxKind::IdentifierNameSyntax);
                                    N.Enter();
                                    {
                                        N(SyntaxKind::IdentifierToken);
                                    }
                                    N.Leave();
                                }
                                N.Leave();
                            }
                            N.Leave();
                        }
                        N.Leave();
                        N(SyntaxKind::CloseParenToken);
                    }
                    N.Leave();
                    N(SyntaxKind::ReturnTypeClauseSyntax);
                    N.Enter();
                    {
                        N(SyntaxKind::MinusGreaterThanToken);
                        N(SyntaxKind::IdentifierNameSyntax);
                        N.Enter();
                        {
                            N(SyntaxKind::IdentifierToken);
                        }
                        N.Leave();
                    }
                    N.Leave();
                    N(SyntaxKind::SemicolonToken);
                }
                N.Leave();
            }
            N.Leave();
            N(SyntaxKind::EndOfFileToken);
            N.Leave();
        }
    }
}
