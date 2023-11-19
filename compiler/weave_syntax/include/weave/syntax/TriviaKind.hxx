#pragma once

#include <cstdint>
#include <string_view>

namespace weave::syntax
{
    enum class TriviaKind
    {
#define WEAVE_TRIVIA(name, spelling) name,
#include "weave/syntax/TriviaKind.inl"
    };

    class TriviaKindTraits final
    {
    public:
        [[nodiscard]] static std::string_view GetName(TriviaKind value);

        [[nodiscard]] static std::string_view GetSpellign(TriviaKind value);

        [[nodiscard]] static bool IsDocumentation(TriviaKind value);
    };
}
