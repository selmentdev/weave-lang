#pragma once

#include <cstdint>
#include <string_view>

namespace Weave::Syntax
{
    enum class TriviaKind
    {
#define WEAVE_TRIVIA(name, spelling) name,
#include "Weave.Syntax/TriviaKind.inl"
    };

    class TriviaKindTraits final
    {
        [[nodiscard]] static std::string_view GetName(TriviaKind value);

        [[nodiscard]] static std::string_view GetSpellign(TriviaKind value);

        [[nodiscard]] static bool IsDocumentation(TriviaKind value);
    };
}
