#pragma once
#include <string_view>
#include "weave/time/DateTime.hxx"
#include "weave/filesystem/FileType.hxx"

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
        time::DateTime LastAccessTime;
        time::DateTime LastWriteTime;
        int64_t Size;
        FileType Type;
        bool Readonly;

        static std::optional<FileInfo> FromPath(std::string_view path);

        static bool Exists(std::string_view path);
    };
}
