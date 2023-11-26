#pragma once
#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/errors/Handler.hxx"

#include <expected>
#include <optional>
#include <functional>

namespace weave::session
{
    struct SanitizersSpec final
    {
        bool AddressSanitizer{};
        bool ThreadSanitizer{};
        bool MemorySanitizer{};
        bool LeakSanitizer{};
    };

    class CodeGeneratorOptions final
    {
    private:
        bool ApplyOptionFromCommandLine(std::string_view name, std::optional<std::string_view> const& value);

    public:
        static CodeGeneratorOptions FromCommandLine(
            errors::Handler& handler,
            commandline::CommandLineParseResult const& command_line);

    public:
        bool Checked = false;
        bool Debug = false;
        uint32_t OptimizationLevel = 0;
        SanitizersSpec Sanitizers{};

    public:
        void DebugPrint();
    };

    class ExperimentalOptions final
    {
    public:
        static ExperimentalOptions FromCommandLine(
            errors::Handler& handler,
            commandline::CommandLineParseResult const& command_line);
    };
}
