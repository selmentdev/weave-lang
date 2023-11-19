#include "weave/filesystem/Pipe.hxx"

namespace weave::filesystem
{
    std::expected<size_t, FileSystemError> Read(Pipe& self, std::vector<std::byte>& buffer)
    {
        buffer.clear();

        auto available = self.BytesAvailable();

        if (available.has_value())
        {
            if (*available > 0)
            {
                buffer.resize(*available);
                return self.Read(std::span{buffer});
            }
        }

        return available;
    }

    std::expected<size_t, FileSystemError> Read(Pipe& self, std::string& buffer)
    {
        buffer.clear();

        auto available = self.BytesAvailable();

        if (available.has_value())
        {
            if (*available > 0)
            {
                buffer.resize(*available);
                return self.Read(std::as_writable_bytes(std::span{buffer}));
            }
        }

        return available;
    }

    std::expected<size_t, FileSystemError> Write(Pipe& self, std::string_view buffer)
    {
        return self.Write(std::as_bytes(std::span{buffer}));
    }
}
