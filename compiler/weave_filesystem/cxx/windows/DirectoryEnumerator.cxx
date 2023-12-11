#include "weave/platform/Compiler.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/bugcheck/BugCheck.hxx"
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
        new (&this->AsPlatform()) impl::PlatformDirectoryEnumerator();
    }

    DirectoryEnumerator::DirectoryEnumerator(DirectoryEnumerator&& other)
        : _root{std::exchange(other._root, {})}
    {
        new (&this->AsPlatform()) impl::PlatformDirectoryEnumerator(other.AsPlatform());
        other.AsPlatform() = {};
    }

    DirectoryEnumerator& DirectoryEnumerator::operator=(DirectoryEnumerator&& other)
    {
        if (this != std::addressof(other))
        {
            impl::PlatformDirectoryEnumerator& self = this->AsPlatform();

            if (self.Handle != nullptr)
            {
                if (FindClose(self.Handle) == FALSE)
                {
                    WEAVE_BUGCHECK("Failed to close directory enumerator");
                }
            }

            this->_root = std::exchange(other._root, {});
            self = std::exchange(other.AsPlatform(), {});
        }

        return (*this);
    }

    DirectoryEnumerator::~DirectoryEnumerator()
    {
        impl::PlatformDirectoryEnumerator& self = this->AsPlatform();

        if (self.Handle != nullptr)
        {
            if (FindClose(self.Handle) == FALSE)
            {
                WEAVE_BUGCHECK("Failed to close directory enumerator");
            }
        }

        this->AsPlatform().~PlatformDirectoryEnumerator();
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

    static DirectoryEntry FromNative(std::string_view root, WIN32_FIND_DATAW const& wfd)
    {
        platform::StringBuffer<char, 512> narrow{};
        platform::NarrowString(narrow, wfd.cFileName);

        std::string path{root};
        path::Append(path, narrow.AsView());

        return DirectoryEntry{
            .Path = std::move(path),
            .Type = impl::ConvertToFileType(wfd.dwFileAttributes),
        };
    }

    std::optional<std::expected<DirectoryEntry, platform::SystemError>> DirectoryEnumerator::Next()
    {
        impl::PlatformDirectoryEnumerator& state = this->AsPlatform();

        WIN32_FIND_DATAW wfd;

        if (state.Handle == nullptr)
        {
            if (auto&& converted = WidenString(this->_root))
            {
                std::wstring wpath = std::move(*converted);

                if (not wpath.empty())
                {
                    if (wchar_t const last = wpath.back(); (last != L'/') and (last != L'\\'))
                    {
                        wpath += L'/';
                    }

                    wpath += L'*';

                    if (HANDLE const handle = FindFirstFileW(wpath.c_str(), &wfd); handle != INVALID_HANDLE_VALUE)
                    {
                        state.Handle = handle;

                        return FromNative(this->_root, wfd);
                    }

                    if (DWORD const dwError = GetLastError(); dwError == ERROR_FILE_NOT_FOUND)
                    {
                        return std::nullopt;
                    }
                    else
                    {
                        return std::unexpected(platform::impl::SystemErrorFromWin32Error(dwError));
                    }
                }
            }

            // Could not convert root string.
            return std::unexpected(platform::SystemError::InvalidArgument);
        }

        if (FindNextFileW(state.Handle, &wfd) != FALSE)
        {
            return FromNative(this->_root, wfd);
        }

        if (DWORD const dwError = GetLastError(); dwError == ERROR_NO_MORE_FILES)
        {
            return std::nullopt;
        }
        else
        {
            return std::unexpected(platform::impl::SystemErrorFromWin32Error(dwError));
        }
    }
}

