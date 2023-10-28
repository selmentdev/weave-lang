#pragma once
#include <span>
#include <cstddef>
#include <vector>

#include "Weave.Core/Assert.hxx"

namespace Weave::Memory
{
    class VirtualHeap
    {
    private:
        struct Segment
        {
            std::byte* First{}; // First byte of the segment.
            std::byte* Last{}; // Last allocated byte of the segment.
            std::byte* End{}; // Pointer to the end of the segment.
        };

    private:
        std::vector<Segment> m_Segments{};

        static constexpr size_t DefaultSegmentSize = 64u << 10u;

    private:
        Segment* AllocateNewSegment(size_t size);
        static void* TryAllocateFromSegment(Segment& segment, size_t size, size_t alignment);

    public:
        VirtualHeap() = default;
        VirtualHeap(VirtualHeap const&) = delete;
        VirtualHeap(VirtualHeap&& other) = default;
        VirtualHeap& operator=(VirtualHeap const&) = delete;
        VirtualHeap& operator=(VirtualHeap&&) = default;
        ~VirtualHeap();


    public:
        [[nodiscard]] void* Allocate(size_t size)
        {
            return this->Allocate(size, alignof(std::max_align_t));
        }

        [[nodiscard]] void* Allocate(size_t size, size_t alignment);

        void Reset();
    };
}

namespace Weave::Memory
{
    template <typename T>
    class VirtualHeapHandle final
    {
    private:
        T* m_Pointer;

    public:
        VirtualHeapHandle(T* pointer)
            : m_Pointer{pointer}
        {
        }

        VirtualHeapHandle(VirtualHeapHandle const&) = delete;

        VirtualHeapHandle(VirtualHeapHandle&& other) noexcept
            : m_Pointer{std::exchange(other.m_Pointer, nullptr)}
        {
        }

        VirtualHeapHandle& operator=(VirtualHeapHandle const&) = delete;

        VirtualHeapHandle& operator=(VirtualHeapHandle&& other)
        {
            if (this != std::addressof(other))
            {
                if (this->m_Pointer != nullptr)
                {
                    std::destroy_at(this->m_Pointer);
                }

                this->m_Pointer = std::exchange(other.m_Pointer, nullptr);
            }

            return *this;
        }

        ~VirtualHeapHandle()
        {
            if (this->m_Pointer != nullptr)
            {
                std::destroy_at(this->m_Pointer);
            }
        }

        T const* operator->() const
        {
            return this->m_Pointer;
        }

        T* operator->()
        {
            return this->m_Pointer;
        }

        T* Get() const
        {
            return this->m_Pointer;
        }

        explicit operator bool() const
        {
            return this->m_Pointer != nullptr;
        }
    };

    template <typename T, typename... Args>
    VirtualHeapHandle<T> NewObject(VirtualHeap& heap, Args&&... args)
    {
        T* ptr = static_cast<T*>(heap.Allocate(sizeof(T), alignof(T)));

        if constexpr (not std::is_trivially_constructible_v<T>)
        {
            ptr = std::construct_at(ptr, std::forward<Args>(args)...);
        }

        return VirtualHeapHandle<T>{ptr};
    }
}

namespace Weave::Memory
{
    template <typename T>
    class VirtualHeapArray final
    {
    private:
        T* m_Pointer;
        size_t m_Size;

    public:
        VirtualHeapArray(T* pointer, size_t size)
            : m_Pointer{pointer}
            , m_Size{size}
        {
        }

        VirtualHeapArray(VirtualHeapArray const&) = delete;

        VirtualHeapArray(VirtualHeapArray&& other) noexcept
            : m_Pointer{std::exchange(other.m_Pointer, nullptr)}
            , m_Size{std::exchange(other.m_Size, 0u)}
        {
        }

        VirtualHeapArray& operator=(VirtualHeapArray const&) = delete;

        VirtualHeapArray& operator=(VirtualHeapArray&& other)
        {
            if (this != std::addressof(other))
            {
                if constexpr (not std::is_trivially_destructible_v<T>)
                {
                    if (this->m_Pointer != nullptr)
                    {
                        WEAVE_ASSERT(this->m_Size != 0u);
                        std::destroy_n(this->m_Pointer, this->m_Size);
                    }
                }

                this->m_Pointer = std::exchange(other.m_Pointer, nullptr);
                this->m_Size = std::exchange(other.m_Size, 0u);
            }

            return *this;
        }

        ~VirtualHeapArray()
        {
            if constexpr (not std::is_trivially_destructible_v<T>)
            {
                if (this->m_Pointer != nullptr)
                {
                    WEAVE_ASSERT(this->m_Size != 0u);
                    std::destroy_n(this->m_Pointer, this->m_Size);
                }
            }
        }

        T& operator[](size_t index) const
        {
            WEAVE_ASSERT(index < this->m_Size);
            return this->m_Pointer[index];
        }

        T* GetPointer() const
        {
            return this->m_Pointer;
        }

        size_t GetSize() const
        {
            return this->m_Size;
        }

        std::span<T> AsSpan()
        {
            return {this->m_Pointer, this->m_Size};
        }

        std::span<T const> AsSpan() const
        {
            return {this->m_Pointer, this->m_Size};
        }
    };

    template <typename T>
    VirtualHeapArray<T> NewArray(VirtualHeap& heap, size_t size)
    {
        T* ptr = static_cast<T*>(heap.Allocate(sizeof(T) * size, alignof(T)));

        if constexpr (not std::is_trivially_default_constructible_v<T>)
        {
            for (size_t i = 0; i < size; ++i)
            {
                std::construct_at(ptr + i);
            }
        }

        return VirtualHeapArray<T>{ptr, size};
    }
}
