#pragma once
#include "weave/platform/Compiler.hxx"
#include "weave/filesystem/FileSystemError.hxx"

#include <cstdint>

namespace weave::filesystem::impl
{
    FileSystemError TranslateErrno(uint32_t error);
}
