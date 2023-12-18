#pragma once
#include "weave/platform/Compiler.hxx"
#include "weave/filesystem/FileType.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <dirent.h>

WEAVE_EXTERNAL_HEADERS_END


namespace weave::filesystem::impl
{
    constexpr FileType ConvertToFileType(unsigned d_type)
    {
        switch (d_type)
        {
        default:
        case DT_UNKNOWN:
            return FileType::Unknown;

        case DT_FIFO:
            return FileType::NamedPipe;

        case DT_CHR:
            return FileType::CharacterDevice;

        case DT_DIR:
            return FileType::Directory;

        case DT_BLK:
            return FileType::BlockDevice;

        case DT_REG:
            return FileType::File;

        case DT_LNK:
            return FileType::SymbolicLink;

        case DT_SOCK:
            return FileType::Socket;

        }
    }
}
