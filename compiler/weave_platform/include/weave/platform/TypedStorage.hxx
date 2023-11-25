#pragma once
#include <cstddef>
#include <memory>

namespace weave::platform
{
    template <size_t Size, size_t Alignment>
    struct TypedStorage
    {
        alignas(Alignment) std::byte Storage[Size];

        template <typename T, typename... Args>
        T& Create(Args&&... args)
            requires((sizeof(T) <= Size) && (alignof(T) <= Alignment))
        {
            return *std::construct_at(static_cast<T*>(static_cast<void*>(this->Storage)), std::forward<Args>(args)...);
        }

        template <typename T>
        void Destroy()
            requires((sizeof(T) <= Size) && (alignof(T) <= Alignment))
        {
            std::destroy_at(&this->Get<T>());
        }

        template <typename T>
        T& Get()
            requires((sizeof(T) <= Size) && (alignof(T) <= Alignment))
        {
            return reinterpret_cast<T&>(*this);
        }

        template <typename T>
        T const& Get() const
            requires((sizeof(T) <= Size) && (alignof(T) <= Alignment))
        {
            return reinterpret_cast<T const&>(*this);
        }
    };
}
