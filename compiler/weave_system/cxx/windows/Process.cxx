#include "weave/system/process.hxx"
#include "weave/platform/windows/string.hxx"
#include "weave/platform/compiler.hxx"

#include <fmt/format.h>

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::system::impl
{
    static size_t read_from_pipe(HANDLE handle, std::string& buffer)
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
        HANDLE _handle;

        ~scope_close_handle()
        {
            if (this->_handle != nullptr)
            {
                CloseHandle(this->_handle);
            }
        }
    };
}

namespace weave::system
{
    std::optional<int> Execute(
        const char* path,
        const char* args,
        const char* working_directory,
        std::string& output,
        std::string& error)
    {

        platform::StringBuffer<wchar_t, 512> wide_path{};
        platform::StringBuffer<wchar_t, 512> wide_command_line{};
        platform::StringBuffer<wchar_t, 512> wide_working_directory{};

        if (not platform::widen_string(wide_path, path))
        {
            return std::nullopt;
        }

        // C runtime expects that first argument is the name of the executable.
        std::string const command_line = (args != nullptr)
            ? fmt::format("\"{}\" {}", path, args)
            : fmt::format("\"{}\"", path);

        if (not platform::widen_string(wide_command_line, command_line))
        {
            return std::nullopt;
        }

        if (not platform::widen_string(wide_working_directory, working_directory ? working_directory : ""))
        {
            return std::nullopt;
        }

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        HANDLE pipe_output_read{};
        HANDLE pipe_output_write{};
        HANDLE pipe_error_read{};
        HANDLE pipe_error_write{};

        impl::defer closeOutRead{[&]()
            {
                if (pipe_output_read != nullptr)
                {
                    CloseHandle(pipe_output_read);
                }
            }};
        impl::defer closeOutWrite{[&]()
            {
                if (pipe_output_write != nullptr)
                {
                    CloseHandle(pipe_output_write);
                }
            }};
        impl::defer closeErrRead{[&]()
            {
                if (pipe_error_read != nullptr)
                {
                    CloseHandle(pipe_error_read);
                }
            }};
        impl::defer closeErrWrite{[&]()
            {
                if (pipe_error_write != nullptr)
                {
                    CloseHandle(pipe_error_write);
                }
            }};

        if (not CreatePipe(&pipe_output_read, &pipe_output_write, &sa, 0))
        {
            return std::nullopt;
        }

        if (not CreatePipe(&pipe_error_read, &pipe_error_write, &sa, 0))
        {
            return std::nullopt;
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
            .hStdOutput = pipe_output_write,
            .hStdError = pipe_error_write,
        };

        DWORD constexpr flags = CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT;

        PROCESS_INFORMATION pi{};

        if (CreateProcessW(
                path != nullptr ? wide_path.get_buffer() : nullptr,
                args != nullptr ? wide_command_line.get_buffer() : nullptr,
                &sa,
                &sa,
                TRUE,
                flags,
                nullptr,
                working_directory != nullptr ? wide_working_directory.get_buffer() : nullptr,
                &si,
                &pi) != FALSE)
        {
            CloseHandle(pi.hThread);

            DWORD dw_exit_code{};

            DWORD dw_result;

            std::string buffer{};

            do
            {
                while (impl::read_from_pipe(pipe_output_read, buffer) != 0)
                {
                    output.append(buffer);
                }

                while (impl::read_from_pipe(pipe_error_read, buffer) != 0)
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
            while (impl::read_from_pipe(pipe_output_read, buffer) != 0)
            {
                output.append(buffer);
            }

            while (impl::read_from_pipe(pipe_error_read, buffer) != 0)
            {
                error.append(buffer);
            }

            CloseHandle(pi.hProcess);

            return static_cast<int>(dw_exit_code);
        }

        return std::nullopt;
    }
}
