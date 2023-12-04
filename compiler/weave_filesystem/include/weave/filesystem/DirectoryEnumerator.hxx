#pragma once
#include "weave/filesystem/FileSystemError.hxx"
#include "weave/filesystem/Path.hxx"

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
    enum class FileType
    {
        Unknown,
        Directory,
        File,
        SymbolicLink,
        BlockDevice,
        CharacterDevice,
        Socket,
        NamedPipe,
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
        impl::NativeDirectoryEnumerator _native{};
        std::string _root{};

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
        
        [[nodiscard]] std::optional<std::expected<DirectoryEntry, FileSystemError>> Next();
    };
}
