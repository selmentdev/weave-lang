#pragma once
#include "weave/filesystem/FileHandle.hxx"

#include <memory>

namespace weave::filesystem
{
    class FileReader
    {
    private:
        FileHandle& _handle;
        std::unique_ptr<std::byte[]> _buffer{};
        size_t _buffer_size{};
        size_t _buffer_capacity{};
        size_t _buffer_position{};
        int64_t _position{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    public:
        FileReader(FileHandle& handle, size_t capacity);

        explicit FileReader(FileHandle& handle);

        FileReader(FileReader const&) = delete;

        FileReader(FileReader&&) = delete;

        FileReader& operator=(FileReader const&) = delete;

        FileReader& operator=(FileReader&&) = delete;

        ~FileReader() = default;

    public:
        std::expected<size_t, platform::SystemError> Read(void* buffer, size_t size);

        std::expected<void, platform::SystemError> ReadExact(void* buffer, size_t size);

        std::expected<size_t, platform::SystemError> Read(std::span<std::byte> buffer);

        std::expected<void, platform::SystemError> ReadExact(std::span<std::byte> buffer);

        [[nodiscard]] int64_t GetPosition() const
        {
            return this->_position;
        }
    };
}
