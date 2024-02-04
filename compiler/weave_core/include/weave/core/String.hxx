#pragma once
#include <string_view>
#include <string>
#include <vector>

namespace weave::core
{
    template <typename CharT, typename CallbackT = void(std::basic_string_view<CharT>)>
    constexpr void Split(
        CallbackT&& callback,
        std::basic_string_view<CharT> value,
        CharT separator,
        bool removeEmpty = true)
    {
        // Start from string beginning.
        typename std::basic_string_view<CharT>::size_type start{};

        while (true)
        {
            // Find separator position.
            typename std::basic_string_view<CharT>::size_type const pos = value.find(separator, start);

            // Get part.
            if (std::basic_string_view<CharT> const part = value.substr(start, pos - start); !part.empty() || !removeEmpty)
            {
                callback(part);
            }

            if (pos == std::basic_string_view<CharT>::npos)
            {
                // Value is empty.
                break;
            }

            // Move past separator.
            start = pos + 1;
        }
    }

    template <typename CharT>
    class StringSplitEnumerator final
    {
    public:
        using StringView = std::basic_string_view<CharT>;

    private:
        StringView m_View;
        StringView m_Part;

        typename StringView::size_type m_Start;

    public:
        explicit StringSplitEnumerator(std::basic_string_view<CharT> view)
            : m_View{view}
            , m_Part{}
            , m_Start{}
        {
        }

        StringView Current() const noexcept
        {
            return this->m_Part;
        }

        bool MoveNext(std::basic_string_view<CharT> separators, bool removeEmpty = true) noexcept
        {
            typename std::basic_string_view<CharT>::size_type start = this->m_Start;

            if (start == std::basic_string_view<CharT>::npos)
            {
                // Previous call already reached end.
                return false;
            }

            while (true)
            {
                // Find separator position.
                typename std::basic_string_view<CharT>::size_type const pos = this->m_View.find_first_of(separators, start);

                // Get part.
                std::basic_string_view<CharT> const part = this->m_View.substr(start, pos - start);

                // Move past separator.
                start = pos + 1;

                if (!part.empty() || !removeEmpty)
                {
                    // Remember state for next call
                    this->m_Part = part;
                    this->m_Start = start;
                    break;
                }

                if (pos == std::basic_string_view<CharT>::npos)
                {
                    // Reached last element. Mark it, so next call will fail.
                    this->m_Start = std::basic_string_view<CharT>::npos;
                    return false;
                }
            }

            return true;
        }

        void Reset() noexcept
        {
            this->m_Start = {};
            this->m_Part = {};
        }
    };

    template <typename CharT, typename CallbackT = void(std::basic_string_view<CharT>)>
    constexpr void Split(
        CallbackT&& callback,
        std::basic_string_view<CharT> value,
        std::basic_string_view<CharT> separators,
        bool removeEmpty = true)
    {
        // Start from string beginning.
        typename std::basic_string_view<CharT>::size_type start{};

        while (true)
        {
            // Find separator position.
            typename std::basic_string_view<CharT>::size_type const pos = value.find_first_of(separators);

            // Get part.
            if (std::basic_string_view<CharT> const part = value.substr(start, pos - start); !part.empty() || !removeEmpty)
            {
                callback(part);
            }

            if (pos == std::basic_string_view<CharT>::npos)
            {
                break;
            }

            // Move past separator.
            start = pos + 1;
        }
    }

    template <typename CharT>
    constexpr auto Split(
        std::basic_string_view<CharT> value,
        CharT separator,
        bool removeEmpty = true) -> std::vector<std::basic_string<CharT>>
    {
        std::vector<std::basic_string<CharT>> result{};

        Split([&](std::basic_string_view<CharT> part)
            {
                result.emplace_back(part);
            },
            value, separator, removeEmpty);

        return result;
    }
}

namespace weave::core
{
    template <typename RangeT, typename CharT>
    constexpr void Join(
        std::basic_string<CharT>& result,
        RangeT const& container,
        std::basic_string_view<CharT> separator) noexcept
    {
        auto it = std::cbegin(container);
        auto end = std::cend(container);

        if (it != end)
        {
            result.append(*it);

            for (++it; it != end; ++it)
            {
                result.append(separator);
                result.append(*it);
            }
        }
    }

    template <typename IteratorT, typename CharT>
    constexpr void Join(
        std::basic_string<CharT>& result,
        IteratorT first,
        IteratorT last,
        std::basic_string_view<CharT> separator) noexcept
    {
        if (first != last)
        {
            result.append(*first);

            for (++first; first != last; ++first)
            {
                result.append(separator);
                result.append(*first);
            }
        }
    }
}

namespace weave::core
{
    template <typename CharT>
    [[nodiscard]] constexpr bool MatchWildcard(
        typename std::basic_string_view<CharT>::const_iterator patternFirst,
        typename std::basic_string_view<CharT>::const_iterator patternLast,
        typename std::basic_string_view<CharT>::const_iterator valueFirst,
        typename std::basic_string_view<CharT>::const_iterator valueLast) noexcept
    {
        typename std::basic_string_view<CharT>::const_iterator storedPattern = patternFirst;
        typename std::basic_string_view<CharT>::const_iterator storedValue = valueFirst;

        bool restartable = false;

        while (valueFirst != valueLast)
        {
            if (patternFirst != patternLast && *patternFirst == CharT{'*'})
            {
                if (++patternFirst == patternLast)
                {
                    // Exit early if we cannot match next pattern character.
                    return true;
                }

                // Store pattern and advance value.
                storedPattern = patternFirst;
                storedValue = valueFirst + 1;

                // Matching first `*` enables restartable mode.
                restartable = true;
            }
            else if (patternFirst != patternLast && (*patternFirst == CharT{'?'} || tolower(*patternFirst) == tolower(*valueFirst)))
            {
                // Pattern matches value
                ++patternFirst;
                ++valueFirst;
            }
            else if (restartable == false)
            {
                // Only `*` matching may restart operation.
                return false;
            }
            else
            {
                if (storedValue == valueLast)
                {
                    // Can't restart operation.
                    return false;
                }

                // Restart matching with stored pattern.
                patternFirst = storedPattern;
                valueFirst = storedValue++;
            }
        }

        while (patternFirst != patternLast && *patternFirst == CharT{'*'})
        {
            // Consume trailing `*`. Compared value matched succesfully.
            ++patternFirst;
        }

        // Check if pattern exhausted.
        return patternFirst == patternLast;
    }

    template <typename CharT>
    [[nodiscard]] constexpr bool MatchWildcard(
        std::basic_string_view<CharT> pattern,
        std::basic_string_view<CharT> value) noexcept
    {
        return MatchWildcard<CharT>(
            pattern.cbegin(),
            pattern.cend(),
            value.cbegin(),
            value.cend());
    }
}
