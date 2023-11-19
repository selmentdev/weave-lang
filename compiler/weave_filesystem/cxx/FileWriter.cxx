#include "weave/filesystem/FileWriter.hxx"

#include <memory>
#include <cstring>

namespace weave::filesystem
{
    FileWriter::FileWriter(FileHandle& handle, size_t capacity)
        : m_Handle{handle}
        , m_Buffer{std::make_unique<std::byte[]>(capacity)}
        , m_BufferCapacity{capacity}
    {
    }

    FileWriter::FileWriter(FileHandle& handle)
        : FileWriter(handle, DefaultBufferCapacity)
    {
    }

    FileWriter::~FileWriter()
    {
        this->FlushBuffer();
    }

    std::expected<void, FileSystemError> FileWriter::FlushBuffer()
    {
        if (this->m_BufferPosition > 0)
        {
            if (auto r = this->m_Handle.Write(std::span{this->m_Buffer.get(), this->m_BufferPosition}, this->m_Position))
            {
                // Update file position.
                this->m_Position += static_cast<int64_t>(*r);

                // Discard internal buffer.
                this->m_BufferPosition = 0;
            }
            else
            {
                return std::unexpected(r.error());
            }
        }

        return {};
    }

    std::expected<void, FileSystemError> FileWriter::Flush()
    {
        if (auto processed = this->FlushBuffer())
        {
            // Flushed internal writer buffer.

            if (auto flushed = this->m_Handle.Flush())
            {
                // Flushed file system buffers.
                return {};
            }
            else
            {
                return std::unexpected(flushed.error());
            }
        }
        else
        {
            return std::unexpected(processed.error());
        }
    }

    std::expected<size_t, FileSystemError> FileWriter::Write(void const* buffer, size_t size)
    {
        if (size >= this->m_BufferCapacity)
        {
            // Buffer is too large, flush internal buffer first, then write directly.
            if (auto flushed = this->FlushBuffer())
            {
                if (auto processed = this->m_Handle.Write(std::span{static_cast<std::byte const*>(buffer), size}, this->m_Position))
                {
                    // Update file position.
                    this->m_Position += static_cast<int64_t>(*processed);

                    return *processed;
                }
                else
                {
                    return std::unexpected(processed.error());
                }
            }
            else
            {
                return std::unexpected(flushed.error());
            }
        }
        else
        {
            // Buffer is large enough, copy to internal buffer, flush if necessary.
            if (this->m_BufferPosition + size > this->m_BufferCapacity)
            {
                if (auto r = this->FlushBuffer())
                {
                    std::memcpy(this->m_Buffer.get(), buffer, size);
                    this->m_BufferPosition = size;

                    return size;
                }
                else
                {
                    return std::unexpected(r.error());
                }
            }
            else
            {
                std::memcpy(this->m_Buffer.get() + this->m_BufferPosition, buffer, size);
                this->m_BufferPosition += size;

                return size;
            }
        }
    }

    std::expected<size_t, FileSystemError> FileWriter::Write(std::span<std::byte const> buffer)
    {
        return this->Write(buffer.data(), buffer.size());
    }
}
