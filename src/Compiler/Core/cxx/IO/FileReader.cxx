#include "Compiler.Core/IO/FileReader.hxx"

namespace Weave::IO
{
    FileReader::FileReader(FileHandle& handle, size_t capacity)
        : m_Handle{handle}
        , m_Buffer{std::make_unique<std::byte[]>(capacity)}
        , m_BufferCapacity{capacity}
    {
    }

    FileReader::FileReader(FileHandle& handle)
        : FileReader{handle, DefaultBufferCapacity}
    {
    }

    std::expected<size_t, FileSystemError> FileReader::Read(void* buffer, size_t size)
    {
        if (size > this->m_BufferCapacity)
        {
            // Read requst is too big, read directly.

            if (auto r = this->m_Handle.Read(std::span{static_cast<std::byte*>(buffer), size}, this->m_Position))
            {
                // Discard internal buffer.
                this->m_BufferSize = 0;
                this->m_BufferPosition = 0;

                // Update file position.
                this->m_Position += *r;
                return *r;
            }
            else
            {
                return std::unexpected(r.error());
            }
        }
        else
        {
            // Copy from internal buffer. Read more if needed.

            size_t processed = 0;

            while (processed < size)
            {
                if (this->m_BufferPosition == this->m_BufferSize)
                {
                    // Buffer is empty, read more.
                    if (auto r = this->m_Handle.Read(std::span{this->m_Buffer.get(), this->m_BufferCapacity}, this->m_Position))
                    {
                        if (*r == 0)
                        {
                            return std::unexpected(FileSystemError::EndOfFile);
                        }

                        // Update file position.
                        this->m_Position += *r;

                        // Update buffer state.
                        this->m_BufferSize = *r;
                        this->m_BufferPosition = 0;
                    }
                    else
                    {
                        return std::unexpected(r.error());
                    }
                }

                // Copy from internal buffer.

                size_t const toCopy = std::min<size_t>(size - processed, this->m_BufferSize - this->m_BufferPosition);
                std::memcpy(static_cast<std::byte*>(buffer) + processed, this->m_Buffer.get() + this->m_BufferPosition, toCopy);

                // Update buffer state.
                this->m_BufferPosition += toCopy;
                processed += toCopy;
            }

            return processed;
        }
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
            else
            {
                // Partial read can result only with end of file.
                return std::unexpected(FileSystemError::EndOfFile);
            }
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
