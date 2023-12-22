#pragma once
#include "weave/platform/SystemError.hxx"

#include <cstddef>
#include <cstdint>
#include <expected>
#include <string_view>
#include <span>
#include <utility>

namespace weave::filesystem::impl
{
    struct PlatformFileHandle;

    struct NativeFileHandle final
    {
#if defined(WIN32)
        void* Native[1];
#elif defined(__linux__)
        void* Native[1];
#else
#error "Not implemented."
#endif
    };
}

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
        impl::NativeFileHandle _native;

    private:
        impl::PlatformFileHandle& AsPlatform();
        impl::PlatformFileHandle const& AsPlatform() const;

    public:
        explicit FileHandle(impl::PlatformFileHandle const& native);
        FileHandle();
        FileHandle(FileHandle const&) = delete;
        FileHandle(FileHandle&& other) noexcept;
        FileHandle& operator=(FileHandle const&) = delete;
        FileHandle& operator=(FileHandle&& other) noexcept;
        ~FileHandle();

    public:
        static std::expected<FileHandle, platform::SystemError> Create(std::string_view path, FileMode mode, FileAccess access, FileOptions options);

        static std::expected<FileHandle, platform::SystemError> Create(std::string_view path, FileMode mode, FileAccess access)
        {
            return Create(path, mode, access, {});
        }

        static bool Exists(std::string_view path);

    public:
        std::expected<void, platform::SystemError> Close();

        std::expected<void, platform::SystemError> Flush();

        [[nodiscard]] std::expected<int64_t, platform::SystemError> GetLength() const;

        std::expected<void, platform::SystemError> SetLength(int64_t length);

        std::expected<size_t, platform::SystemError> Read(std::span<std::byte> buffer, int64_t position);

        std::expected<size_t, platform::SystemError> Write(std::span<std::byte const> buffer, int64_t position);
    };
}
