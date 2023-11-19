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

        TypedLinearAllocator(size_t capacity)
            : LinearAllocator{sizeof(T) * capacity}
        {
        }

        ~TypedLinearAllocator()
        {
            Segment* segment = this->_list.head;

            while (segment != nullptr)
            {
                std::byte* const memory = reinterpret_cast<std::byte*>(segment + 1);
                T* const first = reinterpret_cast<T*>(bitwise::AlignUp(memory, alignof(T)));
                T* const last = reinterpret_cast<T*>(segment->last);

                std::destroy(first, last);

                ASAN_POISON_MEMORY_REGION(first, (last - first) * sizeof(T));

                segment = segment->flink;
            }
        }

        TypedLinearAllocator(TypedLinearAllocator const&) = delete;
        TypedLinearAllocator(TypedLinearAllocator&&) = default;

        TypedLinearAllocator& operator=(TypedLinearAllocator const&) = delete;
        TypedLinearAllocator& operator=(TypedLinearAllocator&&) = default;

    public:
        template <typename CallbackT = bool(T*)>
        void enumerate(CallbackT&& callback)
        {
            Segment* segment = this->_list.head;

            while (segment != nullptr)
            {
                std::byte* const memory = reinterpret_cast<std::byte*>(segment + 1);
                T* first = reinterpret_cast<T*>(bitwise::AlignUp(memory, alignof(T)));
                T* const last = reinterpret_cast<T*>(segment->last);

                while (first != last)
                {
                    if (callback(first))
                    {
                        return;
                    }

                    ++first;
                }

                segment = segment->flink;
            }
        }

    public:
        template <typename... ArgsT>
        [[nodiscard]] T* create(ArgsT&&... args)
        {
            Allocation const allocation = this->allocate(Layout{
                .size = sizeof(T),
                .alignment = alignof(T),
            });

            return new (allocation.address) T(std::forward<ArgsT>(args)...);
        }

        [[nodiscard]] std::span<T> create_array(size_t size)
        {
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

        [[nodiscard]] std::span<T> create_array(std::span<T const> source)
        {
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

        [[nodiscard]] std::span<T> create_copy_combined(std::span<T const> source1, std::span<T const> source2) [[deprecated("Use something better than this")]]
        {
            size_t const count = source1.size() + source2.size();

            if (count != 0)
            {
                Allocation const allocation = this->allocate(Layout{
                    .size = sizeof(T) * count,
                    .alignment = alignof(T),
                });

                T* const result = reinterpret_cast<T*>(allocation.address);

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
