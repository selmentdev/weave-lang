#include "weave/syntax/SyntaxNode.hxx"

namespace weave::syntax
{
    static_assert(std::is_trivially_destructible_v<SyntaxNode>);
}

#include "weave/syntax2/SyntaxNode.hxx"

namespace weave::syntax2
{
    static_assert(sizeof(SyntaxNode) == 12);
    static_assert(std::is_trivially_destructible_v<SyntaxNode>);
}
