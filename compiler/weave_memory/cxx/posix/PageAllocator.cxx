#include "weave/platform/Compiler.hxx"
#include "weave/memory/PageAllocator.hxx"
#include "weave/Bitwise.hxx"
#include "weave/bugcheck/Assert.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN
#include <sys/mman.h>
WEAVE_EXTERNAL_HEADERS_END

namespace weave::memory
{
    PageAllocationResult PageAllocate(size_t size)
    {
        size_t const alignment = (size < PageGranularity)
            ? PageSize
            : PageGranularity;

        size_t const aligned_size = bitwise::AlignUp(size, alignment);

        void* result = mmap(nullptr, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

        WEAVE_ASSERT(bitwise::IsAligned(result, alignment));

        return PageAllocationResult{
            .Pointer = result,
            .Size = aligned_size,
            .Alignment = alignment,
        };
    }

    void PageDeallocate(void* pointer)
    {
        munmap(pointer, 0);
    }
}
