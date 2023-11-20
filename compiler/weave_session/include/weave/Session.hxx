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
