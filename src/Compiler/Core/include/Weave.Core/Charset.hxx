#pragma once
#include <array>
#include <algorithm>
#include <string_view>
#include <span>
#include <cstdint>

namespace Weave
{
    class Charset final
    {
    private:
        std::array<uint64_t, 256 / 8> m_Bits{};

    public:
        explicit constexpr Charset(std::string_view chars) noexcept
        {
            for (unsigned char const c : chars)
            {
                this->m_Bits[c >> 5u] |= uint64_t{1u} << (c & 0x3Fu);
            }
        }

        [[nodiscard]] constexpr bool Contains(char32_t c) const noexcept
        {
            size_t const index = static_cast<size_t>(c);
            return (this->m_Bits[index >> 5u] & (uint64_t{1u} << (index & 0x3Fu))) != 0;
        }

        [[nodiscard]] constexpr bool Contains(char16_t c) const noexcept
        {
            return this->Contains(static_cast<char32_t>(c));
        }

        [[nodiscard]] constexpr bool Contains(char8_t c) const noexcept
        {
            return this->Contains(static_cast<char32_t>(c));
        }

        [[nodiscard]] constexpr bool Contains(char c) const noexcept
        {
            return this->Contains(static_cast<char8_t>(c));
        }
    };

    class UnicodeCharset final
    {
    public:
        struct Range final
        {
            char32_t Lower;
            char32_t Upper;

            [[nodiscard]] friend constexpr bool operator<(char32_t lhs, Range const& rhs) noexcept
            {
                return lhs < rhs.Lower;
            }

            [[nodiscard]] friend constexpr bool operator<(Range const& lhs, char32_t rhs) noexcept
            {
                return lhs.Upper < rhs;
            }
        };

    private:
        std::span<Range const> m_Items{};

    public:
        explicit UnicodeCharset(std::span<Range const> items) noexcept
            : m_Items{items}
        {
        }

        [[nodiscard]] constexpr bool Contains(char32_t value) noexcept
        {
            return std::binary_search(this->m_Items.begin(), this->m_Items.end(), value);
        }
    };
}
