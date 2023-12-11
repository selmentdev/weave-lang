#include "weave/filesystem/Pipe.hxx"
#include "weave/bugcheck/Assert.hxx"

#include "weave/platform/SystemError.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <unistd.h>
#include <sys/ioctl.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::filesystem
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

    std::expected<Pipe, platform::SystemError> Pipe::Create()
    {
        int fd[2];
        if (pipe(fd) == 0)
        {
            return Pipe{
                reinterpret_cast<void*>(fd[0]),
                reinterpret_cast<void*>(fd[1])
            };
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    std::expected<size_t, platform::SystemError> Pipe::Read(std::span<std::byte> buffer)
    {
        WEAVE_ASSERT(this->_read != nullptr);
        WEAVE_ASSERT(buffer.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));

        ssize_t const processed = read(
            static_cast<int>(reinterpret_cast<intptr_t>(this->_read)),
            buffer.data(),
            buffer.size());
            
        if (processed < 0)
        {
            return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
        }
        
        return static_cast<size_t>(processed);
    }

    std::expected<size_t, platform::SystemError> Pipe::Write(std::span<std::byte const> buffer)
    {
        WEAVE_ASSERT(this->_write != nullptr);
        WEAVE_ASSERT(buffer.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));
        
        ssize_t const processed = write(
            static_cast<int>(reinterpret_cast<intptr_t>(this->_write)),
            buffer.data(),
            buffer.size());
            
        if (processed < 0)
        {
            return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
        }

        return static_cast<size_t>(processed);
    }

    std::expected<size_t, platform::SystemError> Pipe::BytesAvailable() const
    {
        WEAVE_ASSERT(this->_read != nullptr);

        int n_available{};

        if (ioctl(static_cast<int>(reinterpret_cast<intptr_t>(this->_read)), FIONREAD, &n_available) == 0)
        {
            return static_cast<size_t>(n_available);
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }
}
