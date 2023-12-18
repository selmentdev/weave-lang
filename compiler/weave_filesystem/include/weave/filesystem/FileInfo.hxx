#pragma once
#include <string_view>
#include "weave/time/DateTime.hxx"

namespace weave::filesystem
{
    bool FileExists(std::string_view path);

    bool DirectoryExists(std::string_view path);
}

namespace weave::filesystem
{
    struct FileInfo
    {
        time::DateTime CreationTime;
    };
}
