#pragma once
#include "weave/filesystem/DirectoryEnumerator.hxx"

namespace weave::filesystem::impl
{
    constexpr FileType ConvertToFileType(DWORD attributes)
    {
        if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
        {
            return FileType::SymbolicLink;
        }

        if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            return FileType::Directory;
        }

        return FileType::File;
    }
}
