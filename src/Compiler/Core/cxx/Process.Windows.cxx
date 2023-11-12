#include "Weave.Core/Process.hxx"

#if defined(WIN32)

#include "Weave.Core/IO/Pipe.hxx"
#include "Weave.Core/Platform/Compiler.hxx"
#include "Weave.Core/Platform/Windows.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace Weave
{
    std::optional<int> Execute(
        const char* path,
        const char* args,
        const char* workingDirectory,
        std::string& output,
        std::string& error)
    {

        SECURITY_ATTRIBUTES sa{
            .nLength = sizeof(SECURITY_ATTRIBUTES),
            .lpSecurityDescriptor = nullptr,
            .bInheritHandle = TRUE,
        };

        IO::Pipe pipeOutput{};
        IO::Pipe pipeError{};

        if (auto pipe = IO::Pipe::Create(); pipe.has_value())
        {
            pipeOutput = std::move(*pipe);
        }
        else
        {
            return std::nullopt;
        }

        if (auto pipe = IO::Pipe::Create(); pipe.has_value())
        {
            pipeError = std::move(*pipe);
        }
        else
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
            .hStdOutput = pipeOutput.GetWriteHandle(),
            .hStdError = pipeError.GetWriteHandle(),
        };

        DWORD constexpr flags = CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT;

        StringBuffer<wchar_t, 512> wPath{};
        StringBuffer<wchar_t, 512> wArgs{};
        StringBuffer<wchar_t, 512> wWorkingDirectory{};

        if (!WidenString(wPath, path))
        {
            return std::nullopt;
        }

        if (!WidenString(wArgs, args ? args : ""))
        {
            return std::nullopt;
        }

        if (!WidenString(wWorkingDirectory, workingDirectory ? workingDirectory : ""))
        {
            return std::nullopt;
        }

        PROCESS_INFORMATION pi{};

        if (CreateProcessW(
                path != nullptr ? wPath.GetBuffer() : nullptr,
                args != nullptr ? wArgs.GetBuffer() : nullptr,
                &sa,
                &sa,
                TRUE,
                flags,
                nullptr,
                workingDirectory != nullptr ? wWorkingDirectory.GetBuffer() : nullptr,
                &si,
                &pi) != FALSE)
        {
            CloseHandle(pi.hThread);

            DWORD dwExitCode{};

            DWORD dwResult{};

            std::string bufferOutput{};
            std::string bufferError{};

            do
            {
                while (Read(pipeOutput, bufferOutput).value_or(0) > 0)
                {
                    output.append(bufferOutput);
                }

                while (Read(pipeError, bufferError).value_or(0) > 0)
                {
                    error.append(bufferError);
                }

                // Idle for a while
                if (SwitchToThread() == FALSE)
                {
                    SleepEx(0, TRUE);
                }

                dwResult = WaitForSingleObject(pi.hProcess, 0);

                bool const active = GetExitCodeProcess(pi.hProcess, &dwExitCode) != FALSE;

                if (active and (dwExitCode != STILL_ACTIVE))
                {
                    break;
                }
            } while (dwResult == WAIT_TIMEOUT);


            // Finish reading data from pipes
            while (Read(pipeOutput, bufferOutput).value_or(0) > 0)
            {
                output.append(bufferOutput);
            }

            while (Read(pipeError, bufferError).value_or(0) > 0)
            {
                error.append(bufferError);
            }

            CloseHandle(pi.hProcess);

            return static_cast<int>(dwExitCode);
        }

        return std::nullopt;
    }
}

#endif
