#pragma once
#include "weave/platform/Compiler.hxx"
#include "weave/memory/Layout.hxx"
#include "weave/Bitwise.hxx"

#include <span>
#include <memory>

namespace weave::memory
{
    class LinearAllocator
    {
        static constexpr size_t DefaultSegmentSize{64u << 10u};

    protected:
        struct Segment
        {
            Segment* BackLink{};
            Segment* ForwardLink{};
            std::byte* Last{};
        };

        struct SegmentList
        {
            Segment* Head{};
            Segment* Tail{};
        };

    protected:
        SegmentList _list{};
        std::byte* _end;
        size_t _segment_size;
        size_t _allocated_segments_size{};

    public:
        LinearAllocator();
        explicit LinearAllocator(size_t segment_size);
        ~LinearAllocator();

        LinearAllocator(LinearAllocator const&) = delete;
        LinearAllocator(LinearAllocator&& other) noexcept;

        LinearAllocator& operator=(LinearAllocator const&) = delete;
        LinearAllocator& operator=(LinearAllocator&& other) noexcept;

    protected:
        Segment* AllocateSegment(size_t size);

        Allocation AllocateImpl(Layout const& layout);

        [[nodiscard]] constexpr bool NeedsSeparateSegment(size_t size) const
        {
            return size > (this->_segment_size / 4);
        }

        // Inserts `other` before `self` segment.
        static constexpr void InsertBefore(SegmentList& list, Segment* self, Segment* other)
        {
            other->BackLink = self->BackLink;
            other->ForwardLink = self;

            if (self->BackLink != nullptr)
            {
                self->BackLink->ForwardLink = other;
            }
            else
            {
                list.Head = other;
            }

            self->BackLink = other;
        }

        static constexpr void PushBack(SegmentList& list, Segment* segment)
        {
            if (list.Head == nullptr)
            {
                list.Head = segment;
                list.Tail = segment;
            }
            else
            {
                list.Tail->ForwardLink = segment;
                segment->BackLink = list.Tail;
                list.Tail = segment;
            }
        }

    public:
        Allocation Allocate(Layout const& layout)
        {
            std::byte* const result = bitwise::AlignUp(this->_list.Tail->Last, layout.Alignment);
            std::byte* const last = result + layout.Size;

            if (last > this->_end)
            {
                return this->AllocateImpl(layout);
            }

            // Update head's end pointer.
            this->_list.Tail->Last = last;

            ASAN_UNPOISON_MEMORY_REGION(result, layout.Size);

            return Allocation{
                .Address = result,
                .Size = layout.Size,
            };
        }

        template <typename T, typename... ArgsT>
        [[nodiscard]] T* Emplace(ArgsT&&... args)
        {
            static_assert(std::is_trivially_destructible_v<T>);

            Allocation const allocation = this->Allocate(Layout{
                .Size = sizeof(T),
                .Alignment = alignof(T),
            });

            return new (allocation.Address) T(std::forward<ArgsT>(args)...);
        }

        template <typename T>
        [[nodiscard]] std::span<T> EmplaceArray(size_t size)
        {
            static_assert(std::is_trivially_destructible_v<T>);

            Allocation const allocation = this->Allocate(Layout{
                .Size = sizeof(T) * size,
                .Alignment = alignof(T),
            });

            T* const result = reinterpret_cast<T*>(allocation.Address);

            std::uninitialized_default_construct_n(
                result,
                size);

            return std::span<T>{
                result,
                size,
            };
        }

        template <typename T>
        [[nodiscard]] std::span<T> EmplaceArray(std::span<T const> source)
        {
            static_assert(std::is_trivially_destructible_v<T>);

            if (not source.empty())
            {
                Allocation const allocation = this->Allocate(Layout{
                    .Size = sizeof(T) * source.size(),
                    .Alignment = alignof(T),
                });

                T* const result = reinterpret_cast<T*>(allocation.Address);

                std::uninitialized_copy_n(
                    source.data(),
                    source.size(),
                    result);

                return std::span<T>{
                    result,
                    source.size(),
                };
            }

            return {};
        }

    public:
        void QueryMemoryUsage(size_t& allocated, size_t& reserved) const;
    };
}
