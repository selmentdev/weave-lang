#pragma once
#include <compare>
#include <cstdint>
#include <cstddef>
#include <utility>
#include <algorithm>

namespace weave::source
{
    struct LineIndex final
    {
        uint32_t Value;
    };

    struct ColumnIndex final
    {
        uint32_t Value;
    };

    struct ByteIndex final
    {
        uint32_t Value;
    };

    struct LinePosition final
    {
        uint32_t Line;
        uint32_t Column;

        [[nodiscard]] constexpr auto operator<=>(LinePosition const&) const = default;
    };

    struct LineSpan final
    {
        LinePosition Start;
        LinePosition End;

        [[nodiscard]] constexpr auto operator<=>(LineSpan const&) const = default;
    };

    struct SourcePosition final
    {
        uint32_t Offset;

        [[nodiscard]] constexpr auto operator<=>(SourcePosition const&) const = default;
    };

    struct SourceSpan final
    {
        SourcePosition Start;
        SourcePosition End;

        [[nodiscard]] constexpr auto operator<=>(SourceSpan const&) const = default;

        [[nodiscard]] constexpr bool Contains(SourceSpan const& other) const
        {
            return (this->Start.Offset <= other.Start.Offset) and (other.End.Offset <= this->End.Offset);
        }

        [[nodiscard]] constexpr bool Contains(SourcePosition const& other) const
        {
            return (this->Start.Offset <= other.Offset) and (other.Offset <= this->End.Offset);
        }

        [[nodiscard]] constexpr SourceSpan WithZeroLength() const
        {
            return {this->Start, this->Start};
        }
    };

    [[nodiscard]] constexpr bool Disjoint(SourceSpan const& self, SourceSpan const& other)
    {
        uint32_t const min_end = std::min(self.End.Offset, other.End.Offset);
        uint32_t const max_start = std::max(self.Start.Offset, other.Start.Offset);
        return min_end <= max_start;
    }

    [[nodiscard]] constexpr SourceSpan Combine(SourceSpan const& s1, SourceSpan const& s2)
    {
        return SourceSpan{
            SourcePosition{
                std::min(s1.Start.Offset, s2.Start.Offset),
            },
            SourcePosition{
                std::max(s1.End.Offset, s2.End.Offset),
            },
        };
    }

    [[nodiscard]] constexpr SourceSpan Combine(SourceSpan const& s, SourcePosition const& p)
    {
        return SourceSpan{
            SourcePosition{
                std::min(s.Start.Offset, p.Offset),
            },
            SourcePosition{
                std::max(s.End.Offset, p.Offset),
            },
        };
    }

    [[nodiscard]] constexpr SourceSpan Combine(SourceSpan const& s1, SourceSpan const& s2, SourceSpan const& s3)
    {
        return SourceSpan{
            SourcePosition{
                std::min({s1.Start.Offset, s2.Start.Offset, s3.Start.Offset}),
            },
            SourcePosition{
                std::max({s1.End.Offset, s2.End.Offset, s3.End.Offset}),
            },
        };
    }
}
