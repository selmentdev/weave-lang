#pragma once
#include <expected>
#include <string>

#include "weave/platform/SystemError.hxx"

namespace weave::system
{
    std::expected<int, platform::SystemError> Execute(
        const char* path,
        const char* args,
        const char* working_directory,
        std::string& output,
        std::string& error);

    /// \brief Get the path to the executable.
    std::string_view GetExecutablePath();

    //// \brief Get the startup directory.
    std::string_view GetStartupDirectory();
}
