#pragma once
#include "weave/memory/TypedLinearAllocator.hxx"

#include <string_view>
#include <vector>

namespace weave::stringpool
{
    class StringPool
    {
    private:
        struct Entry final
        {
            Entry* next{};
            std::string_view value{};
            uint64_t hash{};
        };

    private:
        memory::LinearAllocator _storage{};
        memory::TypedLinearAllocator<Entry> _entries{};

        std::vector<Entry*> _mapping{4096};
        size_t _count{};

        static constexpr size_t RehashFactor = 4u;

    private:
        [[nodiscard]] std::string_view intern(std::string_view value);

    private:
        Entry** try_find(uint64_t hash, std::string_view value, size_t& chainLength);

    private:
        void rehash();

    public:
        [[nodiscard]] std::string_view get(std::string_view value);

        void dump() const;

        void query_memory_usage(size_t& allocated, size_t& reserved) const
        {
            if (size_t const size = this->_mapping.size() * sizeof(Entry*); size != 0)
            {
                allocated += size;
                reserved += size;
            }

            this->_storage.query_memory_usage(allocated, reserved);
            this->_entries.query_memory_usage(allocated, reserved);
        }

        size_t count() const
        {
            return this->_count;
        }

        void enumerate(void* context, bool (*callback)(void*, std::string_view)) const;

        template <typename CallbackT = bool(std::string_view)>
        void enumerate(CallbackT&& callback)
        {
            this->enumerate(&callback, [](void* context, std::string_view value)
                {
                    (*static_cast<CallbackT*>(context))(value);
                });
        }
    };
}
