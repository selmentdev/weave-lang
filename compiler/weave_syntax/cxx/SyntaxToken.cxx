#include "weave/syntax2/SyntaxToken.hxx"

namespace weave::syntax2
{
    static_assert(sizeof(SyntaxTrivia) == 12);
    static_assert(std::is_trivially_destructible_v<SyntaxTrivia>);

    static_assert(sizeof(SyntaxTriviaRange) == (4 * sizeof(void*)));
    static_assert(std::is_trivially_destructible_v<SyntaxTriviaRange>);

    static_assert(sizeof(SyntaxToken) == 24);
    static_assert(std::is_trivially_destructible_v<SyntaxToken>);
}
