#pragma once
#include "Weave.Core/Platform/Compiler.hxx"
#include "Weave.Core/IO/FileSystemError.hxx"

#include <cstdint>

namespace Weave::IO
{
    FileSystemError TranslateErrno(uint32_t error);
}
