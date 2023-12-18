#pragma once
#include "weave/platform/SystemError.hxx"
#include "weave/filesystem/Path.hxx"
#include "weave/filesystem/FileType.hxx"

#include <expected>
#include <string>
#include <optional>
#include <string_view>

namespace weave::filesystem::impl
{
    struct PlatformDirectoryEnumerator;

    struct NativeDirectoryEnumerator
    {
#if defined(WIN32)
        void* Native[1];
#elif defined(__linux__)
#else
#error "Not implemented"
#endif
    };
}


namespace weave::filesystem
{
    struct DirectoryEntry
    {
        std::string Path;
        FileType Type;
    };

    class DirectoryEnumerator
    {
    private:
        std::string _root{};
        impl::NativeDirectoryEnumerator _native{};

    private:
        impl::PlatformDirectoryEnumerator& AsPlatform()
        {
            return *reinterpret_cast<impl::PlatformDirectoryEnumerator*>(&this->_native);
        }

    public:
        explicit DirectoryEnumerator(std::string_view path);
        DirectoryEnumerator(DirectoryEnumerator&& other);
        DirectoryEnumerator& operator=(DirectoryEnumerator&& other);
        ~DirectoryEnumerator();

        DirectoryEnumerator(DirectoryEnumerator const&) = delete;
        DirectoryEnumerator& operator=(DirectoryEnumerator const&) = delete;

        [[nodiscard]] std::optional<std::expected<DirectoryEntry, platform::SystemError>> Next();

        std::string_view Root() const
        {
            return this->_root;
        }
    };
}
