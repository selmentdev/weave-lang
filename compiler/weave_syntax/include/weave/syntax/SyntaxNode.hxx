#pragma once
#include "weave/syntax/SyntaxKind.hxx"
#include "weave/tokenizer/Token.hxx"

namespace weave::syntax
{
    class SyntaxNode
    {
    private:
        SyntaxKind _kind;

    public:
        explicit constexpr SyntaxNode(SyntaxKind kind)
            : _kind{kind}
        {
        }

    public:
        [[nodiscard]] constexpr SyntaxKind Kind() const
        {
            return this->_kind;
        }

        [[nodiscard]] constexpr bool Is(SyntaxKind kind) const
        {
            return this->_kind == kind;
        }

        template <typename T>
        [[nodiscard]] constexpr T* As() const
        {
            return T::ClassOf(this) ? static_cast<T*>(this) : nullptr;
        }
    };
}

// Forward declare all concrete syntax nodes.
namespace weave::syntax
{
#define WEAVE_SYNTAX_NODE(name, spelling) \
    class name;
#include "weave/syntax/SyntaxKind.inl"
}
