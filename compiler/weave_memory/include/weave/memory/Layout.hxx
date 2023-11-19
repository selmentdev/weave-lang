#pragma once
#include <cstddef>

namespace weave::memory
{
    struct Layout
    {
        size_t size{};
        size_t alignment{};
    };

    struct Allocation
    {
        std::byte* address{};
        size_t size{};
    };
}
