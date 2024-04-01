#include "weave/system/process.hxx"
#include "weave/platform/windows/string.hxx"
#include "weave/platform/compiler.hxx"

#include <fmt/format.h>

#include "weave/bugcheck/BugCheck.hxx"
#include "weave/platform/windows/Helpers.hxx"
#include "weave/platform/windows/PlatformHeaders.hxx"

namespace weave::system::impl
{
    static size_t ReadFromPipe(HANDLE handle, std::string& buffer)
    {
        DWORD dw_available{};

        if (PeekNamedPipe(handle, nullptr, 0, nullptr, &dw_available, nullptr))
        {
            if (dw_available != 0)
            {
                buffer.resize(dw_available);

                DWORD dw_processed{};

                if (ReadFile(handle, buffer.data(), dw_available, &dw_processed, nullptr))
                {
                    assert(dw_available == dw_processed);
                    return dw_processed;
                }
            }
        }

        return 0;
    }

    template <typename CallbackT>
    struct defer final
    {
    private:
        CallbackT _callback;

    public:
        defer() = delete;
        defer(defer const&) = delete;
        defer(defer&&) = delete;
        defer& operator=(defer const&) = delete;
        defer& operator=(defer&&) = delete;

        defer(CallbackT&& other)
            : _callback{std::move(other)}
        {
        }

        ~defer()
        {
            this->_callback();
        }
    };

    template <typename CallbackT>
    defer(CallbackT&& other) -> defer<CallbackT>;

    struct scope_close_handle final
    {
        HANDLE Handle;

        ~scope_close_handle()
        {
            if (this->Handle != nullptr)
            {
                CloseHandle(this->Handle);
            }
        }
    };
}

namespace weave::system
{
    std::expected<int, platform::SystemError> Execute(
        const char* path,
        const char* args,
        const char* working_directory,
        std::string& output,
        std::string& error)
    {
        platform::windows::win32_string_buffer<wchar_t, 512> wide_path{};
        platform::windows::win32_string_buffer<wchar_t, 512> wide_command_line{};
        platform::windows::win32_string_buffer<wchar_t, 512> wide_working_directory{};

        if (not platform::windows::win32_WidenString(wide_path, path))
        {
            return std::unexpected(platform::SystemError::InvalidArgument);
        }

        // C runtime expects that first argument is the name of the executable.
        std::string const command_line = (args != nullptr)
            ? fmt::format("\"{}\" {}", path, args)
            : fmt::format("\"{}\"", path);

        if (not platform::windows::win32_WidenString(wide_command_line, command_line))
        {
            return std::unexpected(platform::SystemError::InvalidArgument);
        }

        if (not platform::windows::win32_WidenString(wide_working_directory, working_directory ? working_directory : ""))
        {
            return std::unexpected(platform::SystemError::InvalidArgument);
        }

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        impl::scope_close_handle pipe_output_read{};
        impl::scope_close_handle pipe_output_write{};
        impl::scope_close_handle pipe_error_read{};
        impl::scope_close_handle pipe_error_write{};

        if (not CreatePipe(&pipe_output_read.Handle, &pipe_output_write.Handle, &sa, 0))
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        if (not CreatePipe(&pipe_error_read.Handle, &pipe_error_write.Handle, &sa, 0))
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
        }

        STARTUPINFOW si{
            .cb = sizeof(STARTUPINFOW),
            .lpReserved = nullptr,
            .lpDesktop = nullptr,
            .lpTitle = nullptr,
            .dwX = static_cast<DWORD>(CW_USEDEFAULT),
            .dwY = static_cast<DWORD>(CW_USEDEFAULT),
            .dwXSize = static_cast<DWORD>(CW_USEDEFAULT),
            .dwYSize = static_cast<DWORD>(CW_USEDEFAULT),
            .dwXCountChars = 0,
            .dwYCountChars = 0,
            .dwFillAttribute = 0,
            .dwFlags = STARTF_USESTDHANDLES,
            .cbReserved2 = 0,
            .lpReserved2 = nullptr,
            .hStdInput = GetStdHandle(STD_INPUT_HANDLE),
            .hStdOutput = pipe_output_write.Handle,
            .hStdError = pipe_error_write.Handle,
        };

        DWORD constexpr flags = CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT;

        PROCESS_INFORMATION pi{};

        if (CreateProcessW(
                path != nullptr ? wide_path.data() : nullptr,
                args != nullptr ? wide_command_line.data() : nullptr,
                &sa,
                &sa,
                TRUE,
                flags,
                nullptr,
                working_directory != nullptr ? wide_working_directory.data() : nullptr,
                &si,
                &pi) != FALSE)
        {
            CloseHandle(pi.hThread);

            DWORD dw_exit_code{};

            DWORD dw_result;

            std::string buffer{};

            do
            {
                while (impl::ReadFromPipe(pipe_output_read.Handle, buffer) != 0)
                {
                    output.append(buffer);
                }

                while (impl::ReadFromPipe(pipe_error_read.Handle, buffer) != 0)
                {
                    error.append(buffer);
                }

                // Idle for a while
                if (SwitchToThread() == FALSE)
                {
                    SleepEx(0, TRUE);
                }

                dw_result = WaitForSingleObject(pi.hProcess, 0);

                bool const active = GetExitCodeProcess(pi.hProcess, &dw_exit_code) != FALSE;

                if (active and (dw_exit_code != STILL_ACTIVE))
                {
                    break;
                }
            } while (dw_result == WAIT_TIMEOUT);


            // Finish reading data from pipes
            while (impl::ReadFromPipe(pipe_output_read.Handle, buffer) != 0)
            {
                output.append(buffer);
            }

            while (impl::ReadFromPipe(pipe_error_read.Handle, buffer) != 0)
            {
                error.append(buffer);
            }

            CloseHandle(pi.hProcess);

            return static_cast<int>(dw_exit_code);
        }

        return std::unexpected(platform::impl::SystemErrorFromWin32Error(GetLastError()));
    }
}

namespace weave::system
{
    static const std::string g_ExecutablePath = []() -> std::string
    {
        platform::windows::win32_FilePathW buffer{};

        if (not win32_QueryFullProcessImageName(::GetCurrentProcess(), buffer))
        {
            WEAVE_BUGCHECK("Failed to get executable path");
        }

        std::string narrow{};

        if (not platform::windows::win32_NarrowString(narrow, buffer.data()))
        {
            WEAVE_BUGCHECK("Failed to get executable path");
        }

        return narrow;
    }();

    static const std::string g_StartupPath = []()->std::string
    {
        platform::windows::win32_FilePathW buffer{};

        if (not win32_GetCurrentDirectory(buffer))
        {
            WEAVE_BUGCHECK("Failed to get current directory");
        }

        std::string narrow{};

        if (not platform::windows::win32_NarrowString(narrow, buffer.data()))
        {
            WEAVE_BUGCHECK("Failed to get current directory");
        }

        size_t const last_slash = narrow.find_last_of('\\');

        if (last_slash == std::string::npos)
        {
            return {};
        }

        return narrow.substr(0, last_slash);
    }();

    std::string_view GetExecutablePath()
    {
        return g_ExecutablePath;
    }

    std::string_view GetStartupDirectory()
    {
        std::string_view const narrow = g_StartupPath;

        size_t const last_slash = narrow.find_last_of('\\');

        if (last_slash == std::string::npos)
        {
            return {};
        }

        return narrow.substr(0, last_slash);
    }
}
