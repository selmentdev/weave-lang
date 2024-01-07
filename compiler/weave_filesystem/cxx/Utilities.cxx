#include "weave/filesystem/Utilities.hxx"

#include <fmt/format.h>

namespace weave::filesystem
{
    std::string FormatBinarySize(uint64_t size)
    {
        struct BinarySize
        {
            uint64_t Size;
            std::string_view Unit;
        };

        constexpr BinarySize kBinarySizes[] = {
            {uint64_t{1} << 60u, "EiB"},
            {uint64_t{1} << 50u, "PiB"},
            {uint64_t{1} << 40u, "TiB"},
            {uint64_t{1} << 30u, "GiB"},
            {uint64_t{1} << 20u, "MiB"},
            {uint64_t{1} << 10u, "KiB"},
        };

        for (const auto& binarySize : kBinarySizes)
        {
            if (size >= binarySize.Size)
            {
                return fmt::format("{:.03f} {}", static_cast<double>(size) / static_cast<double>(binarySize.Size), binarySize.Unit);
            }
        }

        return fmt::format("{} B", size);
    }
}
