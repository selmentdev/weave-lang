#include "weave/syntax/SyntaxKind.hxx"
#include "weave/hash/Fnv1a.hxx"
#include "weave/bugcheck/Assert.hxx"

#include <utility>
#include <array>
#include <algorithm>

namespace weave::syntax
{
    std::string_view GetName(SyntaxKind kind)
    {
        constexpr auto lookup = std::array{
#define WEAVE_SYNTAX(name, spelling) std::string_view{#name},
#include "weave/syntax/SyntaxKind.inl"
        };

        size_t const index = std::to_underlying(kind);
        WEAVE_ASSERT(index < lookup.size());
        return lookup[index];
    }

    std::string_view GetSpelling(SyntaxKind kind)
    {
        constexpr auto lookup = std::array{
#define WEAVE_SYNTAX(name, spelling) std::string_view{spelling},
#include "weave/syntax/SyntaxKind.inl"
        };

        size_t const index = std::to_underlying(kind);
        WEAVE_ASSERT(index < lookup.size());
        return lookup[index];
    }

    bool IsToken(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_TOKEN(name, spelling) case SyntaxKind::name:
#define WEAVE_SYNTAX_PUNCTUATION(name, spelling) case SyntaxKind::name:
#define WEAVE_SYNTAX_CONTEXTUAL_KEYWORD(name, spelling) case SyntaxKind::name:
#define WEAVE_SYNTAX_KEYWORD(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool IsTrivia(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_TRIVIA(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool IsDocumentationTrivia(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
        case SyntaxKind::MultiLineDocumentationTrivia:
        case SyntaxKind::SingleLineDocumentationTrivia:
            return true;

        default:
            break;
        }

        return false;
    }

    bool IsPunctuation(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_PUNCTUATION(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool IsKeyword(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_KEYWORD(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool IsContextualKeyword(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_CONTEXTUAL_KEYWORD(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool IsNode(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_NODE(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    bool IsExpression(SyntaxKind kind)
    {
        switch (kind) // NOLINT(clang-diagnostic-switch-enum)
        {
#define WEAVE_SYNTAX_EXPRESSION(name, spelling) case SyntaxKind::name:
#include "weave/syntax/SyntaxKind.inl"
            return true;

        default:
            break;
        }

        return false;
    }

    struct KeywordMappingEntry
    {
        std::string_view Spelling;
        SyntaxKind Kind;
        uint64_t Hash;
    };

    constexpr auto c_KeywordLookup = []()  // NOLINT(fuchsia-statically-constructed-objects)
    {
        auto result = std::array{
#define WEAVE_SYNTAX_CONTEXTUAL_KEYWORD(name, spelling) KeywordMappingEntry{spelling, SyntaxKind::name, hash::Fnv1a64::FromString(spelling)},
#define WEAVE_SYNTAX_KEYWORD(name, spelling) KeywordMappingEntry{spelling, SyntaxKind::name, hash::Fnv1a64::FromString(spelling)},
#include "weave/syntax/SyntaxKind.inl"
        };

        std::sort(result.begin(), result.end(), [](KeywordMappingEntry const& lhs, KeywordMappingEntry const& rhs)
            {
                if (lhs.Hash == rhs.Hash)
                {
                    if (lhs.Spelling.size() == rhs.Spelling.size())
                    {
                        return lhs.Spelling < rhs.Spelling;
                    }

                    return lhs.Spelling.size() < rhs.Spelling.size();
                }

                return lhs.Hash < rhs.Hash;
            });

        return result;
    }();

    std::optional<SyntaxKind> TryMapIdentifierToKeyword(std::string_view value)
    {
        uint64_t const hash = hash::Fnv1a64::FromString(value);

        auto const first = c_KeywordLookup.begin();
        auto const last = c_KeywordLookup.end();

        auto it = std::lower_bound(
            first,
            last,
            hash,
            [&](KeywordMappingEntry const& e, uint64_t h)
            {
                return e.Hash < h;
            });

        while ((it != last) and (it->Hash == hash))
        {
            if (it->Spelling == value)
            {
                return it->Kind;
            }
            ++it;
        }

        return {};
    }

}
