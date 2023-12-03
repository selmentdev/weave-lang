#include "weave/platform/Compiler.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/BugCheck.hxx"
#include "weave/platform/windows/String.hxx"

#include <optional>
#include <utility>

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::filesystem::impl
{
    struct PlatformDirectoryEnumerator
    {
        HANDLE Handle;
    };

    inline FileType ConvertToFileType(DWORD attributes)
    {
        if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0)
        {
            return FileType::SymbolicLink;
        }

        if ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
        {
            return FileType::Directory;
        }

        return FileType::File;
    }
}

namespace weave::filesystem
{
    DirectoryEnumerator::DirectoryEnumerator(std::string_view path)
        : _root{path}
    {
    }

    DirectoryEnumerator::DirectoryEnumerator(DirectoryEnumerator&& other)
        : _root{std::exchange(other._root, {})}
        , _type{std::exchange(other._type, {})}
        , _name{std::exchange(other._name, {})}
    {
        this->AsPlatform().Handle = std::exchange(other.AsPlatform().Handle, nullptr);
    }

    DirectoryEnumerator& DirectoryEnumerator::operator=(DirectoryEnumerator&& other)
    {
        if (this != std::addressof(other))
        {
            if (this->AsPlatform().Handle != nullptr)
            {
                CloseHandle(this->AsPlatform().Handle);
            }

            this->_native = std::exchange(other._native, {});
            this->_root = std::exchange(other._root, {});
            this->_type = std::exchange(other._type, {});
            this->_name = std::exchange(other._name, {});
        }

        return (*this);
    }

    DirectoryEnumerator::~DirectoryEnumerator()
    {
        if (HANDLE const handle = this->AsPlatform().Handle; handle != nullptr)
        {
            if (FindClose(handle) == FALSE)
            {
                WEAVE_BUGCHECK("Failed to close directory enumerator");
            }
        }
    }


    std::optional<std::wstring> WidenString(std::string_view value)
    {
        std::wstring result;
        if (platform::WidenStringImpl(
                &result,
                [](void* context, size_t const size)
                {
                    static_cast<std::wstring*>(context)->resize(size);
                },
                [](void* context) -> std::span<wchar_t>
                {
                    return std::span{*static_cast<std::wstring*>(context)};
                },
                [](void* context, size_t const size)
                {
                    static_cast<std::wstring*>(context)->resize(size);
                },
                value))
        {
            return result;
        }

        return {};
    }

    bool DirectoryEnumerator::MoveNext()
    {
        WIN32_FIND_DATAW current;
        HANDLE& handle = this->AsPlatform().Handle;

        bool success = false;

        if (handle == nullptr)
        {
            if (auto&& converted = WidenString(this->_root))
            {
                std::wstring wpath = std::move(*converted);

                if (not wpath.empty())
                {
                    if (wchar_t const last = wpath.back(); last != L'/' and last != L'\\')
                    {
                        wpath += L'/';
                    }

                    wpath += L'*';
                    handle = FindFirstFileW(wpath.c_str(), &current);

                    success = (handle != INVALID_HANDLE_VALUE);
                }
            }
        }
        else if (FindNextFileW(handle, &current) != FALSE)
        {
            success = true;
        }

        if (success)
        {
            platform::StringBuffer<char, 512> narrow{};
            platform::NarrowString(narrow, current.cFileName);
            this->_name.assign(narrow.AsView());
            this->_type = impl::ConvertToFileType(current.dwFileAttributes);
        }

        return success;
    }
}
