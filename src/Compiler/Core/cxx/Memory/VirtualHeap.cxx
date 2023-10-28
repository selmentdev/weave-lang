#include "Weave.Core/Memory/VirtualHeap.hxx"
#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Bits.hxx"
#include "Weave.Core/Platform/PageAllocator.hxx"

#include <algorithm>
#include <bit>

namespace Weave::Memory
{
    VirtualHeap::~VirtualHeap()
    {
        this->Reset();
    }

    VirtualHeap::Segment* VirtualHeap::AllocateNewSegment(size_t size)
    {
        WEAVE_ASSERT(size != 0);

        PageAllocationResult const allocation = PageAllocate(std::max(size, PageGranularity));

        if (allocation.Pointer == nullptr)
        {
            return nullptr;
        }

        std::byte* const first = static_cast<std::byte*>(allocation.Pointer);
        std::byte* const end = first + allocation.Size;


        return &this->m_Segments.emplace_back(first, first, end);
    }

    void* VirtualHeap::TryAllocateFromSegment(Segment& segment, size_t size, size_t alignment)
    {
        WEAVE_ASSERT(segment.First <= segment.Last);
        WEAVE_ASSERT(segment.Last <= segment.End);

        std::byte* const result = AlignUp(segment.Last, alignment);
        std::byte* const last = result + size;

        if (last <= segment.End)
        {
            // We have enough space in current segment.
            segment.Last = last;
            return result;
        }

        return nullptr;
    }

    void* VirtualHeap::Allocate(size_t size, size_t alignment)
    {
        WEAVE_ASSERT(size > 0);
        WEAVE_ASSERT(alignment > 0);
        WEAVE_ASSERT(std::has_single_bit(alignment));
        WEAVE_ASSERT(IsAligned(size, alignment));

        // Check if we have current segment.
        if (this->m_Segments.empty())
        {
            this->AllocateNewSegment(size);
        }

        // Check if we have enough space in current segment.
        Segment* segment = &this->m_Segments.back();

        void* result = TryAllocateFromSegment(*segment, size, alignment);

        if (result != nullptr)
        {
            return result;
        }

        // Allocate new segment.
        this->AllocateNewSegment(size);

        segment = &this->m_Segments.back();

        result = TryAllocateFromSegment(*segment, size, alignment);

        WEAVE_ASSERT(result != nullptr);

        return result;
    }

    void VirtualHeap::Reset()
    {
        for (Segment const& segment : this->m_Segments)
        {
            WEAVE_ASSERT(segment.First <= segment.Last);
            WEAVE_ASSERT(segment.Last <= segment.End);

            PageDeallocate(segment.First);
        }

        this->m_Segments.clear();
    }
}
