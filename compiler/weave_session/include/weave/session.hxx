#pragma once
#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/errors/handler.hxx"

#include <expected>
#include <optional>
#include <functional>

namespace weave::session
{
    class CodeGeneratorOptions final
    {
    public:
        static CodeGeneratorOptions from_command_line(
            errors::Handler& handler,
            commandline::CommandLineParseResult const& command_line);

    public:
        bool checked = false;
        bool debug = false;
    };

    class ExperimentalOptions final
    {
    public:
        static ExperimentalOptions from_command_line(
            errors::Handler& handler,
            commandline::CommandLineParseResult const& command_line);
    };
}
