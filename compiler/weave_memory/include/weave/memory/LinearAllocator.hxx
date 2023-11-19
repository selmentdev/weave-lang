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
            Segment* blink{};
            Segment* flink{};
            std::byte* last{};
        };

        struct SegmentList
        {
            Segment* head{};
            Segment* tail{};
        };

    protected:
        SegmentList _list{};
        std::byte* _end;
        size_t _segment_size;
        size_t _allocated_segments_size{};

    public:
        LinearAllocator();
        LinearAllocator(size_t segment_size);
        ~LinearAllocator();

        LinearAllocator(LinearAllocator const&) = delete;
        LinearAllocator(LinearAllocator&& other) noexcept;

        LinearAllocator& operator=(LinearAllocator const&) = delete;
        LinearAllocator& operator=(LinearAllocator&& other) noexcept;

    protected:
        Segment* allocate_segment(size_t size);

        Allocation allocate_impl(Layout const& layout);

        constexpr bool needs_separate_segment(size_t size) const
        {
            return size > (this->_segment_size / 4);
        }

        // Inserts `other` before `self` segment.
        static constexpr void insert_before(SegmentList& list, Segment* self, Segment* other)
        {
            other->blink = self->blink;
            other->flink = self;

            if (self->blink != nullptr)
            {
                self->blink->flink = other;
            }
            else
            {
                list.head = other;
            }

            self->blink = other;
        }

        static constexpr void push_back(SegmentList& list, Segment* segment)
        {
            if (list.head == nullptr)
            {
                list.head = segment;
                list.tail = segment;
            }
            else
            {
                list.tail->flink = segment;
                segment->blink = list.tail;
                list.tail = segment;
            }
        }

    public:
        Allocation allocate(Layout const& layout)
        {
            std::byte* const result = bitwise::AlignUp(this->_list.tail->last, layout.alignment);
            std::byte* const last = result + layout.size;

            if (last > this->_end)
            {
                return this->allocate_impl(layout);
            }

            // Update head's end pointer.
            this->_list.tail->last = last;

            ASAN_UNPOISON_MEMORY_REGION(result, layout.size);

            return Allocation{
                .address = result,
                .size = layout.size,
            };
        }

        template <typename T, typename... ArgsT>
        [[nodiscard]] T* create(ArgsT&&... args)
        {
            static_assert(std::is_trivially_destructible_v<T>);

            Allocation const allocation = this->allocate(Layout{
                .size = sizeof(T),
                .alignment = alignof(T),
            });

            return new (allocation.address) T(std::forward<ArgsT>(args)...);
        }

        template <typename T>
        [[nodiscard]] std::span<T> create_array(size_t size)
        {
            static_assert(std::is_trivially_destructible_v<T>);

            Allocation const allocation = this->allocate(Layout{
                .size = sizeof(T) * size,
                .alignment = alignof(T),
            });

            T* const result = reinterpret_cast<T*>(allocation.address);

            std::uninitialized_default_construct_n(
                result,
                size);

            return std::span<T>{
                result,
                size,
            };
        }

        template <typename T>
        [[nodiscard]] std::span<T> create_array(std::span<T const> source)
        {
            static_assert(std::is_trivially_destructible_v<T>);

            if (not source.empty())
            {
                Allocation const allocation = this->allocate(Layout{
                    .size = sizeof(T) * source.size(),
                    .alignment = alignof(T),
                });

                T* const result = reinterpret_cast<T*>(allocation.address);

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
        void query_memory_usage(size_t& allocated, size_t& reserved) const;
    };
}
