#include "weave/source/SourceText.hxx"
#include "weave/unicode.hxx"
#include "weave/bugcheck.hxx"

namespace weave::source
{
    SourceText::SourceText(std::string&& content)
        : _content{std::move(content)}
    {
        WEAVE_ASSERT(unicode::utf8_validate_string(this->_content));

        // Even empty source has one line starting at '0'
        this->_lines.emplace_back(0);

        if (size_t const length = this->_content.size(); length != 0)
        {
            const char* first = this->_content.data();
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
                        this->_lines.emplace_back(static_cast<uint32_t>(it - first));
                    }
                }
                else if (*it == '\n')
                {
                    ++it;

                    // Matched '\n'
                    this->_lines.emplace_back(static_cast<uint32_t>(it - first));
                }
                else
                {
                    ++it;
                }
            }

            WEAVE_ASSERT(it == last);
        }
    }

    std::optional<SourceSpan> SourceText::get_line(uint32_t index) const
    {
        if (index < this->_lines.size())
        {
            uint32_t const start = this->_lines[index];

            if (index == (this->_lines.size() - 1u))
            {
                return SourceSpan{
                    {start},
                    {static_cast<uint32_t>(this->_content.length())},
                };
            }

            // Match line ending.
            uint32_t const end = this->_lines[index + 1u];

            return SourceSpan{
                {start},
                {end},
            };
        }

        return std::nullopt;
    }

    std::optional<SourceSpan> SourceText::get_line_content(uint32_t index) const
    {
        // Get part of line span without line ending. Handle '\r\n' as single one, matching lexer definition.

        if (index < this->_lines.size())
        {
            uint32_t const start = this->_lines[index];

            if (index == (this->_lines.size() - 1u))
            {
                return SourceSpan{
                    {start},
                    {static_cast<uint32_t>(this->_content.length())},
                };
            }

            // Match line ending.
            uint32_t end = this->_lines[index + 1u];

            if ((end > 0) and (this->_content[end - 1] == '\n'))
            {
                if ((end > 1) and (this->_content[end - 2] == '\r'))
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

    std::string_view SourceText::get_line_text(uint32_t index) const
    {
        if (std::optional<SourceSpan> const& span = this->get_line(index); span.has_value())
        {
            return this->get_text(*span);
        }

        return std::string_view{};
    }

    std::string_view SourceText::get_line_content_text(uint32_t index) const
    {
        if (std::optional<SourceSpan> const& span = this->get_line_content(index); span.has_value())
        {
            return this->get_text(*span);
        }

        return std::string_view{};
    }

    LinePosition SourceText::get_line_position(SourcePosition const& position) const
    {
        uint32_t const index = this->get_line_index(position.offset);
        uint32_t const start = this->_lines[index];

        return LinePosition{
            .line = index,
            .column = position.offset - start,
        };
    }

    LineSpan SourceText::get_line_span(SourceSpan const& span) const
    {
        return LineSpan{
            this->get_line_position(span.start),
            this->get_line_position(span.end),
        };
    }

    std::string_view SourceText::get_text(const SourceSpan& span) const
    {
        size_t const start = span.start.offset;
        size_t const end = span.end.offset;

        WEAVE_ASSERT(start <= end);

        if (start < end)
        {
            size_t const length = end - start;
            return std::string_view{this->_content}.substr(start, length);
        }

        return std::string_view{};
    }

    uint32_t SourceText::get_line_index(uint32_t position) const
    {
        // TODO: don't look up positions past end of source
        auto const it = std::upper_bound(this->_lines.begin(), this->_lines.end(), position);
        return static_cast<uint32_t>(std::distance(this->_lines.begin(), it)) - 1u;
    }
}
