#include "weave/syntax/SyntaxKind.hxx"
#include "weave/bugcheck/Assert.hxx"

#include <utility>
#include <array>
#include <algorithm>

namespace weave::syntax
{
    std::string_view SyntaxKindTraits::GetName(SyntaxKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_SYNTAX_NODE(name, spelling) #name,
#include "weave/syntax/SyntaxKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    std::string_view SyntaxKindTraits::GetSpelling(SyntaxKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_SYNTAX_NODE(name, spelling) spelling,
#include "weave/syntax/SyntaxKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    bool SyntaxKindTraits::IsAbstract(SyntaxKind value)
    {
        switch (value) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_ABSTRACT_NODE(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxKindTraits::IsDeclaration(SyntaxKind value)
    {
        switch (value) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_DECLARATION(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxKindTraits::IsExpression(SyntaxKind value)
    {
        switch (value) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_EXPRESSION(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool SyntaxKindTraits::IsStatement(SyntaxKind value)
    {
        switch (value) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_STATEMENT(name ,spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }
}
