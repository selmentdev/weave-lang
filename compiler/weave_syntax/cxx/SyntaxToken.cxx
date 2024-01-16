#include "weave/syntax/SyntaxToken.hxx"
#include "weave/hash/Fnv1a.hxx"

namespace weave::syntax
{
    static_assert(sizeof(SyntaxTrivia) == 12);
    static_assert(std::is_trivially_destructible_v<SyntaxTrivia>);

    static_assert(sizeof(SyntaxTriviaRange) == 32);
    static_assert(std::is_trivially_destructible_v<SyntaxTriviaRange>);

    static_assert(sizeof(SyntaxToken) == 24);
    static_assert(std::is_trivially_destructible_v<SyntaxToken>);

    static_assert(sizeof(IntegerLiteralSyntaxToken) == 64);
    static_assert(std::is_trivially_destructible_v<IntegerLiteralSyntaxToken>);

    static_assert(sizeof(FloatLiteralSyntaxToken) == 64);
    static_assert(std::is_trivially_destructible_v<FloatLiteralSyntaxToken>);

    static_assert(sizeof(StringLiteralSyntaxToken) == 48);
    static_assert(std::is_trivially_destructible_v<StringLiteralSyntaxToken>);

    static_assert(sizeof(CharacterLiteralSyntaxToken) == 32);
    static_assert(std::is_trivially_destructible_v<CharacterLiteralSyntaxToken>);

    static_assert(sizeof(IdentifierSyntaxToken) == 40);
    static_assert(std::is_trivially_destructible_v<IdentifierSyntaxToken>);
}
