#include "weave/platform/Compiler.hxx"
#include "weave/syntax2/SyntaxKind.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

TEST_CASE("SyntaxKind Tests")
{
    using namespace weave::syntax2;

    SECTION("SyntaxKindTraits::IsTrivia")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK_FALSE(SyntaxKindTraits::IsTrivia(SyntaxKind::name));
#define WEAVE_SYNTAX_TRIVIA(name, value, spelling) \
    CHECK(SyntaxKindTraits::IsTrivia(SyntaxKind::name));
#include "weave/syntax2/SyntaxKind.inl"
    }

    SECTION("SyntaxKindTraits::IsToken")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK_FALSE(SyntaxKindTraits::IsToken(SyntaxKind::name));
#define WEAVE_SYNTAX_TOKEN(name, value, spelling) \
    CHECK(SyntaxKindTraits::IsToken(SyntaxKind::name));
#include "weave/syntax2/SyntaxKind.inl"
    }

    SECTION("SyntaxKindTraits::IsBaseKeyword")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK_FALSE(SyntaxKindTraits::IsBaseKeyword(SyntaxKind::name));
#define WEAVE_SYNTAX_KEYWORD(name, value, spelling) \
    CHECK(SyntaxKindTraits::IsBaseKeyword(SyntaxKind::name));
#include "weave/syntax2/SyntaxKind.inl"
    }

    SECTION("SyntaxKindTraits::IsTypeKeyword")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK_FALSE(SyntaxKindTraits::IsTypeKeyword(SyntaxKind::name));
#define WEAVE_SYNTAX_TYPE_KEYWORD(name, value, spelling) \
    CHECK(SyntaxKindTraits::IsTypeKeyword(SyntaxKind::name));
#include "weave/syntax2/SyntaxKind.inl"
    }

    SECTION("SyntaxKindTraits::IsKeyword")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK_FALSE(SyntaxKindTraits::IsKeyword(SyntaxKind::name));
#define WEAVE_SYNTAX_TYPE_KEYWORD(name, value, spelling) \
    CHECK(SyntaxKindTraits::IsKeyword(SyntaxKind::name));
#define WEAVE_SYNTAX_KEYWORD(name, value, spelling) \
    CHECK(SyntaxKindTraits::IsKeyword(SyntaxKind::name));
#include "weave/syntax2/SyntaxKind.inl"
    }

    SECTION("SyntaxKindTraits::GetName")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK(SyntaxKindTraits::GetName(SyntaxKind::name) == #name);
#include "weave/syntax2/SyntaxKind.inl"
    }

    SECTION("SyntaxKindTraits::GetSpelling")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK(SyntaxKindTraits::GetSpelling(SyntaxKind::name) == spelling);
#include "weave/syntax2/SyntaxKind.inl"
    }

    SECTION("SyntaxKindTraits::TryMapIdentifierToKeyword")
    {
#define WEAVE_SYNTAX(name, value, spelling) \
    CHECK_FALSE(SyntaxKindTraits::TryMapIdentifierToKeyword(spelling).has_value());
#define WEAVE_SYNTAX_KEYWORD(name, value, spelling) \
    CHECK(SyntaxKindTraits::TryMapIdentifierToKeyword(spelling).value_or(SyntaxKind::None) == SyntaxKind::name);
#include "weave/syntax2/SyntaxKind.inl"
    }
}
