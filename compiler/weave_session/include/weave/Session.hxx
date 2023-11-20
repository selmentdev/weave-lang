#pragma once
#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/errors/Handler.hxx"

#include <expected>
#include <optional>
#include <functional>

namespace weave::session
{
    class CodeGeneratorOptions final
    {
    public:
        static CodeGeneratorOptions FromCommandLine(
            errors::Handler& handler,
            commandline::CommandLineParseResult const& command_line);

    public:
        bool CheckedFromString(std::optional<std::string_view> const& value)
        {
            if (value)
            {
                if (*value == "true")
                {
                    this->Checked = true;
                    return true;
                }

                if (*value == "false")
                {
                    this->Checked = false;
                    return true;
                }
            }

            return false;
        }

        bool DebugFromString(std::optional<std::string_view> const& value)
        {
            if (value)
            {
                if (*value == "true")
                {
                    this->Debug = true;
                    return true;
                }

                if (*value == "false")
                {
                    this->Debug = false;
                    return true;
                }
            }

            return false;
        }

    public:
        bool Checked = false;
        bool Debug = false;
    };

    class ExperimentalOptions final
    {
    public:
        static ExperimentalOptions FromCommandLine(
            errors::Handler& handler,
            commandline::CommandLineParseResult const& command_line);
    };
}
