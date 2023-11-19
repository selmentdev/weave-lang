#pragma once
#include <compare>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <algorithm>

namespace weave::source
{
    struct LinePosition final
    {
        uint32_t line;
        uint32_t column;

        [[nodiscard]] constexpr auto operator<=>(LinePosition const&) const = default;
    };

    struct LineSpan final
    {
        LinePosition start;
        LinePosition end;

        [[nodiscard]] constexpr auto operator<=>(LineSpan const&) const = default;
    };

    struct SourcePosition final
    {
        uint32_t offset;

        [[nodiscard]] constexpr auto operator<=>(SourcePosition const&) const = default;
    };

    struct SourceSpan final
    {
        SourcePosition start;
        SourcePosition end;

        [[nodiscard]] constexpr auto operator<=>(SourceSpan const&) const = default;

        [[nodiscard]] constexpr bool contains(SourceSpan const& other) const
        {
            return (this->start.offset <= other.start.offset) and (other.end.offset <= this->end.offset);
        }

        [[nodiscard]] constexpr bool contains(SourcePosition const& other) const
        {
            return (this->start.offset <= other.offset) and (other.offset <= this->end.offset);
        }
    };

    [[nodiscard]] constexpr SourceSpan combine(SourceSpan const& s1, SourceSpan const& s2)
    {
        return SourceSpan{
            SourcePosition{
                std::min(s1.start.offset, s2.start.offset),
            },
            SourcePosition{
                std::max(s1.end.offset, s2.end.offset),
            },
        };
    }

    [[nodiscard]] constexpr SourceSpan combine(SourceSpan const& s, SourcePosition const& p)
    {
        return SourceSpan{
            SourcePosition{
                std::min(s.start.offset, p.offset),
            },
            SourcePosition{
                std::max(s.end.offset, p.offset),
            },
        };
    }

    [[nodiscard]] constexpr SourceSpan combine(SourceSpan const& s1, SourceSpan const& s2, SourceSpan const& s3)
    {
        return SourceSpan{
            SourcePosition{
                std::min({s1.start.offset, s2.start.offset, s3.start.offset}),
            },
            SourcePosition{
                std::max({s1.end.offset, s2.end.offset, s3.end.offset}),
            },
        };
    }
}
