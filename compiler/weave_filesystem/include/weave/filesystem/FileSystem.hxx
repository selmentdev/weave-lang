#pragma once
#include "weave/platform/SystemError.hxx"

#include <expected>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace weave::filesystem
{
    std::expected<std::string, platform::SystemError> ReadTextFile(std::string_view path);

    std::expected<std::vector<std::byte>, platform::SystemError> ReadBinaryFile(std::string_view path);

    std::expected<void, platform::SystemError> WriteTextFile(std::string_view path, std::string_view content);

    std::expected<void, platform::SystemError> WriteBinaryFile(std::string_view path, std::span<std::byte const> content);
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
        static std::expected<std::string, platform::SystemError> Copy(
            std::string_view existing,
            std::string_view destination,
            NameCollisionResolve collision);

        static std::expected<void, platform::SystemError> Remove(
            std::string_view path);
    };

    class Directory
    {
    public:
        static std::expected<void, platform::SystemError> Create(
            std::string_view path);

        static std::expected<void, platform::SystemError> Remove(
            std::string_view path);
    };
}
