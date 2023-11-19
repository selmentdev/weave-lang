#pragma once
#include "Weave.Core/Platform/Compiler.hxx"
#include "Weave.Core/IO/FileSystemError.hxx"

#include <cstdint>

namespace weave::filesystem::impl
{
    FileSystemError TranslateErrno(uint32_t error);
}
