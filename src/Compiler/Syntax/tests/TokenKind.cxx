#include "Weave.Core/Platform/Compiler.hxx"
#include "Weave.Syntax/TokenKind.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

TEST_CASE("Token Kind tests")
{
    using namespace Weave::Syntax;

    SECTION("Tokens")
    {
#define WEAVE_TOKEN_KEYWORD(name, spelling) REQUIRE(TokenKindTraits::TryMapIdentifierToKeyword(spelling).value() == TokenKind::name);
#define WEAVE_TOKEN(name, spelling) REQUIRE_FALSE(TokenKindTraits::TryMapIdentifierToKeyword(spelling).has_value());
#include "Weave.Syntax/TokenKind.inl"
    }

    SECTION("Non-Tokens")
    {
    }
}

