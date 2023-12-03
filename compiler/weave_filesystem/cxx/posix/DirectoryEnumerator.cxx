#include "weave/platform/Compiler.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"
#include "weave/BugCheck.hxx"

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

    inline FileType ConvertToFileType(unsigned d_type)
    {
        switch (d_type)
        {
        case DT_UNKNOWN:
            return FileType::Unknown;

        case DT_FIFO:
            return FileType::NamedPipe;

        case DT_CHR:
            return FileType::CharacterDevice;

        case DT_DIR:
            return FileType::Directory;

        case DT_BLK:
            return FileType::BlockDevice;

        case DT_REG:
            return FileType::File;

        case DT_LNK:
            return FileType::SymbolicLink;

        case DT_SOCK:
            return FileType::Socket;

        default:
            WEAVE_BUGCHECK("Unknown file type (d_type: {})", d_type);
        }
    }
}

namespace weave::filesystem
{
    DirectoryEnumerator::DirectoryEnumerator(std::string_view path)
        : _root{path}
    {
        this->AsPlatform().Handle = opendir(this->_root.c_str());
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
                closedir(this->AsPlatform().Handle);
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
        if (this->AsPlatform().Handle != nullptr)
        {
            closedir(this->AsPlatform().Handle);
        }
    }

    bool DirectoryEnumerator::MoveNext()
    {
        if (this->AsPlatform().Handle != nullptr)
        {
            if (dirent* current = readdir(this->AsPlatform().Handle); current != nullptr)
            {
                this->_name = current->d_name;
                this->_type = impl::ConvertToFileType(current->d_type);
                return true;
            }
            else
            {
                return false;
            }
        }

        return false;
    }
}
