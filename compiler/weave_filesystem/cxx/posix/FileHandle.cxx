#include "weave/filesystem/FileHandle.hxx"
#include "weave/platform/SystemError.hxx"

#include <cassert>
#include <cerrno>
#include <string>
#include <sys/stat.h>
#include <sys/file.h>
#include <unistd.h>
#include <fcntl.h>

namespace weave::filesystem
{
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

    static int ToFileDescriptor(void* handle)
    {
        return static_cast<int>(reinterpret_cast<intptr_t>(handle));
    }

    std::expected<FileHandle, platform::SystemError> FileHandle::Create(std::string_view path, FileMode mode, FileAccess access, FileOptions options)
    {
        int const flags = TranslateToOpenFlags(mode, access, options, false);

        int const fd = open(std::string{path}.c_str(), flags, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        if (fd != -1)
        {
            if (flock(fd, LOCK_EX | LOCK_NB) == -1)
            {
                int error = errno;
                bool failed{ false };

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

            return FileHandle{reinterpret_cast<void*>(fd)};
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    bool FileHandle::Exists(std::string_view path)
    {
        struct stat64 st{};
        return (stat64(std::string{path}.c_str(), &st) == 0) and S_ISREG(st.st_mode);
    }

    std::expected<void, platform::SystemError> FileHandle::Close()
    {
        assert(this->_handle != nullptr);

        if (close(ToFileDescriptor(this->_handle)) == 0)
        {
            return {};
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    std::expected<int64_t, platform::SystemError> FileHandle::GetLength() const
    {
        assert(this->_handle != nullptr);

        struct stat64 st{};

        if (fstat64(ToFileDescriptor(this->_handle), &st) == 0)
        {
            return st.st_size;
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    std::expected<void, platform::SystemError> FileHandle::SetLength(int64_t length)
    {
        assert(this->_handle != nullptr);

        if (ftruncate64(ToFileDescriptor(this->_handle), length) == 0)
        {
            return {};
        }

        return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
    }

    std::expected<size_t, platform::SystemError> FileHandle::Read(std::span<std::byte> buffer, int64_t position)
    {
        assert(this->_handle != nullptr);
        int const fd = ToFileDescriptor(this->_handle);

        size_t processed = 0;
        while (processed < buffer.size())
        {
            ssize_t const result = pread64(
                fd,
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
        assert(this->_handle != nullptr);
        int const fd = ToFileDescriptor(this->_handle);

        size_t processed = 0;
        while (processed < buffer.size())
        {
            ssize_t const result = pwrite64(
                fd,
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
