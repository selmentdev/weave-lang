#pragma once
#include "Compiler.Core/Platform/Compiler.hxx"
#include "Compiler.Core/IO/FileSystemError.hxx"

namespace Weave::IO
{
    FileSystemError TranslateErrorCode(uint32_t dwError);
}
