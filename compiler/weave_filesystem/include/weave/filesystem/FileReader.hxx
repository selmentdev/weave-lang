#pragma once
#include "weave/filesystem/FileHandle.hxx"

#include <memory>

namespace weave::filesystem
{
    class FileReader
    {
    private:
        FileHandle& m_Handle;
        std::unique_ptr<std::byte[]> m_Buffer{};
        size_t m_BufferSize{};
        size_t m_BufferCapacity{};
        size_t m_BufferPosition{};
        int64_t m_Position{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    public:
        FileReader(FileHandle& handle, size_t capacity);

        FileReader(FileHandle& handle);

        FileReader(FileReader const&) = delete;

        FileReader(FileReader&&) = delete;

        FileReader& operator=(FileReader const&) = delete;

        FileReader& operator=(FileReader&&) = delete;

        ~FileReader() = default;

    public:
        std::expected<size_t, FileSystemError> Read(void* buffer, size_t size);

        std::expected<void, FileSystemError> ReadExact(void* buffer, size_t size);

        std::expected<size_t, FileSystemError> Read(std::span<std::byte> buffer);

        std::expected<void, FileSystemError> ReadExact(std::span<std::byte> buffer);

        int64_t GetPosition() const
        {
            return this->m_Position;
        }
    };
}
