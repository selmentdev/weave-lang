#include "weave/syntax2/SyntaxKind.hxx"
#include "weave/hash/Fnv1a.hxx"
#include "weave/bugcheck/Assert.hxx"

#include <utility>
#include <array>
#include <algorithm>

// Implements SyntaxKind to Name mapping
namespace weave::syntax2
{
#define WEAVE_SYNTAX_BEGIN_GROUP(name, value)   constexpr std::string_view name##_NameLookup[] {
#define WEAVE_SYNTAX(name, value, spelling)         #name,
#define WEAVE_SYNTAX_END_GROUP(name, value)     };
#include "weave/syntax2/SyntaxKind.inl"

    std::string_view SyntaxKindTraits::GetName(SyntaxKind value)
    {
        auto const as_integer = std::to_underlying(value);

#define WEAVE_SYNTAX_BEGIN_GROUP(name, value)   { \
    if ((SyntaxKindTraits::name##_First <= as_integer) and (as_integer <= SyntaxKindTraits::name##_Last)) \
    { \
        return name##_NameLookup[as_integer - SyntaxKindTraits::name##_First]; \
    } \
}
#include "weave/syntax2/SyntaxKind.inl"

        return "<unknown>";
    }
}

// Implements SyntaxKind to Spelling mapping
namespace weave::syntax2
{
#define WEAVE_SYNTAX_BEGIN_GROUP(name, value)   constexpr std::string_view name##_SpellingLookup[] {
#define WEAVE_SYNTAX(name, value, spelling)         spelling,
#define WEAVE_SYNTAX_END_GROUP(name, value)     };
#include "weave/syntax2/SyntaxKind.inl"

    std::string_view SyntaxKindTraits::GetSpelling(SyntaxKind value)
    {
        auto const as_integer = std::to_underlying(value);

#define WEAVE_SYNTAX_BEGIN_GROUP(name, value)   { \
    if ((SyntaxKindTraits::name##_First <= as_integer) and (as_integer <= SyntaxKindTraits::name##_Last)) \
    { \
        return name##_SpellingLookup[as_integer - SyntaxKindTraits::name##_First]; \
    } \
}
#include "weave/syntax2/SyntaxKind.inl"

        return "<unknown>";
    }
}

namespace weave::syntax2
{
    struct KeywordMappingEntry
    {
        std::string_view Spelling;
        SyntaxKind Kind;
        uint64_t Hash;
    };

    struct KeywordMapping
    {
        std::array<KeywordMappingEntry, SyntaxKindTraits::Keyword_Count> Entries{{
#define WEAVE_SYNTAX_KEYWORD(name, value, spelling) { spelling, SyntaxKind::name, hash::Fnv1a64::FromString(spelling) },
#include "weave/syntax2/SyntaxKind.inl"
        }};

        constexpr KeywordMapping()
        {
            std::sort(Entries.begin(), Entries.end(), [](KeywordMappingEntry const& lhs, KeywordMappingEntry const& rhs)
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
        }

        [[nodiscard]] constexpr std::optional<SyntaxKind> TryMap(std::string_view value) const
        {
            uint64_t const hash = hash::Fnv1a64::FromString(value);

            auto const end = this->Entries.end();

            auto it = std::lower_bound(
                this->Entries.begin(),
                this->Entries.end(),
                hash,
                [&](KeywordMappingEntry const& e, uint64_t h)
                {
                    return e.Hash < h;
                });

            while ((it != end) and (it->Hash == hash))
            {
                if (it->Spelling == value)
                {
                    return it->Kind;
                }
                ++it;
            }

            return {};
        }
    };

    std::optional<SyntaxKind> SyntaxKindTraits::TryMapIdentifierToKeyword(std::string_view value)
    {
        static constexpr KeywordMapping c_KeywordsMapping{};
        return c_KeywordsMapping.TryMap(value);
    }
}
