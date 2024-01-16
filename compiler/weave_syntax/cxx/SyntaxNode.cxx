#include "weave/syntax/SyntaxNode.hxx"

namespace weave::syntax
{
    static_assert(std::is_trivially_destructible_v<SyntaxNode>);
}
