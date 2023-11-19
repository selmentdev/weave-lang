#pragma once
#include <string_view>
#include <string>
#include <vector>
#include <expected>
#include <span>
#include <utility>

#include "weave/filesystem/FileSystemError.hxx"

namespace weave::filesystem
{
    class Pipe
    {
    private:
        void* _read{};
        void* _write{};

    private:
        Pipe(void* read, void* write)
            : _read{read}
            , _write{write}
        {
        }

    public:
        Pipe() = default;

        Pipe(Pipe const&) = delete;

        Pipe(Pipe&& other)
            : _read{std::exchange(other._read, nullptr)}
            , _write{std::exchange(other._write, nullptr)}
        {
        }

        Pipe& operator=(Pipe const&) = delete;

        Pipe& operator=(Pipe&& other)
        {
            if (this != std::addressof(other))
            {
                this->Close();

                this->_read = std::exchange(other._read, nullptr);
                this->_write = std::exchange(other._write, nullptr);
            }

            return *this;
        }

        ~Pipe()
        {
            this->Close();
        }

    public:
        void* GetReadHandle() const
        {
            return this->_read;
        }

        void* GetWriteHandle() const
        {
            return this->_write;
        }

    public:
        static std::expected<Pipe, FileSystemError> Create();

    public:
        std::expected<size_t, FileSystemError> Read(std::span<std::byte> buffer);

        std::expected<size_t, FileSystemError> Write(std::span<std::byte const> buffer);

        std::expected<size_t, FileSystemError> BytesAvailable() const;

    public:
        void CloseRead();

        void CloseWrite();

        void Close()
        {
            this->CloseRead();
            this->CloseWrite();
        }
    };

    std::expected<size_t, FileSystemError> Read(Pipe& self, std::vector<std::byte>& buffer);

    std::expected<size_t, FileSystemError> Read(Pipe& self, std::string& buffer);

    std::expected<size_t, FileSystemError> Write(Pipe& self, std::string_view buffer);
}
