#pragma once
#include <cstddef>

namespace weave::memory
{
    inline constexpr size_t PageSize = size_t{4} << 10u;
    inline constexpr size_t PageAlignment = size_t{4} << 10u;
    inline constexpr size_t PageGranularity = size_t{64} << 10u;

    struct PageAllocationResult final
    {
        void* pointer;
        size_t size;
        size_t alignment;
    };

    PageAllocationResult page_allocate(size_t size);

    void page_deallocate(void* pointer);
}
