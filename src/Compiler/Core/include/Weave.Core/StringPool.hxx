#pragma once
#include <string_view>
#include <vector>

#include "Weave.Core/Memory/Allocator.hxx"

namespace Weave
{
    class StringPool
    {
    private:
        struct Entry final
        {
            Entry* Next{};
            std::string_view Value{};
            uint64_t Hash{};
        };

    private:
        Memory::LinearAllocator _storage{};
        Memory::TypedLinearAllocator<Entry> _entries{};

        std::vector<Entry*> _mapping{4096};
        size_t _count{};

        static constexpr size_t RehashFactor = 4u;

    private:
        [[nodiscard]] std::string_view Intern(std::string_view value);

    private:
        Entry** TryFind(uint64_t hash, std::string_view value, size_t& chainLength);

    private:
        void Rehash();

    public:
        [[nodiscard]] std::string_view Get(std::string_view value);

        void Dump() const;

        void QueryMemoryUsage(size_t& allocated, size_t& reserved) const
        {
            if (size_t const size = this->_mapping.size() * sizeof(Entry*); size != 0)
            {
                allocated += size;
                reserved += size;
            }

            this->_storage.QueryMemoryUsage(allocated, reserved);
            this->_entries.QueryMemoryUsage(allocated, reserved);
        }

        size_t Count() const
        {
            return this->_count;
        }

        void Enumerate(void* context, bool (*callback)(void*, std::string_view)) const;

        template <typename CallbackT = bool(std::string_view)>
        void Enumerate(CallbackT&& callback)
        {
            this->Enumerate(&callback, [](void* context, std::string_view value)
                {
                    (*static_cast<CallbackT*>(context))(value);
                });
        }
    };
}
