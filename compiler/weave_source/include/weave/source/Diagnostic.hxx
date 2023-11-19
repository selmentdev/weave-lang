#pragma once
#include "weave/source/source.hxx"

#include <fmt/format.h>

#include <vector>

namespace weave::source
{
    class SourceText;
}

namespace weave::source
{
    enum class DiagnosticLevel
    {
        Error,
        Warning,
        Info,
        Hint,
    };
}

template <>
struct fmt::formatter<weave::source::DiagnosticLevel> : formatter<std::string_view>
{
    constexpr auto format(weave::source::DiagnosticLevel const& value, auto& context)
    {
        std::string_view result = "unknown";

        switch (value)
        {
        case weave::source::DiagnosticLevel::Error:
            result = "error";
            break;

        case weave::source::DiagnosticLevel::Warning:
            result = "warning";
            break;

        case weave::source::DiagnosticLevel::Info:
            result = "info";
            break;

        case weave::source::DiagnosticLevel::Hint:
            result = "hint";
            break;
        }

        return formatter<std::string_view>::format(result, context);
    }
};

namespace weave::source
{
    class DiagnosticSink final
    {
    public:
        struct Entry final
        {
            SourceSpan source{};
            DiagnosticLevel level{};
            std::string message{};
        };

    public:
        std::string path{};
        std::vector<Entry> items{};

    public:
        void add(SourceSpan const& source, DiagnosticLevel level, std::string&& message)
        {
            this->items.emplace_back(source, level, std::move(message));
        }

        void add_error(SourceSpan const& source, std::string&& message)
        {
            this->add(source, DiagnosticLevel::Error, std::move(message));
        }

        void add_warning(SourceSpan const& source, std::string&& message)
        {
            this->add(source, DiagnosticLevel::Warning, std::move(message));
        }

        void add_info(SourceSpan const& source, std::string&& message)
        {
            this->add(source, DiagnosticLevel::Info, std::move(message));
        }

        void add_hint(SourceSpan const& source, std::string&& message)
        {
            this->add(source, DiagnosticLevel::Hint, std::move(message));
        }
    };

    void format_diagnostic(
        std::vector<std::string>& lines,
        SourceText const& source,
        DiagnosticSink::Entry const& entry,
        DiagnosticSink const& sink);

    void format_diagnostics(
        std::vector<std::string>& lines,
        SourceText const& source,
        DiagnosticSink const& sink,
        size_t limit);
}
