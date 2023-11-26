#include "weave/lexer/TriviaKind.hxx"
#include "weave/BugCheck.hxx"

#include <utility>

namespace weave::lexer
{
    std::string_view TriviaKindTraits::GetName(TriviaKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_TRIVIA(name, spelling) #name,
#include "weave/lexer/TriviaKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];
    }

    std::string_view TriviaKindTraits::GetSpellign(TriviaKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_TRIVIA(name, spelling) spelling,
#include "weave/lexer/TriviaKind.inl"
        };

        size_t const index = std::to_underlying(value);
        WEAVE_ASSERT(index < std::size(lookup));
        return lookup[index];        
    }

    bool TriviaKindTraits::IsDocumentation(TriviaKind value)
    {
        return (value == TriviaKind::MultiLineDocumentation) or (value == TriviaKind::SingleLineDocumentation);
    }
}
