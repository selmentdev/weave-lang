#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <fmt/format.h>

#include "Weave.Core/Bits.hxx"
#include "Weave.Core/Platform/Compiler.hxx"

namespace Weave::Memory
{
    struct Layout
    {
        size_t Size{};
        size_t Alignment{};
    };

    struct Allocation
    {
        std::byte* Address{};
        size_t Size{};
    };
}

namespace Weave::Memory
{
    class LinearAllocator
    {
        static constexpr size_t DefaultSegmentSize{64u << 10u};

    protected:
        struct Segment
        {
            Segment* BLink{};
            Segment* FLink{};
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
        size_t _segmentSize;
        size_t _allocatedSegmentsSize{};

    public:
        LinearAllocator();
        LinearAllocator(size_t segmentSize);
        ~LinearAllocator();

        LinearAllocator(LinearAllocator const&) = delete;
        LinearAllocator(LinearAllocator&& other) noexcept;

        LinearAllocator& operator=(LinearAllocator const&) = delete;
        LinearAllocator& operator=(LinearAllocator&& other) noexcept;

    protected:
        Segment* AllocateSegment(size_t size);

        Allocation AllocateImpl(Layout const& layout);

        constexpr bool NeedsSeparateSegment(size_t size) const
        {
            return size > (this->_segmentSize / 4);
        }

        // Inserts `other` before `self` segment.
        static constexpr void InsertBefore(SegmentList& list, Segment* self, Segment* other)
        {
            other->BLink = self->BLink;
            other->FLink = self;

            if (self->BLink != nullptr)
            {
                self->BLink->FLink = other;
            }
            else
            {
                list.Head = other;
            }

            self->BLink = other;
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
                list.Tail->FLink = segment;
                segment->BLink = list.Tail;
                list.Tail = segment;
            }
        }

    public:
        Allocation Allocate(Layout const& layout)
        {
            std::byte* const result = AlignUp(this->_list.Tail->Last, layout.Alignment);
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
        [[nodiscard]] T* Create(ArgsT&&... args)
        {
            static_assert(std::is_trivially_destructible_v<T>);

            Allocation const allocation = this->Allocate(Layout{
                .Size = sizeof(T),
                .Alignment = alignof(T),
            });

            return new (allocation.Address) T(std::forward<ArgsT>(args)...);
        }

        template <typename T>
        [[nodiscard]] std::span<T> CreateArray(size_t size)
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
        [[nodiscard]] std::span<T> CreateArray(std::span<T const> source)
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

namespace Weave::Memory
{
    template <typename T>
    class TypedLinearAllocator : public LinearAllocator
    {
        static constexpr size_t DefaultCapacity = 1024;

    public:
        TypedLinearAllocator()
            : TypedLinearAllocator{DefaultCapacity}
        {
        }

        TypedLinearAllocator(size_t capacity)
            : LinearAllocator{sizeof(T) * capacity}
        {
        }

        ~TypedLinearAllocator()
        {
            Segment* segment = this->_list.Head;

            while (segment != nullptr)
            {
                std::byte* const memory = reinterpret_cast<std::byte*>(segment + 1);
                T* const first = reinterpret_cast<T*>(AlignUp(memory, alignof(T)));
                T* const last = reinterpret_cast<T*>(segment->Last);

                std::destroy(first, last);

                ASAN_POISON_MEMORY_REGION(first, (last - first) * sizeof(T));

                segment = segment->FLink;
            }
        }

        TypedLinearAllocator(TypedLinearAllocator const&) = delete;
        TypedLinearAllocator(TypedLinearAllocator&&) = default;

        TypedLinearAllocator& operator=(TypedLinearAllocator const&) = delete;
        TypedLinearAllocator& operator=(TypedLinearAllocator&&) = default;

    public:
        template <typename CallbackT = bool(T*)>
        void Enumerate(CallbackT&& callback)
        {
            Segment* segment = this->_list.Head;

            while (segment != nullptr)
            {
                std::byte* const memory = reinterpret_cast<std::byte*>(segment + 1);
                T* first = reinterpret_cast<T*>(AlignUp(memory, alignof(T)));
                T* const last = reinterpret_cast<T*>(segment->Last);

                while (first != last)
                {
                    if (callback(first))
                    {
                        return;
                    }

                    ++first;
                }

                segment = segment->FLink;
            }
        }

    public:
        template <typename... ArgsT>
        [[nodiscard]] T* Create(ArgsT&&... args)
        {
            Allocation const allocation = this->Allocate(Layout{.Size = sizeof(T), .Alignment = alignof(T)});

            return new (allocation.Address) T(std::forward<ArgsT>(args)...);
        }

        [[nodiscard]] std::span<T> CreateArray(size_t size)
        {
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

        [[nodiscard]] std::span<T> CreateArray(std::span<T const> source)
        {
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

        [[nodiscard]] std::span<T> CreateCopyCombined(std::span<T const> source1, std::span<T const> source2) [[deprecated("Use something better than this")]]
        {
            size_t const count = source1.size() + source2.size();

            if (count != 0)
            {
                Allocation const allocation = this->Allocate(Layout{
                    .Size = sizeof(T) * count,
                    .Alignment = alignof(T),
                });

                T* const result = reinterpret_cast<T*>(allocation.Address);

                T* out = std::uninitialized_copy_n(
                    source1.data(),
                    source1.size(),
                    result);

                std::uninitialized_copy_n(
                    source2.data(),
                    source2.size(),
                    out);

                return std::span<T>{
                    result,
                    count,
                };
            }

            return {};
        }
    };
}
