#pragma once
#include "weave/syntax2/SyntaxKind.hxx"
#include "weave/source/Source.hxx"

namespace weave::syntax2
{
    struct SyntaxNode
    {
        SyntaxKind Kind;

        [[nodiscard]] constexpr bool Is(SyntaxKind kind) const
        {
            return this->Kind == kind;
        }

        template <typename T>
        [[nodiscard]] constexpr T const* TryCast() const
        {
            return T::ClassOf(this) ? static_cast<T const*>(this) : nullptr;
        }

        template <typename T>
        [[nodiscard]] constexpr T* TryCast()
        {
            return T::ClassOf(this) ? static_cast<T*>(this) : nullptr;
        }
    };
}


// Forward declare all syntax nodes

namespace weave::syntax2
{
#define WEAVE_SYNTAX_NODE(name, value, spelling) class name;
#include "weave/syntax2/SyntaxKind.inl"
}
