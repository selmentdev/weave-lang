#pragma once
#include "Weave.Core/Source.hxx"

#include <string>
#include <vector>
#include <span>
#include <optional>

namespace Weave
{
    class SourceText final
    {
    private:
        std::string m_Content{};
        std::vector<uint32_t> m_Lines{};

    public:
        explicit SourceText(std::string&& content);

        [[nodiscard]] std::span<uint32_t const> GetLines() const
        {
            return this->m_Lines;
        }

        [[nodiscard]] std::string_view GetContent() const
        {
            return this->m_Content;
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
