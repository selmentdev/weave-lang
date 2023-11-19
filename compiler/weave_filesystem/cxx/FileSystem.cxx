#include "weave/filesystem/FileSystem.hxx"
#include "weave/filesystem/FileHandle.hxx"

#include <memory>

namespace weave::filesystem
{
    std::expected<std::string, FileSystemError> ReadTextFile(std::string_view path)
    {
        if (std::expected<FileHandle, FileSystemError> handle = FileHandle::Create(path, FileMode::OpenExisting, FileAccess::Read, FileOptions{.SequentialScan = true}))
        {
            if (std::expected<int64_t, FileSystemError> expected = handle->GetLength())
            {
                std::unique_ptr<std::byte[]> const buffer = std::make_unique_for_overwrite<std::byte[]>(*expected);

                if (std::expected<size_t, FileSystemError> read = handle->Read(std::span{(buffer.get()), static_cast<size_t>(*expected)}, 0))
                {
                    return std::string{reinterpret_cast<char*>(buffer.get()), static_cast<size_t>(*expected)};
                }
                else
                {
                    return std::unexpected(read.error());
                }
            }
            else
            {
                return std::unexpected(expected.error());
            }
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }

    std::expected<std::vector<std::byte>, FileSystemError> ReadBinaryFile(std::string_view path)
    {
        if (std::expected<FileHandle, FileSystemError> handle = FileHandle::Create(path, FileMode::OpenExisting, FileAccess::Read, FileOptions{.SequentialScan = true}))
        {
            if (std::expected<int64_t, FileSystemError> expected = handle->GetLength())
            {
                std::vector<std::byte> result{};
                result.resize(*expected);

                if (std::expected<size_t, FileSystemError> read = handle->Read(std::span{result.data(), result.size()}, 0))
                {
                    return result;
                }
                else
                {
                    return std::unexpected(read.error());
                }
            }
            else
            {
                return std::unexpected(expected.error());
            }
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }

    std::expected<void, FileSystemError> WriteTextFile(std::string_view path, std::string_view content)
    {
        if (auto handle = FileHandle::Create(path, FileMode::CreateAlways, FileAccess::Write))
        {
            if (auto processed = handle->Write(
                std::span{
                    reinterpret_cast<std::byte const*>(content.data()),
                    content.size()},
                0))
            {
                return {};
            }
            else
            {
                return std::unexpected(processed.error());
            }
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }

    std::expected<void, FileSystemError> WriteBinaryFile(std::string_view path, std::span<std::byte const> content)
    {
        if (auto handle = FileHandle::Create(path, FileMode::CreateAlways, FileAccess::Write))
        {
            if (auto processed = handle->Write(std::span{content}, 0))
            {
                return {};
            }
            else
            {
                return std::unexpected(processed.error());
            }
        }
        else
        {
            return std::unexpected(handle.error());
        }
    }
}
