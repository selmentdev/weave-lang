#include "weave/platform/Compiler.hxx"

#include "weave/platform/windows/String.hxx"
#include "weave/platform/windows/PlatformHeaders.hxx"

namespace weave::platform::windows
{
    bool win32_WidenString(std::wstring& result, std::string_view value) noexcept
    {
        int const length = static_cast<int>(value.size());

        if (length > 0)
        {
            int const required = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0);

            if (required == 0)
            {
                result.clear();
                return false;
            }

            result.resize_and_overwrite(static_cast<size_t>(required), [&](wchar_t* ptr, size_t size) -> size_t
                {
                    int const processed = MultiByteToWideChar(
                        CP_UTF8,
                        0,
                        value.data(),
                        static_cast<int>(value.size()),
                        ptr,
                        static_cast<int>(size));

                    assert(required == processed);
                    return static_cast<size_t>(processed);
                });
            return true;
        }

        result.clear();
        return true;
    }

    bool win32_NarrowString(std::string& result, std::wstring_view value) noexcept
    {
        if (not value.empty())
        {
            int const required = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                static_cast<int>(value.size()),
                nullptr,
                0,
                nullptr,
                nullptr);

            if (required == 0)
            {
                result.clear();
                return false;
            }

            result.resize_and_overwrite(static_cast<size_t>(required), [&](char* ptr, size_t size) -> size_t
                {
                    int const processed = WideCharToMultiByte(
                        CP_UTF8,
                        0,
                        value.data(),
                        static_cast<int>(value.size()),
                        ptr,
                        static_cast<int>(size),
                        nullptr,
                        nullptr);

                    assert(required == processed);
                    return static_cast<size_t>(processed);
                });
            return true;
        }

        result.clear();
        return true;
    }
}
