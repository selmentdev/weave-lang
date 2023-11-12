#include "Weave.Core/Process.hxx"

#if defined(__linux__)

namespace Weave
{
    std::optional<int> Execute(
        const char* path,
        const char* args,
        const char* workingDirectory,
        std::string& output,
        std::string& error);
}

#endif
