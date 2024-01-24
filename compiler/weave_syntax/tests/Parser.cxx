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
            this->_entries.emplace_back(this->Depth, node->Kind);
            SyntaxWalker::OnDefault(node);
        }
    };

public:
    template <typename ActionT = weave::syntax::SyntaxNode*(weave::syntax::Parser&)>
    ParserHelper(std::string_view source, ActionT&& action)
        : Text{std::string{source}}
    {
        weave::syntax::Parser parser{&this->Diagnostic, &this->Factory, this->Text};
        Walker walker{this->Entries};

        this->Root = action(parser);
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

    void operator()(size_t depth, weave::syntax::SyntaxKind kind)
    {
        CAPTURE(this->_index);
        REQUIRE(this->_index < this->_helper.Entries.size());
        CHECK(this->_helper.Entries[this->_index].Kind == kind);
        CHECK(this->_helper.Entries[this->_index].Depth == depth);
        ++this->_index;
    }

    void operator()(weave::syntax::SyntaxKind kind)
    {
        CAPTURE(this->_index);
        REQUIRE(this->_index < this->_helper.Entries.size());
        CHECK(this->_helper.Entries[this->_index].Kind == kind);
        CHECK(this->_helper.Entries[this->_index].Depth == this->_depth);
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

TEST_CASE("parser - valid 'if' statement with parenthesized expression")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if (true) { }

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};

    REQUIRE(helper.Entries.size() == 14);
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
                N(SyntaxKind::ParenthesizedExpressionSyntax);
                N.Enter();
                {
                    N(SyntaxKind::OpenParenToken);
                    N(SyntaxKind::LiteralExpressionSyntax);
                    N.Enter();
                    {
                        N(SyntaxKind::TrueKeyword);
                    }
                    N.Leave();
                    N(SyntaxKind::CloseParenToken);
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

TEST_CASE("parser - valid 'if' statement with true")
{
    using namespace weave::syntax;
    ParserHelper helper{
        R"___({

if true { }

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};

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

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};

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

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};

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

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};

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
                N(3, SyntaxKind::IfKeyword);
                N(3, SyntaxKind::IdentifierNameSyntax);
                {
                    N(4, SyntaxKind::IdentifierToken);
                }
                N(3, SyntaxKind::ExpressionStatementSyntax);
                {
                    N(4, SyntaxKind::IdentifierNameSyntax);
                    {
                        N(5, SyntaxKind::IdentifierToken);
                    }
                    N(4, SyntaxKind::SemicolonToken);
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

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};

    REQUIRE(helper.Entries.size() == 26);
    Validator N{helper};

    N(0, SyntaxKind::BlockStatementSyntax);
    {
        N(1, SyntaxKind::OpenBraceToken);
        N(1, SyntaxKind::SyntaxList);
        {
            N(2, SyntaxKind::IfStatementSyntax);
            {
                N(3, SyntaxKind::IfKeyword);
                N(3, SyntaxKind::IdentifierNameSyntax);
                {
                    N(4, SyntaxKind::IdentifierToken);
                }
                N(3, SyntaxKind::ExpressionStatementSyntax);
                {
                    N(4, SyntaxKind::IdentifierNameSyntax);
                    {
                        N(5, SyntaxKind::IdentifierToken);
                    }
                    N(4, SyntaxKind::SemicolonToken);
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

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};

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
                N(3, SyntaxKind::IfKeyword);
                N(3, SyntaxKind::IdentifierNameSyntax);
                {
                    N(4, SyntaxKind::IdentifierToken);
                }
                N(3, SyntaxKind::ExpressionStatementSyntax);
                {
                    N(4, SyntaxKind::IdentifierNameSyntax);
                    {
                        N(5, SyntaxKind::IdentifierToken);
                    }
                    N(4, SyntaxKind::SemicolonToken);
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
        R"___()___",
        [](Parser& parser)
        {
            return parser.ParseCompilationUnit();
        }};

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

)___",
        [](Parser& parser)
        {
            return parser.ParseCompilationUnit();
        }};

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

)___",
        [](Parser& parser)
        {
            return parser.ParseCompilationUnit();
        }};

    REQUIRE(helper.Entries.size() == 15);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::UsingDirectiveSyntax);
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

)___",
        [](Parser& parser)
        {
            return parser.ParseCompilationUnit();
        }};

    REQUIRE(helper.Entries.size() == 24);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::UsingDirectiveSyntax);
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

)___",
        [](Parser& parser)
        {
            return parser.ParseCompilationUnit();
        }};

    REQUIRE(helper.Entries.size() == 18);
    Validator N{helper};

    N(0, SyntaxKind::CompilationUnitSyntax);
    N(1, SyntaxKind::SyntaxList);
    N(2, SyntaxKind::UsingDirectiveSyntax);
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

)___",
        [](Parser& parser)
        {
            return parser.ParseCompilationUnit();
        }};

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

})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};


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
                    N(4, SyntaxKind::IdentifierToken);
                }
                N(3, SyntaxKind::SemicolonToken);
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


})___",
        [](Parser& parser)
        {
            return parser.ParseBlockStatement();
        }};


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
                    N(4, SyntaxKind::IdentifierToken);
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

    std::vector<SyntaxToken*> tokens{};
    std::vector<SyntaxNode*> unexpected{};

    SECTION("valid - empty sequence")
    {
        ParserHelper helper{
            ")",
            [&](Parser& parser)
            {
                return parser.MatchBalancedTokenSequence(SyntaxKind::CloseParenToken, tokens, unexpected);
            }};

        REQUIRE(tokens.size() == 0);
        REQUIRE(unexpected.size() == 0);
    }

    SECTION("valid - premature")
    {
        ParserHelper helper{
            "))",
            [&](Parser& parser)
            {
                return parser.MatchBalancedTokenSequence(SyntaxKind::CloseParenToken, tokens, unexpected);
            }};

        REQUIRE(tokens.size() == 0);
        REQUIRE(unexpected.size() == 0);
    }

    SECTION("valid - balanced")
    {
        ParserHelper helper{
            "())",
            [&](Parser& parser)
            {
                return parser.MatchBalancedTokenSequence(SyntaxKind::CloseParenToken, tokens, unexpected);
            }};

        REQUIRE(tokens.size() == 0);
        REQUIRE(unexpected.size() == 0);
    }

    SECTION("invalid - premature end of file")
    {
        ParserHelper helper{
            "",
            [&](Parser& parser)
            {
                return parser.MatchBalancedTokenSequence(SyntaxKind::CloseParenToken, tokens, unexpected);
            }};

        REQUIRE(tokens.size() == 0);
        REQUIRE(unexpected.size() == 0);
    }

    SECTION("invalid - unbalanced")
    {
        ParserHelper helper{
            "aaa'b'{cccc]420_69)",
            [&](Parser& parser)
            {
                return parser.MatchBalancedTokenSequence(SyntaxKind::CloseParenToken, tokens, unexpected);
            }};

        REQUIRE(tokens.size() == 5);
        REQUIRE(unexpected.size() == 4);
    }
}
