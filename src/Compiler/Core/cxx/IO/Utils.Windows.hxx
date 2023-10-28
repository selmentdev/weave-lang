#pragma once
#include "Weave.Core/Platform/Compiler.hxx"
#include "Weave.Core/IO/FileSystemError.hxx"

namespace Weave::IO
{
    FileSystemError TranslateErrorCode(uint32_t dwError);
}
