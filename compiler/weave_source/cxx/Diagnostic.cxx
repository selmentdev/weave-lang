#include "weave/source/Diagnostic.hxx"
#include "weave/source/SourceText.hxx"

namespace weave::source
{
    void FormatDiagnostic(
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
            lines.emplace_back(fmt::format("{}: {}", entry.Level, entry.Message));
        }

        LineSpan const line_range = source.GetLineSpan(entry.Source);
        lines.emplace_back(fmt::format("        --> {}:{}:{}", sink.Path, line_range.Start.Line + 1, line_range.Start.Column + 1));

        uint32_t const lines_count = line_range.End.Line - line_range.Start.Line;

        lines.emplace_back("         |");

        if (lines_count == 0)
        {
            uint32_t const column = line_range.Start.Column;
            uint32_t const line = line_range.Start.Line;
            std::string_view const line_view = source.GetLineContentText(line);

            lines.emplace_back(fmt::format("{:>8} | {}", line + 1, line_view));
            lines.emplace_back(fmt::format("         | {0: <{1}}{0:^<{2}}", "", column, std::max<uint32_t>(1, entry.Source.End.Offset - entry.Source.Start.Offset)));
        }
        else
        {
            bool too_long = lines_count > 6;
            uint32_t const prolog_line = line_range.Start.Line + 3;
            uint32_t const epilog_line = line_range.End.Line - 2;

            if (line_range.Start.Column > 0)
            {
                //lines.emplace_back(fmt::format("         | /{0:-<{1}}\\", "", line_range.End.Column));
            }

            for (uint32_t line = line_range.Start.Line; line <= line_range.End.Line; ++line)
            {
                if (too_long)
                {
                    if (line >= prolog_line)
                    {
                        lines.emplace_back(fmt::format("     ... |"));

                        line = epilog_line;
                        too_long = false;
                    }
                }

                auto sourceSpan = source.GetLineContent(line).value_or(SourceSpan{});
                SourceSpan clamped{
                    .Start = SourcePosition{std::max(sourceSpan.Start.Offset, entry.Source.Start.Offset)},
                    .End = SourcePosition{std::min(sourceSpan.End.Offset, entry.Source.End.Offset)},
                };
                auto lineSpan = source.GetLineSpan(clamped);
                std::string_view const lineView = source.GetLineContentText(line);

                lines.emplace_back(fmt::format("{:>8} | {}", line + 1, lineView));
                lines.emplace_back(fmt::format("         | {0: <{1}}{0:^<{2}}", "", lineSpan.Start.Column, lineSpan.End.Column - lineSpan.Start.Column, std::max<uint32_t>(1, entry.Source.End.Offset - entry.Source.Start.Offset)));
            }

            //lines.emplace_back(fmt::format("         | |{0:_<{1}}^", "", line_range.End.Column));
        }
    }

    void FormatDiagnostics(
        std::vector<std::string>& lines,
        SourceText const& source,
        DiagnosticSink const& sink,
        size_t limit)
    {
        size_t current = 0;

        for (DiagnosticSink::Entry const& item : sink.Items)
        {
            FormatDiagnostic(lines, source, item, sink);

            if (current > limit)
            {
                lines.emplace_back(fmt::format("Too many error messages: {}", sink.Items.size()));
                break;
            }

            ++current;
        }
    }
}
