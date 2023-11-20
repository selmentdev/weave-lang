#pragma once
#include "weave/filesystem/FileSystemError.hxx"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <string_view>
#include <span>
#include <utility>

namespace weave::filesystem
{
    enum class FileMode
    {
        // Creates new file. Truncate if exists.
        CreateAlways,

        // Create new file. Fail if exists.
        CreateNew,

        // Create new file. Open if exists.
        OpenAlways,

        // Open if exists. Fail otherwise.
        OpenExisting,

        // Truncates existing file. Fails otherwise.
        TruncateExisting,
    };

    enum class FileAccess
    {
        Read = 1u << 0u,
        Write = 1u << 1u,
        ReadWrite = Read | Write,
    };

    struct FileOptions final
    {
        bool ShareRead = false;
        bool ShareWrite = false;
        bool Inheritable = false;
        bool DeleteOnClose = false;
        bool RandomAccess = false;
        bool SequentialScan = false;
        bool WriteThrough = false;
        bool NoBuffering = false;
    };

    class FileHandle final
    {
    private:
        void* _handle{};

    private:
        explicit FileHandle(void* handle)
            : _handle{handle}
        {
        }

    public:
        FileHandle() = delete;

        FileHandle(FileHandle const&) = delete;

        FileHandle(FileHandle&& other) noexcept
            : _handle{std::exchange(other._handle, nullptr)}
        {
        }

        FileHandle& operator=(FileHandle const&) = delete;

        FileHandle& operator=(FileHandle&& other) noexcept
        {
            if (this != std::addressof(other))
            {
                if (this->_handle != nullptr)
                {
                    // Ignore errors.
                    (void)this->Close();
                }

                this->_handle = std::exchange(other._handle, nullptr);
            }

            return *this;
        }

        ~FileHandle()
        {
            if (this->_handle != nullptr)
            {
                // Ignore errors.
                (void)this->Close();
            }
        }

    public:
        static std::expected<FileHandle, FileSystemError> Create(std::string_view path, FileMode mode, FileAccess access, FileOptions options);

        static std::expected<FileHandle, FileSystemError> Create(std::string_view path, FileMode mode, FileAccess access)
        {
            return Create(path, mode, access, {});
        }

        static bool Exists(std::string_view path);

    public:
        std::expected<void, FileSystemError> Close();

        std::expected<void, FileSystemError> Flush();

        [[nodiscard]] std::expected<int64_t, FileSystemError> GetLength() const;

        std::expected<void, FileSystemError> SetLength(int64_t length);

        std::expected<size_t, FileSystemError> Read(std::span<std::byte> buffer, int64_t position);

        std::expected<size_t, FileSystemError> Write(std::span<std::byte const> buffer, int64_t position);
    };
}
