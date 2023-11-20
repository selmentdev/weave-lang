#include "weave/filesystem/FileReader.hxx"

#include <cstring>

namespace weave::filesystem
{
    FileReader::FileReader(FileHandle& handle, size_t capacity)
        : _handle{handle}
        , _buffer{std::make_unique<std::byte[]>(capacity)}
        , _buffer_capacity{capacity}
    {
    }

    FileReader::FileReader(FileHandle& handle)
        : FileReader{handle, DefaultBufferCapacity}
    {
    }

    std::expected<size_t, FileSystemError> FileReader::Read(void* buffer, size_t size)
    {
        if (size > this->_buffer_capacity)
        {
            // Read requst is too big, read directly.

            if (auto r = this->_handle.Read(std::span{static_cast<std::byte*>(buffer), size}, this->_position))
            {
                // Discard internal buffer.
                this->_buffer_size = 0;
                this->_buffer_position = 0;

                // Update file position.
                this->_position += static_cast<int64_t>(*r);
                return *r;
            }
            else
            {
                return std::unexpected(r.error());
            }
        }

        // Copy from internal buffer. Read more if needed.
        size_t processed = 0;

        while (processed < size)
        {
            if (this->_buffer_position == this->_buffer_size)
            {
                // Buffer is empty, read more.
                if (auto r = this->_handle.Read(std::span{this->_buffer.get(), this->_buffer_capacity}, this->_position))
                {
                    if (*r == 0)
                    {
                        return std::unexpected(FileSystemError::EndOfFile);
                    }

                    // Update file position.
                    this->_position += static_cast<int64_t>(*r);

                    // Update buffer state.
                    this->_buffer_size = *r;
                    this->_buffer_position = 0;
                }
                else
                {
                    return std::unexpected(r.error());
                }
            }

            // Copy from internal buffer.

            size_t const toCopy = std::min<size_t>(size - processed, this->_buffer_size - this->_buffer_position);
            std::memcpy(static_cast<std::byte*>(buffer) + processed, this->_buffer.get() + this->_buffer_position, toCopy);

            // Update buffer state.
            this->_buffer_position += toCopy;
            processed += toCopy;
        }

        return processed;
    }

    std::expected<void, FileSystemError> FileReader::ReadExact(void* buffer, size_t size)
    {
        if (auto r = this->Read(buffer, size))
        {
            if (*r == size)
            {
                // Read was successful.
                return {};
            }

            // Partial read can result only with end of file.
            return std::unexpected(FileSystemError::EndOfFile);
        }
        else
        {
            // Other error.
            return std::unexpected(r.error());
        }
    }

    std::expected<size_t, FileSystemError> FileReader::Read(std::span<std::byte> buffer)
    {
        return this->Read(buffer.data(), buffer.size());
    }

    std::expected<void, FileSystemError> FileReader::ReadExact(std::span<std::byte> buffer)
    {
        return this->ReadExact(buffer.data(), buffer.size());
    }
}
