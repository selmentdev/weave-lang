#include "weave/platform/compiler.hxx"
#include "weave/platform/windows/string.hxx"
#include "weave/filesystem/FileHandle.hxx"

#include <bit>

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::filesystem
{
    std::expected<FileHandle, platform::SystemError> FileHandle::Create(std::string_view path, FileMode mode, FileAccess access, FileOptions options)
    {
        DWORD dwMode = 0;

        switch (mode)
        {
        case FileMode::CreateNew:
            dwMode = CREATE_NEW;
            break;

        case FileMode::CreateAlways:
            dwMode = CREATE_ALWAYS;
            break;

        default:
        case FileMode::OpenExisting:
            dwMode = OPEN_EXISTING;
            break;

        case FileMode::OpenAlways:
            dwMode = OPEN_ALWAYS;
            break;

        case FileMode::TruncateExisting:
            dwMode = TRUNCATE_EXISTING;
            break;
        }

        DWORD dwAccess = 0;

        switch (access)
        {
        case FileAccess::Read:
            dwAccess = GENERIC_READ;
            break;
        case FileAccess::Write:
            dwAccess = GENERIC_WRITE;
            break;
        case FileAccess::ReadWrite:
            dwAccess = GENERIC_READ | GENERIC_WRITE;
            break;
        }

        DWORD dwShare = 0;

        if (options.ShareRead)
        {
            dwShare |= FILE_SHARE_READ;
        }

        if (options.ShareWrite)
        {
            dwShare |= FILE_SHARE_WRITE;
        }

        DWORD dwFlags = 0;

        if (options.DeleteOnClose)
        {
            dwFlags |= FILE_FLAG_DELETE_ON_CLOSE;
        }

        if (options.RandomAccess)
        {
            dwFlags |= FILE_FLAG_RANDOM_ACCESS;
        }
        else if (options.SequentialScan)
        {
            dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;
        }

        if (options.WriteThrough)
        {
            dwFlags |= FILE_FLAG_WRITE_THROUGH;
        }

        if (options.NoBuffering)
        {
            dwFlags |= FILE_FLAG_NO_BUFFERING;
        }

        platform::StringBuffer<wchar_t, MAX_PATH> wpath{};

        if (platform::WidenString(wpath, path))
        {
            SECURITY_ATTRIBUTES sa{
                .nLength = sizeof(SECURITY_ATTRIBUTES),
                .bInheritHandle = TRUE,
            };

            CREATEFILE2_EXTENDED_PARAMETERS cf{
                .dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS),
                .dwFileAttributes = FILE_ATTRIBUTE_NORMAL,
                .dwFileFlags = dwFlags,
                .dwSecurityQosFlags = SECURITY_SQOS_PRESENT | SECURITY_ANONYMOUS,
                .lpSecurityAttributes = options.Inheritable ? &sa : nullptr,
                .hTemplateFile = nullptr,
            };

            HANDLE const result = CreateFile2(
                wpath.GetBuffer(),
                dwAccess,
                dwShare,
                dwMode,
                &cf);

            if (result != INVALID_HANDLE_VALUE)
            {
                return FileHandle{result};
            }

            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        return std::unexpected(platform::SystemError::NoSuchFileOrDirectory);
    }

    bool FileHandle::Exists(std::string_view path)
    {
        platform::StringBuffer<wchar_t, MAX_PATH> wpath{};

        if (platform::WidenString(wpath, path))
        {
            DWORD const dwAttributes = GetFileAttributesW(wpath.GetBuffer());

            if (dwAttributes != INVALID_FILE_ATTRIBUTES)
            {
                return (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
            }
        }

        return false;
    }

    std::expected<void, platform::SystemError> FileHandle::Close()
    {
        assert(this->_handle != nullptr);

        void* handle = std::exchange(this->_handle, nullptr);

        if (!CloseHandle(handle))
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        return {};
    }

    std::expected<void, platform::SystemError> FileHandle::Flush()
    {
        assert(this->_handle != nullptr);

        if (!FlushFileBuffers(this->_handle))
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        return {};
    }

    std::expected<int64_t, platform::SystemError> FileHandle::GetLength() const
    {
        assert(this->_handle != nullptr);

        LARGE_INTEGER li{};
        if (!GetFileSizeEx(this->_handle, &li))
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        return li.QuadPart;
    }

    std::expected<void, platform::SystemError> FileHandle::SetLength(int64_t length)
    {
        assert(this->_handle != nullptr);

        LARGE_INTEGER const li = std::bit_cast<LARGE_INTEGER>(length);

        BOOL result = SetFilePointerEx(this->_handle, li, nullptr, FILE_BEGIN);

        if (result != FALSE)
        {
            result = SetEndOfFile(this->_handle);
        }

        if (result == FALSE)
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        return {};
    }

    constexpr OVERLAPPED GetOverlappedForPosition(int64_t position)
    {
        LARGE_INTEGER const li = std::bit_cast<LARGE_INTEGER>(position);

        OVERLAPPED overlapped{};
        overlapped.Offset = li.LowPart;
        overlapped.OffsetHigh = li.HighPart;
        return overlapped;
    }

    /*constexpr bool IsEndOfFile(DWORD dwError)
    {
        switch (dwError)
        {
        case ERROR_HANDLE_EOF:
        case ERROR_BROKEN_PIPE:
        case ERROR_PIPE_NOT_CONNECTED:
            return true;
        case ERROR_INVALID_PARAMETER:
            // TODO: This one is reported when FILE_FLAG_NO_BUFFERING was set.
            return true;
        }
    }*/

    constexpr size_t DefaultBufferSize = 1u << 20u;

    std::expected<size_t, platform::SystemError> FileHandle::Read(std::span<std::byte> buffer, int64_t position)
    {
        assert(this->_handle != nullptr);

        size_t processed = 0;

        while (not buffer.empty())
        {
            OVERLAPPED overlapped = GetOverlappedForPosition(position);

            DWORD const dwRequested = static_cast<DWORD>(std::min<size_t>(DefaultBufferSize, buffer.size()));
            DWORD dwProcessed = 0;

            if (ReadFile(this->_handle, buffer.data(), dwRequested, &dwProcessed, &overlapped) == FALSE)
            {
                DWORD const dwError = GetLastError();

                if (dwError == ERROR_HANDLE_EOF)
                {
                    break;
                }

                if (dwError != ERROR_IO_PENDING)
                {
                    return std::unexpected(platform::impl::SystemErrorFromWin32Error(dwError));
                }

                if (GetOverlappedResult(this->_handle, &overlapped, &dwProcessed, TRUE) == FALSE)
                {
                    return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
                }
            }


            processed += dwProcessed;
            buffer = buffer.subspan(dwProcessed);
            position += dwProcessed;
        }

        return processed;
    }

    std::expected<size_t, platform::SystemError> FileHandle::Write(std::span<std::byte const> buffer, int64_t position)
    {
        assert(this->_handle != nullptr);

        size_t processed = 0;

        while (!buffer.empty())
        {
            OVERLAPPED overlapped = GetOverlappedForPosition(position);

            DWORD const dwRequested = static_cast<DWORD>(std::min<size_t>(DefaultBufferSize, buffer.size()));
            DWORD dwProcessed{};

            if (WriteFile(this->_handle, buffer.data(), dwRequested, &dwProcessed, &overlapped) == FALSE)
            {
                DWORD const dwError = ::GetLastError();

                if (dwError == ERROR_HANDLE_EOF)
                {
                    break;
                }

                if (dwError != ERROR_IO_PENDING)
                {
                    return std::unexpected(platform::impl::SystemErrorFromWin32Error(dwError));
                }

                dwProcessed = 0;

                if (GetOverlappedResult(this->_handle, &overlapped, &dwProcessed, TRUE) == FALSE)
                {
                    return std::unexpected(platform::SystemError::IoError);
                }
            }

            buffer = buffer.subspan(dwProcessed);

            processed += dwProcessed;
            position += dwProcessed;
        }

        return processed;
    }
}
