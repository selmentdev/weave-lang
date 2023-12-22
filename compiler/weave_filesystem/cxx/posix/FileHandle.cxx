#include "weave/filesystem/FileHandle.hxx"
#include "weave/platform/SystemError.hxx"

#include <cassert>
#include <cerrno>
#include <string>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>

namespace weave::filesystem::impl
{
    struct PlatformFileHandle final
    {
        int FileDescriptor{};
        int Padding{};
    };

    int TranslateToOpenFlags(FileMode mode, FileAccess access, FileOptions options, bool failForSymlinks)
    {
        int result{};

        if (failForSymlinks)
        {
            result |= O_NOFOLLOW;
        }

        switch (mode)
        {
        case FileMode::OpenExisting:
            break;

        case FileMode::TruncateExisting:
            result |= O_TRUNC;
            break;

        case FileMode::OpenAlways:
            result |= O_CREAT;
            break;

        case FileMode::CreateAlways:
            result |= O_CREAT | O_TRUNC;
            break;

        case FileMode::CreateNew:
            result |= O_CREAT | O_EXCL;
            break;
        }

        switch (access)
        {
        case FileAccess::Read:
            result |= O_RDONLY;
            break;

        case FileAccess::Write:
            result |= O_WRONLY;
            break;

        case FileAccess::ReadWrite:
            result |= O_RDWR;
            break;
        }

        if (options.WriteThrough)
        {
            result |= O_SYNC;
        }

        return result;
    }
}

namespace weave::filesystem
{
    inline impl::PlatformFileHandle& FileHandle::AsPlatform()
    {
        return *reinterpret_cast<impl::PlatformFileHandle*>(&this->_native);
    }

    inline impl::PlatformFileHandle const& FileHandle::AsPlatform() const
    {
        return *reinterpret_cast<impl::PlatformFileHandle const*>(&this->_native);
    }

    FileHandle::FileHandle(impl::PlatformFileHandle const& native)
    {
        this->AsPlatform() = native;
    }

    FileHandle::FileHandle(FileHandle&& other) noexcept
    {
        this->AsPlatform() = std::exchange(other.AsPlatform(), {});
    }

    FileHandle& FileHandle::operator=(FileHandle&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            if (this->AsPlatform().FileDescriptor >= 0)
            {
                // Ignore errors.
                (void)this->Close();
            }

            this->AsPlatform() = std::exchange(other.AsPlatform(), {});
        }

        return *this;
    }

    FileHandle::~FileHandle()
    {
        if (this->AsPlatform().FileDescriptor >= 0)
        {
            // Ignore errors.
            (void)this->Close();
        }
    }

    std::expected<FileHandle, platform::SystemError> FileHandle::Create(std::string_view path, FileMode mode, FileAccess access, FileOptions options)
    {
        int const flags = impl::TranslateToOpenFlags(mode, access, options, false);

        int const fd = open(std::string{path}.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

        if (fd != -1)
        {
            if (flock(fd, LOCK_EX | LOCK_NB) == -1)
            {
                int const error = errno;
                bool failed{false};

                if ((error == EAGAIN) or (error == EWOULDBLOCK))
                {
                    failed = true;
                }

                if (failed)
                {
                    close(fd);
                    return std::unexpected(platform::impl::SystemErrorFromErrno(error));
                }
            }

            if ((mode == FileMode::TruncateExisting) or (mode == FileMode::CreateAlways))
            {
                if (ftruncate64(fd, 0) != 0)
                {
                    int error = errno;
                    close(fd);

                    return std::unexpected(platform::impl::SystemErrorFromErrno(error));
                }
            }

            return FileHandle{impl::PlatformFileHandle{
                .FileDescriptor = fd,
            });
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    bool FileHandle::Exists(std::string_view path)
    {
        // clang-format off
        struct stat64 st{};
        // clang-format on
        return (stat64(std::string{path}.c_str(), &st) == 0) and S_ISREG(st.st_mode);
    }

    std::expected<void, platform::SystemError> FileHandle::Close()
    {
        impl::PlatformFileHandle& native = this->AsPlatform();
        assert(native.FileDescriptor >= 0);

        if (close(native.FileDescriptor) == 0)
        {
            return {};
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    std::expected<int64_t, platform::SystemError> FileHandle::GetLength() const
    {
        impl::PlatformFileHandle const& native = this->AsPlatform();
        assert(native.FileDescriptor >= 0);

        // clang-format off
        struct stat64 st{};
        // clang-format on

        if (fstat64(native.FileDescriptor, &st) == 0)
        {
            return st.st_size;
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    std::expected<void, platform::SystemError> FileHandle::SetLength(int64_t length)
    {
        impl::PlatformFileHandle& native = this->AsPlatform();
        assert(native.FileDescriptor >= 0);

        if (ftruncate64(native.FileDescriptor, length) == 0)
        {
            return {};
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    std::expected<size_t, platform::SystemError> FileHandle::Read(std::span<std::byte> buffer, int64_t position)
    {
        impl::PlatformFileHandle& native = this->AsPlatform();
        assert(native.FileDescriptor >= 0);

        ssize_t processed = 0;
        while (processed < std::ssize(buffer))
        {
            ssize_t const result = pread64(
                native.FileDescriptor,
                buffer.data() + processed,
                buffer.size() - processed,
                position + processed);

            if (result < 0)
            {
                return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
            }

            if (result == 0)
            {
                break;
            }

            processed += result;
        }

        return processed;
    }

    std::expected<size_t, platform::SystemError> FileHandle::Write(std::span<std::byte const> buffer, int64_t position)
    {
        impl::PlatformFileHandle& native = this->AsPlatform();
        assert(native.FileDescriptor >= 0);

        ssize_t processed = 0;
        while (processed < std::ssize(buffer))
        {
            ssize_t const result = pwrite64(
                native.FileDescriptor,
                buffer.data() + processed,
                buffer.size() - processed,
                position + processed);

            if (result < 0)
            {
                return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
            }

            if (result == 0)
            {
                break;
            }

            processed += result;
        }

        return processed;
    }
}
