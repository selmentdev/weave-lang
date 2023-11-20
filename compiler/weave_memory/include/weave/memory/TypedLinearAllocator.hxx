#pragma once
#include "weave/memory/LinearAllocator.hxx"

namespace weave::memory
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

        explicit TypedLinearAllocator(size_t capacity)
            : LinearAllocator{sizeof(T) * capacity}
        {
        }

        ~TypedLinearAllocator()
        {
            Segment* segment = this->_list.Head;

            while (segment != nullptr)
            {
                std::byte* const memory = reinterpret_cast<std::byte*>(segment + 1);
                T* const first = reinterpret_cast<T*>(bitwise::AlignUp(memory, alignof(T)));
                T* const last = reinterpret_cast<T*>(segment->Last);

                std::destroy(first, last);

                ASAN_POISON_MEMORY_REGION(first, (last - first) * sizeof(T));

                segment = segment->ForwardLink;
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
                T* first = reinterpret_cast<T*>(bitwise::AlignUp(memory, alignof(T)));
                T* const last = reinterpret_cast<T*>(segment->Last);

                while (first != last)
                {
                    if (callback(first))
                    {
                        return;
                    }

                    ++first;
                }

                segment = segment->ForwardLink;
            }
        }

    public:
        template <typename... ArgsT>
        [[nodiscard]] T* Emplace(ArgsT&&... args)
        {
            Allocation const allocation = this->Allocate(Layout{
                .Size = sizeof(T),
                .Alignment = alignof(T),
            });

            return new (allocation.Address) T(std::forward<ArgsT>(args)...);
        }

        [[nodiscard]] std::span<T> EmplaceArray(size_t size)
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

        [[nodiscard]] std::span<T> EmplaceArray(std::span<T const> source)
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

        [[nodiscard]] std::span<T> EmplaceArrayCombined(std::span<T const> source1, std::span<T const> source2) [[deprecated("Use something better than this")]]
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
