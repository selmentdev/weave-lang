#pragma once
#include <string>
#include <optional>

namespace Weave
{
    std::optional<int> Execute(
        const char* path,
        const char* args,
        const char* workingDirectory,
        std::string& output,
        std::string& error);
}
