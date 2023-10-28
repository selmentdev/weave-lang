#pragma once
#include "Weave.Core/IO/FileHandle.hxx"

#include <memory>

namespace Weave::IO
{
    class FileWriter
    {
    private:
        FileHandle& m_Handle;
        std::unique_ptr<std::byte[]> m_Buffer{};
        size_t m_BufferCapacity{};
        size_t m_BufferPosition{};
        int64_t m_Position{};

        static constexpr size_t DefaultBufferCapacity = 8u << 10u;

    public:
        FileWriter(FileHandle& handle, size_t capacity);

        FileWriter(FileHandle& handle);

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

        int64_t GetPosition() const
        {
            return this->m_Position;
        }
    };
}
