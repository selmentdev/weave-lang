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

    std::string_view GetExecutablePath();

    std::string_view GetStartupDirectory();
}
