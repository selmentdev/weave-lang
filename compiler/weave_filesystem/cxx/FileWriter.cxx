#include "weave/filesystem/FileWriter.hxx"

#include <memory>
#include <cstring>

namespace weave::filesystem
{
    FileWriter::FileWriter(FileHandle& handle, size_t capacity)
        : _handle{handle}
        , _buffer{std::make_unique<std::byte[]>(capacity)}
        , _buffer_capacity{capacity}
    {
    }

    FileWriter::FileWriter(FileHandle& handle)
        : FileWriter(handle, DefaultBufferCapacity)
    {
    }

    FileWriter::~FileWriter()
    {
        (void)this->FlushBuffer();
    }

    std::expected<void, FileSystemError> FileWriter::FlushBuffer()
    {
        if (this->_buffer_position > 0)
        {
            if (auto r = this->_handle.Write(std::span{this->_buffer.get(), this->_buffer_position}, this->_position))
            {
                // Update file position.
                this->_position += static_cast<int64_t>(*r);

                // Discard internal buffer.
                this->_buffer_position = 0;
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

            if (auto flushed = this->_handle.Flush())
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
        if (size >= this->_buffer_capacity)
        {
            // Buffer is too large, flush internal buffer first, then write directly.
            if (auto flushed = this->FlushBuffer())
            {
                if (auto processed = this->_handle.Write(std::span{static_cast<std::byte const*>(buffer), size}, this->_position))
                {
                    // Update file position.
                    this->_position += static_cast<int64_t>(*processed);

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
            if (this->_buffer_position + size > this->_buffer_capacity)
            {
                if (auto r = this->FlushBuffer())
                {
                    std::memcpy(this->_buffer.get(), buffer, size);
                    this->_buffer_position = size;

                    return size;
                }
                else
                {
                    return std::unexpected(r.error());
                }
            }
            else
            {
                std::memcpy(this->_buffer.get() + this->_buffer_position, buffer, size);
                this->_buffer_position += size;

                return size;
            }
        }
    }

    std::expected<size_t, FileSystemError> FileWriter::Write(std::span<std::byte const> buffer)
    {
        return this->Write(buffer.data(), buffer.size());
    }
}
