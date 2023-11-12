#include "Weave.Core/IO/Pipe.hxx"

#if defined(__linux__)

WEAVE_EXTERNAL_HEADERS_BEGIN
#define NOMINMAX
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace Weave::IO
{
    void Pipe::CloseRead()
    {
    }

    void Pipe::CloseWrite()
    {
    }

    std::expected<Pipe, FileSystemError> Pipe::Create()
    {
    }

    std::expected<size_t, FileSystemError> Pipe::Read(std::span<std::byte> buffer)
    {
    }

    std::expected<size_t, FileSystemError> Pipe::Write(std::span<std::byte const> buffer)
    {
    }

    std::expected<size_t, FileSystemError> Pipe::BytesAvailable() const
    {
    }
}

#endif
