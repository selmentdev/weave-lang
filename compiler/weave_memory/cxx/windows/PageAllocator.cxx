#include "weave/platform/Compiler.hxx"
#include "weave/memory/PageAllocator.hxx"
#include "weave/Bitwise.hxx"
#include "weave/bugcheck/Assert.hxx"

#include "weave/platform/windows/PlatformHeaders.hxx"

namespace weave::memory
{
    PageAllocationResult PageAllocate(size_t size)
    {
        size_t const alignment = (size < PageGranularity)
            ? PageSize
            : PageGranularity;

        size_t const aligned_size = bitwise::AlignUp(size, alignment);

        void* result = VirtualAlloc(nullptr, aligned_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

        WEAVE_ASSERT(bitwise::IsAligned(result, alignment));

        return PageAllocationResult{
            .Pointer = result,
            .Size = aligned_size,
            .Alignment = alignment,
        };
    }

    void PageDeallocate(void* pointer)
    {
        VirtualFree(pointer, 0, MEM_RELEASE);
    }
}
