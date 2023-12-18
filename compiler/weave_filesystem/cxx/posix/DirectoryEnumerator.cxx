#include "weave/platform/Compiler.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/bugcheck/BugCheck.hxx"
#include "weave/platform/SystemError.hxx"

#include "Common.hxx"

#include <utility>

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <dirent.h>

WEAVE_EXTERNAL_HEADERS_END

namespace weave::filesystem::impl
{
    struct PlatformDirectoryEnumerator
    {
        DIR* Handle;
    };
}

namespace weave::filesystem
{
    DirectoryEnumerator::DirectoryEnumerator(std::string_view path)
        : _root{path}
    {
        this->AsPlatform().Handle = nullptr;
    }

    DirectoryEnumerator::DirectoryEnumerator(DirectoryEnumerator&& other)
        : _root{std::exchange(other._root, {})}
    {
        this->AsPlatform().Handle = std::exchange(other.AsPlatform().Handle, nullptr);
    }

    DirectoryEnumerator& DirectoryEnumerator::operator=(DirectoryEnumerator&& other)
    {
        if (this != std::addressof(other))
        {
            if (this->AsPlatform().Handle != nullptr)
            {
                closedir(this->AsPlatform().Handle);
            }

            this->_native = std::exchange(other._native, {});
            this->_root = std::exchange(other._root, {});
        }

        return (*this);
    }

    DirectoryEnumerator::~DirectoryEnumerator()
    {
        if (this->AsPlatform().Handle != nullptr)
        {
            closedir(this->AsPlatform().Handle);
        }
    }

    std::optional<std::expected<DirectoryEntry, platform::SystemError>> DirectoryEnumerator::Next()
    {
        if (this->AsPlatform().Handle == nullptr)
        {
            DIR* handle = opendir(this->_root.c_str());

            if (handle != nullptr)
            {
                this->AsPlatform().Handle = handle;
            }
            else
            {
                return std::unexpected(platform::impl::SystemErrorFromErrno(errno));
            }
        }


        if (this->AsPlatform().Handle != nullptr)
        {
            auto& error = errno;

            error = 0;

            if (dirent* current = readdir(this->AsPlatform().Handle); current != nullptr)
            {
                return DirectoryEntry{
                    .Path = current->d_name,
                    .Type = impl::ConvertToFileType(current->d_type),
                };
            }
            else if (error == 0)
            {
                return std::unexpected(platform::impl::SystemErrorFromErrno(error));
            }
        }

        return std::nullopt;
    }
}
