#pragma once
#include "weave/source/Source.hxx"

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
            SourceSpan Source{};
            DiagnosticLevel Level{};
            std::string Message{};
        };

    public:
        std::string Path{};
        std::vector<Entry> Items{};

    public:
        void Add(SourceSpan const& source, DiagnosticLevel level, std::string&& message)
        {
            this->Items.emplace_back(source, level, std::move(message));
        }

        void AddError(SourceSpan const& source, std::string&& message)
        {
            this->Add(source, DiagnosticLevel::Error, std::move(message));
        }

        void AddWarning(SourceSpan const& source, std::string&& message)
        {
            this->Add(source, DiagnosticLevel::Warning, std::move(message));
        }

        void AddInfo(SourceSpan const& source, std::string&& message)
        {
            this->Add(source, DiagnosticLevel::Info, std::move(message));
        }

        void AddHint(SourceSpan const& source, std::string&& message)
        {
            this->Add(source, DiagnosticLevel::Hint, std::move(message));
        }
    };

    void FormatDiagnostic(
        std::vector<std::string>& lines,
        SourceText const& source,
        DiagnosticSink::Entry const& entry,
        DiagnosticSink const& sink);

    void FormatDiagnostics(
        std::vector<std::string>& lines,
        SourceText const& source,
        DiagnosticSink const& sink,
        size_t limit);
}
