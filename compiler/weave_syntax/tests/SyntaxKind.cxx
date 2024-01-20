#include "weave/platform/Compiler.hxx"
#include "weave/syntax/SyntaxKind.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

TEST_CASE("SyntaxKind Tests - IsToken")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(IsToken(SyntaxKind::name));
#define WEAVE_SYNTAX_TOKEN(name, spelling) \
    CHECK(IsToken(SyntaxKind::name));
#define WEAVE_SYNTAX_KEYWORD(name, spelling) \
    CHECK(IsToken(SyntaxKind::name));
#define WEAVE_SYNTAX_CONTEXTUAL_KEYWORD(name, spelling) \
    CHECK(IsToken(SyntaxKind::name));
#define WEAVE_SYNTAX_PUNCTUATION(name, spelling) \
    CHECK(IsToken(SyntaxKind::name));
#include "weave/syntax/SyntaxKind.inl"
}

TEST_CASE("SyntaxKind Tests - IsTrivia")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(IsTrivia(SyntaxKind::name));
#define WEAVE_SYNTAX_TRIVIA(name, spelling) \
    CHECK(IsTrivia(SyntaxKind::name));
#include "weave/syntax/SyntaxKind.inl"
}

TEST_CASE("SyntaxKind Tests - IsPunctuation")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(IsPunctuation(SyntaxKind::name));
#define WEAVE_SYNTAX_PUNCTUATION(name, spelling) \
    CHECK(IsPunctuation(SyntaxKind::name));
#include "weave/syntax/SyntaxKind.inl"
}

TEST_CASE("SyntaxKind Tests - IsKeyword")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(IsKeyword(SyntaxKind::name));
#define WEAVE_SYNTAX_KEYWORD(name, spelling) \
    CHECK(IsKeyword(SyntaxKind::name));
#include "weave/syntax/SyntaxKind.inl"
}

TEST_CASE("SyntaxKind Tests - IsContextualKeyword")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(IsContextualKeyword(SyntaxKind::name));
#define WEAVE_SYNTAX_CONTEXTUAL_KEYWORD(name, spelling) \
    CHECK(IsContextualKeyword(SyntaxKind::name));
#include "weave/syntax/SyntaxKind.inl"
}

TEST_CASE("SyntaxKind Tests - IsNode")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(IsNode(SyntaxKind::name));
#define WEAVE_SYNTAX_NODE(name, spelling) \
    CHECK(IsNode(SyntaxKind::name));
#include "weave/syntax/SyntaxKind.inl"
}


TEST_CASE("SyntaxKind Tests - IsExpression")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(IsExpression(SyntaxKind::name));
#define WEAVE_SYNTAX_EXPRESSION(name, spelling) \
    CHECK(IsExpression(SyntaxKind::name));
#include "weave/syntax/SyntaxKind.inl"
}


TEST_CASE("SyntaxKind Tests - GetName")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK(GetName(SyntaxKind::name) == #name);
#include "weave/syntax/SyntaxKind.inl"
}

TEST_CASE("SyntaxKind Tests - GetSpelling")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK(GetSpelling(SyntaxKind::name) == spelling);
#include "weave/syntax/SyntaxKind.inl"
}

TEST_CASE("SyntaxKind Tests - TryMapIdentifierToKeyword")
{
    using namespace weave::syntax;
#define WEAVE_SYNTAX(name, spelling) \
    CHECK_FALSE(TryMapIdentifierToKeyword(spelling).has_value());
#define WEAVE_SYNTAX_KEYWORD(name, spelling) \
    CHECK(TryMapIdentifierToKeyword(spelling).value_or(SyntaxKind::None) == SyntaxKind::name);
#define WEAVE_SYNTAX_CONTEXTUAL_KEYWORD(name, spelling) \
    CHECK(TryMapIdentifierToKeyword(spelling).value_or(SyntaxKind::None) == SyntaxKind::name);
#include "weave/syntax/SyntaxKind.inl"
}
