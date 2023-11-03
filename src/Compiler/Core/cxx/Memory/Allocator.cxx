#include "Weave.Core/Memory/Allocator.hxx"
#include "Weave.Core/Assert.hxx"

#include <utility>

namespace Weave::Memory
{
    LinearAllocator::LinearAllocator()
        : LinearAllocator{DefaultSegmentSize}
    {
    }

    LinearAllocator::LinearAllocator(size_t segmentSize)
        : _end{nullptr}
        , _segmentSize{segmentSize}
    {
        Segment* const segment = AllocateSegment(this->_segmentSize);
        this->_end = reinterpret_cast<std::byte*>(segment) + this->_segmentSize;

        PushBack(this->_list, segment);
    }

    LinearAllocator::~LinearAllocator()
    {
        Segment* current = this->_list.Head;

        while (current != nullptr)
        {
            Segment* const back = current->FLink;
            ::operator delete(current);
            current = back;
        }
    }

    LinearAllocator::LinearAllocator(LinearAllocator&& other) noexcept
        : _list{std::exchange(other._list, {})}
        , _end{std::exchange(other._end, {})}
        , _segmentSize{other._segmentSize}
        , _allocatedSegmentsSize{other._allocatedSegmentsSize}
    {
    }

    LinearAllocator& LinearAllocator::operator=(LinearAllocator&& other) noexcept
    {
        if (this != std::addressof(other))
        {
            this->~LinearAllocator();
            new (this) LinearAllocator(std::move(other));
        }

        return *this;
    }

    LinearAllocator::Segment* LinearAllocator::AllocateSegment(size_t size)
    {
        this->_allocatedSegmentsSize += size;

        static_assert(alignof(Segment) <= alignof(std::max_align_t));

        // use process heap for such allocations
        std::byte* const memory = static_cast<std::byte*>(::operator new(size));
        WEAVE_ASSERT(IsAligned(memory, alignof(Segment)));
        std::byte* const last = memory + sizeof(Segment);

        Segment* const result = new (memory) Segment{nullptr, nullptr, last};

        ASAN_POISON_MEMORY_REGION(last, size - sizeof(Segment));

        return result;
    }

    Allocation LinearAllocator::AllocateImpl(Layout const& layout)
    {
        if (this->NeedsSeparateSegment(layout.Size))
        {
            // Compute size of segment.
            size_t const size = AlignUp(sizeof(Segment), layout.Alignment) + layout.Size;

            // Insert it before the current segment.
            Segment* const segment = AllocateSegment(size);

            InsertBefore(this->_list, this->_list.Tail, segment);

            std::byte* const result = AlignUp(segment->Last, layout.Alignment);
            segment->Last = result + layout.Size;

            ASAN_UNPOISON_MEMORY_REGION(result, layout.Size);

            return Allocation{
                .Address = result,
                .Size = layout.Size,
            };
        }

        // Start new segment.
        Segment* const segment = AllocateSegment(this->_segmentSize);

        PushBack(this->_list, segment);

        // Remember last allocation point.
        this->_end = reinterpret_cast<std::byte*>(segment) + this->_segmentSize;

        // Restart allocation - this time it should work
        return this->Allocate(layout);
    }

    void LinearAllocator::QueryMemoryUsage(size_t& allocated, size_t& reserved) const
    {
        Segment* current = this->_list.Head;

        while (current != nullptr)
        {
            allocated += current->Last - reinterpret_cast<std::byte*>(current);
            current = current->FLink;
        }

        reserved += this->_allocatedSegmentsSize;
    }
}
