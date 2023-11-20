#pragma once
#include "weave/platform/compiler.hxx"
#include "weave/filesystem/FileSystemError.hxx"

namespace weave::filesystem::impl
{
    FileSystemError TranslateErrorCode(uint32_t error);
}
