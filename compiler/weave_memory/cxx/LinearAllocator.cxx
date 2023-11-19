#include "weave/memory/LinearAllocator.hxx"
#include "weave/BugCheck.hxx"

#include <utility>

namespace weave::memory
{
    LinearAllocator::LinearAllocator()
        : LinearAllocator{DefaultSegmentSize}
    {
    }

    LinearAllocator::LinearAllocator(size_t segmentSize)
        : _end{nullptr}
        , _segment_size{segmentSize}
    {
        Segment* const segment = allocate_segment(this->_segment_size);
        this->_end = reinterpret_cast<std::byte*>(segment) + this->_segment_size;

        push_back(this->_list, segment);
    }

    LinearAllocator::~LinearAllocator()
    {
        Segment* current = this->_list.head;

        while (current != nullptr)
        {
            Segment* const back = current->flink;
            ::operator delete(current);
            current = back;
        }
    }

    LinearAllocator::LinearAllocator(LinearAllocator&& other) noexcept
        : _list{std::exchange(other._list, {})}
        , _end{std::exchange(other._end, {})}
        , _segment_size{other._segment_size}
        , _allocated_segments_size{other._allocated_segments_size}
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

    LinearAllocator::Segment* LinearAllocator::allocate_segment(size_t size)
    {
        this->_allocated_segments_size += size;

        static_assert(alignof(Segment) <= alignof(std::max_align_t));

        // use process heap for such allocations
        std::byte* const memory = static_cast<std::byte*>(::operator new(size));
        WEAVE_ASSERT(bitwise::IsAligned(memory, alignof(Segment)));
        std::byte* const last = memory + sizeof(Segment);

        Segment* const result = new (memory) Segment{nullptr, nullptr, last};

        ASAN_POISON_MEMORY_REGION(last, size - sizeof(Segment));

        return result;
    }

    Allocation LinearAllocator::allocate_impl(Layout const& layout)
    {
        if (this->needs_separate_segment(layout.size))
        {
            // Compute size of segment.
            size_t const size = bitwise::AlignUp(sizeof(Segment), layout.alignment) + layout.size;

            // Insert it before the current segment.
            Segment* const segment = this->allocate_segment(size);

            insert_before(this->_list, this->_list.tail, segment);

            std::byte* const result = bitwise::AlignUp(segment->last, layout.alignment);
            segment->last = result + layout.size;

            ASAN_UNPOISON_MEMORY_REGION(result, layout.size);

            return Allocation{
                .address = result,
                .size = layout.size,
            };
        }

        // Start new segment.
        Segment* const segment = this->allocate_segment(this->_segment_size);

        push_back(this->_list, segment);

        // Remember last allocation point.
        this->_end = reinterpret_cast<std::byte*>(segment) + this->_segment_size;

        // Restart allocation - this time it should work
        return this->allocate(layout);
    }

    void LinearAllocator::query_memory_usage(size_t& allocated, size_t& reserved) const
    {
        Segment* current = this->_list.head;

        while (current != nullptr)
        {
            allocated += current->last - reinterpret_cast<std::byte*>(current);
            current = current->flink;
        }

        reserved += this->_allocated_segments_size;
    }
}
