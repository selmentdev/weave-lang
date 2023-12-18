#pragma once
#include "weave/platform/windows/String.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <ShlObj.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::platform::windows
{
    template <size_t CapacityT>
    bool win32_QueryFullProcessImageName(HANDLE hProcess, win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD dwSize = static_cast<DWORD>(buffer.size());

                if (QueryFullProcessImageNameW(hProcess, 0, buffer.data(), &dwSize))
                {
                    capacity = dwSize + 1;
                    return true;
                }

                if (DWORD const dwError = GetLastError(); dwError == ERROR_INSUFFICIENT_BUFFER)
                {
                    capacity = static_cast<size_t>(dwSize) * 2;
                    return true;
                }

                capacity = 0;
                return false;
            });
    }

    template <size_t CapacityT>
    bool win32_GetCurrentDirectory(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
            {
                if (DWORD const dwLength = GetCurrentDirectoryW(static_cast<DWORD>(buffer.size()), buffer.data()); dwLength != 0)
                {
                    capacity = static_cast<size_t>(dwLength) + 1;
                    return true;
                }

                capacity = 0;
                return false;
            });
    }

    template <typename CallbackT = void(std::wstring_view)>
    bool win32_GetKnownFolderPath(KNOWNFOLDERID const& id, CallbackT&& callback) noexcept
    {
        PWSTR result{};

        if (HRESULT const hr = SHGetKnownFolderPath(id, 0, nullptr, &result); SUCCEEDED(hr))
        {
            if (result != nullptr)
            {
                callback(std::wstring_view{result});
                CoTaskMemFree(result);
                return true;
            }
        }

        return false;
    }

    template <size_t CapacityT>
    bool win32_GetEnvironmentVariable(win32_string_buffer<wchar_t, CapacityT>& result, const wchar_t* name) noexcept
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
            {
                SetLastError(ERROR_SUCCESS);
                DWORD dwLength = GetEnvironmentVariableW(name, buffer.data(), static_cast<DWORD>(buffer.size()));

                if ((dwLength == 0) and GetLastError() != ERROR_SUCCESS)
                {
                    capacity = 0;
                    return false;
                }

                if (dwLength < buffer.size())
                {
                    ++dwLength;
                }

                capacity = dwLength;
                return true;
            });
    }

    template <size_t CapacityT>
    bool win32_QueryRegistry(win32_string_buffer<wchar_t, CapacityT>& result, HKEY key, const wchar_t* subkey, const wchar_t* name) noexcept
    {
        if ((key == nullptr) or (subkey == nullptr) or (name == nullptr))
        {
            return false;
        }

        bool succeeded = false;

        for (int32_t const flag : {KEY_WOW64_32KEY, KEY_WOW64_64KEY})
        {
            if (succeeded)
            {
                break;
            }

            HKEY current = nullptr;

            if (RegOpenKeyExW(key, subkey, 0, static_cast<REGSAM>(KEY_READ | flag), &current) == ERROR_SUCCESS)
            {
                if (win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
                        {
                            DWORD const dwLength = static_cast<DWORD>(buffer.size());

                            DWORD dwSize = dwLength * sizeof(wchar_t);
                            LSTATUS const status = RegQueryValueExW(
                                current,
                                name,
                                nullptr,
                                nullptr,
                                reinterpret_cast<LPBYTE>(buffer.data()),
                                &dwSize);

                            if ((status == ERROR_MORE_DATA) or (status == ERROR_SUCCESS))
                            {
                                DWORD const dwFinal = (dwSize / sizeof(wchar_t));
                                capacity = dwFinal + 1;
                                return true;
                            }

                            capacity = 0;
                            return false;
                        }))
                {
                    succeeded = true;
                }

                RegCloseKey(current);
            }
        }

        return succeeded;
    }

    template <size_t CapacityT>
    bool win32_GetSystemDirectory(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
            {
                UINT const length = GetSystemDirectoryW(buffer.data(), static_cast<UINT>(buffer.size()));

                if (length == 0)
                {
                    capacity = 0;
                    return false;
                }

                capacity = length + 1;
                return true;
            });
    }

    template <size_t CapacityT>
    bool win32_GetUserDefaultLocaleName(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        std::span<wchar_t> writable = result.as_span();
        if (int const length = GetUserDefaultLocaleName(writable.data(), static_cast<int>(writable.size())); length > 0)
        {
            result.trim(static_cast<size_t>(length));
            return true;
        }

        result.trim(0);
        return false;
    }

    template <size_t CapacityT>
    bool win32_GetComputerName(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD dwSize = static_cast<DWORD>(buffer.size());
                if (GetComputerNameW(buffer.data(), &dwSize))
                {
                    // Trimmed
                    capacity = static_cast<size_t>(dwSize) + 1;
                    return true;
                }

                if (GetLastError() == ERROR_BUFFER_OVERFLOW)
                {
                    // Retry with that buffer size.
                    capacity = dwSize;
                    return true;
                }

                capacity = 0;
                return false;
            });
    }

    template <size_t CapacityT>
    bool win32_GetUserName(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD dwSize = static_cast<DWORD>(buffer.size());
                if (GetUserNameW(buffer.data(), &dwSize))
                {
                    // Trimmed
                    capacity = dwSize;
                    return true;
                }

                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                {
                    // Retry with that buffer size.
                    capacity = dwSize;
                    return true;
                }

                capacity = 0;
                return false;
            });
    }

    template <size_t CapacityT>
    bool win32_GetTempPath(win32_string_buffer<wchar_t, CapacityT>& result) noexcept
    {
        return win32_adapt_string_buffer(result, [](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD const length = GetTempPathW(static_cast<DWORD>(buffer.size()), buffer.data());

                if (length != 0)
                {
                    capacity = static_cast<size_t>(length) + 1;
                    return true;
                }

                capacity = 0;
                return false;
            });
    }

    template <size_t CapacityT>
    bool win32_GetLongPathName(win32_string_buffer<wchar_t, CapacityT>& result, const wchar_t* path) noexcept
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
            {
                DWORD const length = GetLongPathNameW(path, buffer.data(), static_cast<DWORD>(buffer.size()));

                if (length != 0)
                {
                    capacity = length;
                    return true;
                }

                capacity = 0;
                return false;
            });
    }

    template <size_t CapacityT>
    bool win32_GetUserPreferredUILanguages(win32_string_buffer<wchar_t, CapacityT>& result, DWORD flags, ULONG& numLanguages)
    {
        return win32_adapt_string_buffer(result, [&](std::span<wchar_t> buffer, size_t& capacity)
            {
                ULONG uSize = static_cast<ULONG>(buffer.size());

                if (GetUserPreferredUILanguages(flags, &numLanguages, buffer.data(), &uSize))
                {
                    // Trim to result.
                    capacity = uSize;
                    return true;
                }

                if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
                {
                    // Retry with 2x buffer size.
                    capacity = buffer.size() * 2;
                    return true;
                }

                capacity = 0;
                return false;
            });
    }
}
