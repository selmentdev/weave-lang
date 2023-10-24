#pragma once
#include "Compiler.Core/Source.hxx"

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
        std::vector<size_t> m_Lines{};

    public:
        explicit SourceText(std::string&& content);

        [[nodiscard]] std::span<size_t const> GetLines() const
        {
            return this->m_Lines;
        }

        [[nodiscard]] std::string_view GetContent() const
        {
            return this->m_Content;
        }


        [[nodiscard]] std::optional<SourceSpan> GetLine(size_t index) const;

        [[nodiscard]] std::optional<SourceSpan> GetLineContent(size_t index) const;

        [[nodiscard]] std::string_view GetLineText(size_t index) const;

        [[nodiscard]] std::string_view GetLineContentText(size_t index) const;

        [[nodiscard]] LinePosition GetLinePosition(SourcePosition const& position) const;

        [[nodiscard]] LineSpan GetLineSpan(SourceSpan const& span) const;

        [[nodiscard]] std::string_view GetText(SourceSpan const& span) const;

        [[nodiscard]] size_t GetLineIndex(size_t position) const;
    };
}
