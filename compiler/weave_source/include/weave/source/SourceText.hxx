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

        [[nodiscard]] std::span<uint32_t const> GetLines() const
        {
            return this->_lines;
        }

        [[nodiscard]] std::string_view GetContentView() const
        {
            return this->_content;
        }

        [[nodiscard]] std::optional<SourceSpan> GetLine(uint32_t index) const;

        [[nodiscard]] std::optional<SourceSpan> GetLineContent(uint32_t index) const;

        [[nodiscard]] std::string_view GetLineText(uint32_t index) const;

        [[nodiscard]] std::string_view GetLineContentText(uint32_t index) const;

        [[nodiscard]] LinePosition GetLinePosition(SourcePosition const& position) const;

        [[nodiscard]] LineSpan GetLineSpan(SourceSpan const& span) const;

        [[nodiscard]] std::string_view GetText(SourceSpan const& span) const;

        [[nodiscard]] uint32_t GetLineIndex(uint32_t position) const;
    };
}
