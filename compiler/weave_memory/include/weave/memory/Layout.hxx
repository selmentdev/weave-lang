#pragma once
#include <cstddef>

namespace weave::memory
{
    struct Layout
    {
        size_t Size{};
        size_t Alignment{};
    };

    struct Allocation
    {
        std::byte* Address{};
        size_t Size{};
    };
}
