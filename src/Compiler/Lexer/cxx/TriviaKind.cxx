#include "Weave.Lexer/TriviaKind.hxx"

#include <utility>
#include <cassert>

namespace Weave::Lexer
{
    std::string_view TriviaKindTraits::GetName(TriviaKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_TRIVIA(name, spelling) #name,
#include "Weave.Lexer/TriviaKind.inc"
        };

        size_t const index = std::to_underlying(value);
        assert(index < std::size(lookup));
        return lookup[index];
    }

    std::string_view TriviaKindTraits::GetSpelling(TriviaKind value)
    {
        static constexpr std::string_view lookup[]{
#define WEAVE_TRIVIA(name, spelling) spelling,
#include "Weave.Lexer/TriviaKind.inc"
        };

        size_t const index = std::to_underlying(value);
        assert(index < std::size(lookup));
        return lookup[index];
    }

    bool TriviaKindTraits::IsDocumentation(TriviaKind value)
    {
        return (value == TriviaKind::MultiLineDocumentation) or (value == TriviaKind::SingleLineDocumentation);
    }
}
