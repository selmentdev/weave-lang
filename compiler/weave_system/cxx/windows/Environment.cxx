#include "weave/system/Environment.hxx"
#include "weave/platform/windows/Helpers.hxx"

namespace weave::system::environment
{
    bool GetVariable(std::string& result, std::string_view name)
    {
        result.clear();

        platform::windows::win32_string_buffer<wchar_t, 128> wname{};
        platform::windows::win32_WidenString(wname, name);

        platform::windows::win32_string_buffer<wchar_t, 128> wresult{};

        if (platform::windows::win32_GetEnvironmentVariable(wresult, wname.c_str()))
        {
            return platform::windows::win32_NarrowString(result, wresult.as_view());
        }

        return false;
    }

    std::optional<std::string> GetVariable(std::string_view name)
    {
        std::string result{};

        if (GetVariable(result, name))
        {
            return result;
        }

        return std::nullopt;
    }

    bool SetVariable(std::string_view name, std::string_view value)
    {
        platform::windows::win32_string_buffer<wchar_t, 128> wname{};
        platform::windows::win32_WidenString(wname, name);

        platform::windows::win32_string_buffer<wchar_t, 128> wvalue{};
        platform::windows::win32_WidenString(wvalue, value);

        return SetEnvironmentVariableW(wname.c_str(), wvalue.c_str()) != FALSE;
    }

    bool RemoveVariable(std::string_view name)
    {
        platform::windows::win32_string_buffer<wchar_t, 128> wname{};
        platform::windows::win32_WidenString(wname, name);

        return SetEnvironmentVariableW(wname.c_str(), nullptr) != FALSE;
    }
}
