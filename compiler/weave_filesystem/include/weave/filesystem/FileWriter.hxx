#pragma once
#include "weave/filesystem/FileHandle.hxx"

#include <memory>

namespace weave::filesystem
{
    class FileWriter
    {
    private:
        FileHandle& _handle;
        std::unique_ptr<std::byte[]> _buffer{};
        size_t _buffer_capacity{};
        size_t _buffer_position{};
        int64_t _position{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    public:
        FileWriter(FileHandle& handle, size_t capacity);

        explicit FileWriter(FileHandle& handle);

        FileWriter(FileWriter const&) = delete;

        FileWriter(FileWriter&&) = delete;

        FileWriter& operator=(FileWriter const&) = delete;

        FileWriter& operator=(FileWriter&&) = delete;

        ~FileWriter();

    private:
        std::expected<void, FileSystemError> FlushBuffer();

    public:
        std::expected<void, FileSystemError> Flush();

        std::expected<size_t, FileSystemError> Write(void const* buffer, size_t size);

        std::expected<size_t, FileSystemError> Write(std::span<std::byte const> buffer);

        [[nodiscard]] int64_t GetPosition() const
        {
            return this->_position;
        }
    };
}

namespace weave::filesystem
{
    inline std::expected<void, FileSystemError> Write(FileWriter& writer, std::string_view value)
    {
        if (auto processed = writer.Write(std::as_bytes(std::span{value})); not processed)
        {
            return std::unexpected(processed.error());
        }

        return {};
    }
}
