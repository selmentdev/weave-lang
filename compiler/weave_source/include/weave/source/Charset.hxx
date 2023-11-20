#pragma once
#include <array>
#include <algorithm>
#include <string_view>
#include <span>
#include <cstdint>

namespace weave::source
{
    class Charset final
    {
    private:
        std::array<uint64_t, 256 / 8> _bits{};

    public:
        explicit constexpr Charset(std::string_view chars) noexcept
        {
            for (unsigned char const c : chars)
            {
                this->_bits[c >> 5u] |= uint64_t{1u} << (c & 0x3Fu);
            }
        }

        [[nodiscard]] constexpr bool Contains(char32_t c) const noexcept
        {
            size_t const index = static_cast<size_t>(c);
            return (this->_bits[index >> 5u] & (uint64_t{1u} << (index & 0x3Fu))) != 0;
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
            char32_t lower;
            char32_t upper;

            [[nodiscard]] friend constexpr bool operator<(char32_t lhs, Range const& rhs) noexcept
            {
                return lhs < rhs.lower;
            }

            [[nodiscard]] friend constexpr bool operator<(Range const& lhs, char32_t rhs) noexcept
            {
                return lhs.upper < rhs;
            }
        };

    private:
        std::span<Range const> _items{};

    public:
        explicit UnicodeCharset(std::span<Range const> items) noexcept
            : _items{items}
        {
        }

        [[nodiscard]] constexpr bool Contains(char32_t value) const noexcept
        {
            return std::binary_search(this->_items.begin(), this->_items.end(), value);
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
}
