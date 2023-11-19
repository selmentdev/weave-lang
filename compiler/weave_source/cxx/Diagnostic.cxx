#include "weave/source/diagnostic.hxx"
#include "weave/source/SourceText.hxx"

namespace weave::source
{
    void format_diagnostic(
        std::vector<std::string>& lines,
        SourceText const& source,
        DiagnosticSink::Entry const& entry,
        DiagnosticSink const& sink)
    {
        //if (entry.ErrorCode > 0)
        {
            //fmt::format_to(out, "{}({:04}): {}", entry.Level, entry.ErrorCode, entry.Message);
        }
        //else
        {
            lines.emplace_back(fmt::format("{}: {}", entry.level, entry.message));
        }

        LineSpan const line_range = source.get_line_span(entry.source);
        lines.emplace_back(fmt::format("        --> {}:{}:{}", sink.path, line_range.start.line + 1, line_range.start.column + 1));

        uint32_t const lines_count = line_range.end.line - line_range.start.line;

        lines.emplace_back("         |");

        if (lines_count == 0)
        {
            uint32_t const column = line_range.start.column;
            uint32_t const line = line_range.start.line;
            std::string_view const line_view = source.get_line_content_text(line);

            lines.emplace_back(fmt::format("{:>8} | {}", line + 1, line_view));
            lines.emplace_back(fmt::format("         | {0: <{1}}{0:^<{2}}", "", column, std::max<uint32_t>(1, entry.source.end.offset - entry.source.start.offset)));
        }
        else
        {
            bool too_long = lines_count > 6;
            uint32_t const prolog_line = line_range.start.line + 3;
            uint32_t const epilog_line = line_range.end.line - 2;

            if (line_range.start.column > 0)
            {
                lines.emplace_back(fmt::format("         | /{0:-<{1}}\\", "", line_range.end.column));
            }

            for (uint32_t line = line_range.start.line; line <= line_range.end.line; ++line)
            {
                if (too_long)
                {
                    if (line >= prolog_line)
                    {
                        lines.emplace_back(fmt::format("     ... | |"));

                        line = epilog_line;
                        too_long = false;
                    }
                }

                std::string_view const lineView = source.get_line_content_text(line);

                lines.emplace_back(fmt::format("{:>8} | | {}", line + 1, lineView));
            }

            lines.emplace_back(fmt::format("         | |{0:_<{1}}^", "", line_range.end.column));
        }
    }

    void format_diagnostics(
        std::vector<std::string>& lines,
        SourceText const& source,
        DiagnosticSink const& sink,
        size_t limit)
    {
        size_t current = 0;

        for (DiagnosticSink::Entry const& item : sink.items)
        {
            format_diagnostic(lines, source, item, sink);

            if (current > limit)
            {
                lines.emplace_back(fmt::format("Too many error messages: {}", sink.items.size()));
                break;
            }

            ++current;
        }
    }
}
