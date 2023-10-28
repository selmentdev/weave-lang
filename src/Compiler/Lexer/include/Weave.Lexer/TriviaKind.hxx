#pragma once
#include <cstdint>
#include <string_view>

namespace Weave::Lexer
{
    enum class TriviaKind : uint32_t
    {
#define WEAVE_TRIVIA(name, spelling) name,
#include "Weave.Lexer/TriviaKind.inc"
    };

    class TriviaKindTraits final
    {
    public:
        [[nodiscard]] static std::string_view GetName(TriviaKind value);

        [[nodiscard]] static std::string_view GetSpelling(TriviaKind value);

        [[nodiscard]] static bool IsDocumentation(TriviaKind value);
    };
}
