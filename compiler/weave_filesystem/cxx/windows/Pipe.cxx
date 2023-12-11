#include "weave/filesystem/Pipe.hxx"
#include "weave/bugcheck/Assert.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::filesystem
{
    void Pipe::CloseRead()
    {
        if (this->_read != nullptr)
        {
            CloseHandle(this->_read);
            this->_read = nullptr;
        }
    }

    void Pipe::CloseWrite()
    {
        if (this->_write != nullptr)
        {
            CloseHandle(this->_write);
            this->_write = nullptr;
        }
    }

    std::expected<Pipe, platform::SystemError> Pipe::Create()
    {
        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        void* read{};
        void* write{};

        if (!CreatePipe(&read, &write, &sa, 0))
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        return Pipe{read, write};
    }

    std::expected<size_t, platform::SystemError> Pipe::Read(std::span<std::byte> buffer)
    {
        WEAVE_ASSERT(this->_read != nullptr);
        WEAVE_ASSERT(buffer.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));

        DWORD const dwSize = static_cast<DWORD>(buffer.size());

        if (DWORD dwProcessed{}; ReadFile(this->_read, buffer.data(), dwSize, &dwProcessed, nullptr) != FALSE)
        {
            return dwProcessed;
        }

        DWORD const dwError = GetLastError();

        if ((dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
        {
            return 0;
        }

        return std::unexpected(platform::impl::SystemErrorFromWin32Error(dwError));
    }

    std::expected<size_t, platform::SystemError> Pipe::Write(std::span<std::byte const> buffer)
    {
        WEAVE_ASSERT(this->_write != nullptr);
        WEAVE_ASSERT(buffer.size() <= static_cast<size_t>(std::numeric_limits<int32_t>::max()));

        DWORD const dwSize = static_cast<DWORD>(buffer.size());

        if (DWORD dwProcessed{}; WriteFile(this->_write, buffer.data(), dwSize, &dwProcessed, nullptr) != FALSE)
        {
            return dwProcessed;
        }

        DWORD const dwError = GetLastError();

        if ((dwError == ERROR_BROKEN_PIPE) or (dwError == ERROR_NO_DATA))
        {
            return 0;
        }

        return std::unexpected(platform::impl::SystemErrorFromWin32Error(dwError));
    }

    std::expected<size_t, platform::SystemError> Pipe::BytesAvailable() const
    {
        WEAVE_ASSERT(this->_read != nullptr);

        if (DWORD dwAvailable{}; PeekNamedPipe(this->_read, nullptr, 0, nullptr, &dwAvailable, nullptr))
        {
            return dwAvailable;
        }

        return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
    }
}
