#include "weave/stringpool/stringpool.hxx"
#include "weave/hash/fnv1a.hxx"

#include <fmt/format.h>

namespace weave::stringpool
{
    std::string_view StringPool::intern(std::string_view value)
    {
        char* buffer = reinterpret_cast<char*>(this->_storage.allocate(memory::Layout{value.length() + 1, alignof(char)}).address);
        std::memcpy(buffer, value.data(), value.length());
        buffer[value.length()] = '\0';
        return std::string_view{buffer, value.length()};
    }

    StringPool::Entry** StringPool::try_find(uint64_t hash, std::string_view value, size_t& chainLength)
    {
        size_t const index = hash % this->_mapping.size();
        Entry** entry = &this->_mapping[index];

        while (*entry != nullptr)
        {
            if (((*entry)->hash == hash) and ((*entry)->value == value))
            {
                // Found the place where matching entry is.
                break;
            }

            ++chainLength;
            entry = &(*entry)->next;
        }

        // Entry couldn't be found here, but if we need to place a new one, this is the location.
        return entry;
    }

    void StringPool::rehash()
    {
        std::vector<Entry*> mapping{this->_mapping.size() * 2};
        size_t const old_size = this->_mapping.size();

        for (size_t i = 0; i < old_size; ++i)
        {
            Entry* entry = this->_mapping[i];

            while (entry != nullptr)
            {
                Entry* const next = entry->next;
                size_t const index = entry->hash % mapping.size();
                entry->next = mapping[index];
                mapping[index] = entry;
                entry = next;
            }
        }

        this->_mapping = std::move(mapping);
    }

    std::string_view StringPool::get(std::string_view value)
    {
        if (this->_count >= (this->_mapping.size() * RehashFactor))
        {
            this->rehash();
        }

        uint64_t const hash = hash::Fnv1a64::FromString(value);

        size_t chain_length{};

        Entry** entry = this->try_find(hash, value, chain_length);

        if (*entry == nullptr)
        {
            // Entry doesn't exist, create a new one.
            *entry = this->_entries.create(
                nullptr,
                this->intern(value),
                hash);
            ++this->_count;
        }

        return (*entry)->value;
    }

    void StringPool::dump() const
    {
        fmt::println("StringTable: {} entries, {} buckets", this->_count, this->_mapping.size());


        for (size_t i = 0; i < this->_mapping.size(); ++i)
        {
            Entry* entry = this->_mapping[i];

            if (entry != nullptr)
            {
                fmt::println("Bucket {}: ", i);

                do
                {
                    fmt::println("  hash: {:016X}, value: {}", entry->hash, entry->value);
                    entry = entry->next;
                } while (entry != nullptr);
            }
        }
    }

    void StringPool::enumerate(void* context, bool (*callback)(void*, std::string_view)) const
    {
        for (Entry const* bucket : this->_mapping)
        {
            Entry const* current = bucket;

            while (current != nullptr)
            {
                if (callback(context, current->value))
                {
                    return;
                }

                current = current->next;
            }
        }
    }
}
