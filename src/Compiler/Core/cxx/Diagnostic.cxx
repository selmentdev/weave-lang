#include "Weave.Core/Diagnostic.hxx"
#include "Weave.Core/SourceText.hxx"

namespace Weave
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

        LineSpan const lineRange = source.GetLineSpan(entry.Source);
        lines.emplace_back(fmt::format("        --> {}:{}:{}", sink.Path, lineRange.Start.Line + 1, lineRange.Start.Column + 1));

        size_t const linesCount = lineRange.End.Line - lineRange.Start.Line;

        lines.emplace_back("         |");

        if (linesCount == 0)
        {
            size_t const column = lineRange.Start.Column;
            size_t const line = lineRange.Start.Line;
            std::string_view const lineView = source.GetLineContentText(line);

            lines.emplace_back(fmt::format("{:>8} | {}", line + 1, lineView));
            lines.emplace_back(fmt::format("         | {0: <{1}}{0:^<{2}}", "", column, std::max<size_t>(1, entry.Source.End.Offset - entry.Source.Start.Offset)));
        }
        else
        {
            bool tooLong = linesCount > 6;
            size_t const prologLine = lineRange.Start.Line + 3;
            size_t const epilogLine = lineRange.End.Line - 2;

            if (lineRange.Start.Column > 0)
            {
                lines.emplace_back(fmt::format("         | /{0:-<{1}}\\", "", lineRange.End.Column));
            }

            for (size_t line = lineRange.Start.Line; line <= lineRange.End.Line; ++line)
            {
                if (tooLong)
                {
                    if (line >= prologLine)
                    {
                        lines.emplace_back(fmt::format("     ... | |"));

                        line = epilogLine;
                        tooLong = false;
                    }
                }

                std::string_view const lineView = source.GetLineContentText(line);

                lines.emplace_back(fmt::format("{:>8} | | {}", line + 1, lineView));
            }

            lines.emplace_back(fmt::format("         | |{0:_<{1}}^", "", lineRange.End.Column));
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
