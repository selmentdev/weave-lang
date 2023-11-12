#include "Weave.Core/IO/Pipe.hxx"

#if defined(__linux__)
#include "Weave.Core/Assert.hxx"
#include "Utils.Posix.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <unistd.h>
#include <sys/ioctl.h>

WEAVE_EXTERNAL_HEADERS_END

namespace Weave::IO
{
    void Pipe::CloseRead()
    {
        if (this->_read != nullptr)
        {
            close(static_cast<int>(reinterpret_cast<intptr_t>(this->_read)));
            this->_read = nullptr;
        }
    }

    void Pipe::CloseWrite()
    {
        if (this->_write != nullptr)
        {
            close(static_cast<int>(reinterpret_cast<intptr_t>(this->_write)));
            this->_write = nullptr;
        }
    }

    std::expected<Pipe, FileSystemError> Pipe::Create()
    {
        int fd[2];
        if (pipe(fd) == 0)
        {
            return Pipe{
                reinterpret_cast<void*>(fd[0]),
                reinterpret_cast<void*>(fd[1])
            };
        }

        return std::unexpected(TranslateErrno(errno));
    }

    std::expected<size_t, FileSystemError> Pipe::Read(std::span<std::byte> buffer)
    {
        WEAVE_ASSERT(this->_read != nullptr);
        WEAVE_ASSERT(buffer.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));

        ssize_t const processed = read(
            static_cast<int>(reinterpret_cast<intptr_t>(this->_read)),
            buffer.data(),
            buffer.size());
            
        fmt::println("pipe read: {}", processed);
        if (processed < 0)
        {
            return std::unexpected(TranslateErrno(errno));
        }
        
        return static_cast<size_t>(processed);
    }

    std::expected<size_t, FileSystemError> Pipe::Write(std::span<std::byte const> buffer)
    {
        WEAVE_ASSERT(this->_write != nullptr);
        WEAVE_ASSERT(buffer.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));
        
        ssize_t const processed = write(
            static_cast<int>(reinterpret_cast<intptr_t>(this->_write)),
            buffer.data(),
            buffer.size());
            
        fmt::println("pipe write: {}", processed);
        if (processed < 0)
        {
            return std::unexpected(TranslateErrno(errno));
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, FileSystemError> Pipe::BytesAvailable() const
    {
        WEAVE_ASSERT(this->_read != nullptr);

        int n_available{};

        if (ioctl(static_cast<int>(reinterpret_cast<intptr_t>(this->_read)), FIONREAD, &n_available) == 0)
        {
            fmt::println("bytes avail {}", n_available);
            fflush(stdout);
            return static_cast<size_t>(n_available);
        }

        fmt::println("bytes avail failed");
        fflush(stdout);

        return std::unexpected(TranslateErrno(errno));
    }
}

#endif
