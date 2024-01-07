#pragma once
#include "weave/syntax2/SyntaxNode.hxx"
#include "weave/bitwise/Flag.hxx"

#include <span>

namespace weave::syntax2
{
    struct SyntaxTrivia final
    {
        SyntaxKind Kind;
        source::SourceSpan Source;
    };

    struct SyntaxTriviaRange
    {
        std::span<SyntaxTrivia const> Leading{};
        std::span<SyntaxTrivia const> Trailing{};
    };

    enum class SyntaxTokenFlags : uint32_t
    {
        None = 0u,
        Missing = 1u << 0u,
    };

    struct SyntaxToken : SyntaxNode
    {
        bitwise::Flags<SyntaxTokenFlags> Flags{};
        SyntaxTriviaRange const* Trivia;

        constexpr SyntaxToken(SyntaxKind kind, source::SourceSpan source, SyntaxTriviaRange const* trivia)
            : SyntaxNode{kind, source}
            , Trivia{trivia}
        {
        }

        constexpr SyntaxToken(SyntaxKind kind, source::SourceSpan source, SyntaxTriviaRange const* trivia, bitwise::Flags<SyntaxTokenFlags> flags)
            : SyntaxNode{kind, source}
            , Flags{flags}
            , Trivia{trivia}
        {
        }

        [[nodiscard]] constexpr std::span<SyntaxTrivia const> GetLeadingTrivia() const
        {
            if (this->Trivia != nullptr)
            {
                return this->Trivia->Leading;
            }

            return {};
        }

        [[nodiscard]] constexpr std::span<SyntaxTrivia const> GetTrailingTrivia() const
        {
            if (this->Trivia != nullptr)
            {
                return this->Trivia->Trailing;
            }

            return {};
        }

        [[nodiscard]] constexpr bool IsMissing() const
        {
            return this->Flags.Any(SyntaxTokenFlags::Missing);
        }
    };
}
