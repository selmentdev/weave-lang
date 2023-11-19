#pragma once
#include <string>
#include <optional>

namespace weave::system
{
    std::optional<int> Execute(
        const char* path,
        const char* args,
        const char* working_directory,
        std::string& output,
        std::string& error);
}
