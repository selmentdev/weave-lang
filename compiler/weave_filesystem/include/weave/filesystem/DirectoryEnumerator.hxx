#pragma once
#include "weave/filesystem/Path.hxx"
#include <string>
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
    class DirectoryEnumerator
    {
    private:
        impl::NativeDirectoryEnumerator _native;
        std::string _root;
        FileType _type;
        std::string_view _name;

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
        
        std::string GetPath() const;
        FileType GetFileType() const;

        bool MoveNext();
    };

    inline std::string DirectoryEnumerator::GetPath() const
    {
        std::string result{this->_root};
        path::Append(result, this->_name);
        return result;
    }

    inline FileType DirectoryEnumerator::GetFileType() const
    {
        return this->_type;
    }
}
