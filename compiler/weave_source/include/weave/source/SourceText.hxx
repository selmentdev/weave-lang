#pragma once
#include "weave/source/Source.hxx"

#include <string>
#include <vector>
#include <span>
#include <optional>

namespace weave::source
{
    class SourceText final
    {
    private:
        std::string _content{};
        std::vector<uint32_t> _lines{};

    public:
        explicit SourceText(std::string&& content);

        [[nodiscard]] std::span<uint32_t const> get_lines() const
        {
            return this->_lines;
        }

        [[nodiscard]] std::string_view get_content_view() const
        {
            return this->_content;
        }


        [[nodiscard]] std::optional<SourceSpan> get_line(uint32_t index) const;

        [[nodiscard]] std::optional<SourceSpan> get_line_content(uint32_t index) const;

        [[nodiscard]] std::string_view get_line_text(uint32_t index) const;

        [[nodiscard]] std::string_view get_line_content_text(uint32_t index) const;

        [[nodiscard]] LinePosition get_line_position(SourcePosition const& position) const;

        [[nodiscard]] LineSpan get_line_span(SourceSpan const& span) const;

        [[nodiscard]] std::string_view get_text(SourceSpan const& span) const;

        [[nodiscard]] uint32_t get_line_index(uint32_t position) const;
    };
}
