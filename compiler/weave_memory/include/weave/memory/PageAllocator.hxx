#pragma once
#include <cstddef>

namespace weave::memory
{
    inline constexpr size_t PageSize = size_t{4} << 10u;
    inline constexpr size_t PageAlignment = size_t{4} << 10u;
    inline constexpr size_t PageGranularity = size_t{64} << 10u;

    struct PageAllocationResult final
    {
        void* Pointer;
        size_t Size;
        size_t Alignment;
    };

    PageAllocationResult PageAllocate(size_t size);

    void PageDeallocate(void* pointer);
}
