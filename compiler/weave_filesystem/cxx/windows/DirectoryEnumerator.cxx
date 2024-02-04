#include "weave/platform/Compiler.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/bugcheck/BugCheck.hxx"
#include "weave/platform/windows/String.hxx"
#include "weave/core/String.hxx"

#include <optional>
#include <utility>

#include "weave/platform/windows/PlatformHeaders.hxx"
#include "Common.hxx"

namespace weave::filesystem::impl
{
    struct PlatformDirectoryEnumerator
    {
        HANDLE Handle;
    };
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

    static DirectoryEntry FromNative(std::string_view root, WIN32_FIND_DATAW const& wfd)
    {
        platform::windows::win32_string_buffer<char, 512> narrow{};
        platform::windows::win32_NarrowString(narrow, wfd.cFileName);

        std::string path{root};
        path::Push(path, narrow.as_view());

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
            std::wstring wpath{};
            if (auto&& converted = platform::windows::win32_WidenString(wpath, this->_root))
            {
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

    auto EnumerateDirectoryRecursive(std::string_view root, std::string_view pattern) -> std::expected<std::vector<std::string>, platform::SystemError>
    {
        std::vector<DirectoryEnumerator> stack{};

        stack.emplace_back(root);
        std::vector<std::string> result{};

        while (not stack.empty())
        {
            DirectoryEnumerator& enumerator = stack.back();
            if (std::optional<std::expected<DirectoryEntry, platform::SystemError>> const& next = enumerator.Next(); next)
            {
                std::expected<DirectoryEntry, platform::SystemError> const& item = *next;

                if (item)
                {
                    DirectoryEntry const& entry = *item;

                    if (entry.Type == FileType::Directory)
                    {
                        if (not entry.Path.ends_with("/..") and not entry.Path.ends_with("/."))
                        {
                            stack.emplace_back(entry.Path);
                        }
                    }
                    else if (entry.Type == FileType::File)
                    {
                        if (core::MatchWildcard<char>(pattern, entry.Path))
                        {
                            result.push_back(entry.Path);
                        }
                    }
                }
                else
                {
                    stack.pop_back();
                    // Happily ignore errors here
                }
            }
            else
            {
                stack.pop_back();
            }
        }

        return result;
    }
}

