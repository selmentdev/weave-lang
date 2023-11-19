#pragma once
#include "weave/filesystem/FileSystemError.hxx"

#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace weave::filesystem
{
    std::expected<std::string, FileSystemError> ReadTextFile(std::string_view path);

    std::expected<std::vector<std::byte>, FileSystemError> ReadBinaryFile(std::string_view path);

    std::expected<void, FileSystemError> WriteTextFile(std::string_view path, std::string_view content);

    std::expected<void, FileSystemError> WriteBinaryFile(std::string_view path, std::span<std::byte const> content);
}

namespace weave::filesystem
{
    enum class NameCollisionResolve
    {
        Overwrite,
        Fail,
        GenerateUnique,
    };

    class File
    {
    public:
        static std::expected<std::string, FileSystemError> Copy(
            std::string_view existing,
            std::string_view destination,
            NameCollisionResolve collision);
    };
}