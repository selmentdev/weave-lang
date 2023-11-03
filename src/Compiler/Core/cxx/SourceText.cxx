#include "Weave.Core/SourceText.hxx"
#include "Weave.Core/Unicode.hxx"

#include <cassert>

namespace Weave
{
    SourceText::SourceText(std::string&& content)
        : m_Content{std::move(content)}
    {
        assert(UTF8ValidateString(this->m_Content));

        // Even empty source has one line starting at '0'
        this->m_Lines.push_back(0);

        if (size_t const length = this->m_Content.size(); length != 0)
        {
            const char* first = this->m_Content.data();
            const char* last = first + length;

            // Split lines by '\r\n' or '\n' only.
            // Mac line ending (isolated CR) is treated as whitespace character.
            // Unicode line endings are not supported.
            // This matches lexer behavior.

            const char* it = first;

            while (it < last)
            {
                if (*it == '\r')
                {
                    ++it;

                    if ((it < last) and (*it == '\n'))
                    {
                        ++it;

                        // Matched '\r\n'
                        this->m_Lines.push_back(static_cast<uint32_t>(it - first));
                    }
                }
                else if (*it == '\n')
                {
                    ++it;

                    // Matched '\n'
                    this->m_Lines.push_back(static_cast<uint32_t>(it - first));
                }
                else
                {
                    ++it;
                }
            }

            assert(it == last);
        }
    }

    std::optional<SourceSpan> SourceText::GetLine(uint32_t index) const
    {
        if (index < this->m_Lines.size())
        {
            uint32_t const start = this->m_Lines[index];

            if (index == (this->m_Lines.size() - 1u))
            {
                return SourceSpan{
                    {start},
                    {static_cast<uint32_t>(this->m_Content.length())},
                };
            }

            // Match line ending.
            uint32_t const end = this->m_Lines[index + 1u];

            return SourceSpan{
                {start},
                {end},
            };
        }

        return std::nullopt;
    }

    std::optional<SourceSpan> SourceText::GetLineContent(uint32_t index) const
    {
        // Get part of line span without line ending. Handle '\r\n' as single one, matching lexer definition.

        if (index < this->m_Lines.size())
        {
            uint32_t const start = this->m_Lines[index];

            if (index == (this->m_Lines.size() - 1u))
            {
                return SourceSpan{
                    {start},
                    {static_cast<uint32_t>(this->m_Content.length())},
                };
            }

            // Match line ending.
            uint32_t end = this->m_Lines[index + 1u];

            if ((end > 0) and (this->m_Content[end - 1] == '\n'))
            {
                if ((end > 1) and (this->m_Content[end - 2] == '\r'))
                {
                    // Matched '\r\n'
                    --end;
                }

                // Matched single '\n'
                --end;
            }

            return SourceSpan{
                {start},
                {end},
            };
        }

        return std::nullopt;
    }

    std::string_view SourceText::GetLineText(uint32_t index) const
    {
        if (std::optional<SourceSpan> const& span = this->GetLine(index); span.has_value())
        {
            return this->GetText(*span);
        }

        return std::string_view{};
    }

    std::string_view SourceText::GetLineContentText(uint32_t index) const
    {
        if (std::optional<SourceSpan> const& span = this->GetLineContent(index); span.has_value())
        {
            return this->GetText(*span);
        }

        return std::string_view{};
    }

    LinePosition SourceText::GetLinePosition(SourcePosition const& position) const
    {
        uint32_t const index = this->GetLineIndex(position.Offset);
        uint32_t const start = this->m_Lines[index];

        return LinePosition{
            .Line = index,
            .Column = position.Offset - start,
        };
    }

    LineSpan SourceText::GetLineSpan(SourceSpan const& span) const
    {
        return LineSpan{
            this->GetLinePosition(span.Start),
            this->GetLinePosition(span.End),
        };
    }

    std::string_view SourceText::GetText(const SourceSpan& span) const
    {
        size_t const start = span.Start.Offset;
        size_t const end = span.End.Offset;

        assert(start <= end);

        if (start < end)
        {
            size_t const length = end - start;
            return std::string_view{this->m_Content}.substr(start, length);
        }

        return std::string_view{};
    }

    uint32_t SourceText::GetLineIndex(uint32_t position) const
    {
        // TODO: don't look up positions past end of source
        auto const it = std::upper_bound(this->m_Lines.begin(), this->m_Lines.end(), position);
        return static_cast<uint32_t>(std::distance(this->m_Lines.begin(), it)) - 1u;
    }
}
