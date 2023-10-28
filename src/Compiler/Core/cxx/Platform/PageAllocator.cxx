#include "Weave.Core/Platform/PageAllocator.hxx"
#include "Weave.Core/Bits.hxx"
#include "Weave.Core/Platform/Compiler.hxx"


WEAVE_EXTERNAL_HEADERS_BEGIN

#if defined(WIN32)

#define NOMINMAX
#include <Windows.h>

#elif defined(__linux__)

#include <sys/mman.h>

#else
#error Not implemented
#endif

WEAVE_EXTERNAL_HEADERS_END


namespace Weave::Memory
{
    PageAllocationResult PageAllocate(size_t size)
    {

        size_t const alignment = (size < PageGranularity)
            ? PageSize
            : PageGranularity;

        size_t const aligned_size = AlignUp(size, alignment);

#if defined(WIN32)

        void* result = VirtualAlloc(nullptr, aligned_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

#elif defined(__linux__)

        void* result = mmap(nullptr, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

#else
#error Not implemented
#endif

        return PageAllocationResult{
            .Pointer = result,
            .Size = aligned_size,
            .Alignment = alignment,
        };
    }

    void PageDeallocate(void* pointer)
    {
#if defined(WIN32)

        VirtualFree(pointer, 0, MEM_RELEASE);

#elif defined(__linux__)

        munmap(pointer, 0);

#else
#error Not implemented
#endif
    }
}
