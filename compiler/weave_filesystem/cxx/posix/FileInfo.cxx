#include "weave/filesystem/FileInfo.hxx"
#include "weave/time/impl/Time.hxx"

#include "Common.hxx"

namespace weave::filesystem
{
    std::optional<FileInfo> FileInfo::FromPath(std::string_view path)
    {
        (void)path;
        return {};
    }

    bool FileInfo::Exists(std::string_view path)
    {
        (void)path;
        return false;
    }
}
