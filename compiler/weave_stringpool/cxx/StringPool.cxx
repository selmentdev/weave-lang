#include "weave/stringpool/StringPool.hxx"
#include "weave/hash/Fnv1a.hxx"

#include <fmt/format.h>

namespace weave::stringpool
{
    std::string_view StringPool::Intern(std::string_view value)
    {
        char* buffer = reinterpret_cast<char*>(this->_storage.Allocate(memory::Layout{value.length() + 1, alignof(char)}).Address);
        std::memcpy(buffer, value.data(), value.length());
        buffer[value.length()] = '\0';

        // ReSharper disable once CppDFALocalValueEscapesFunction
        // Value is allocated from allocator.
        return std::string_view{buffer, value.length()};
    }

    StringPool::Entry** StringPool::TryFind(uint64_t hash, std::string_view value, size_t& chainLength)
    {
        size_t const index = hash % this->_mapping.size();
        Entry** entry = &this->_mapping[index];

        while (*entry != nullptr)
        {
            if (((*entry)->Hash == hash) and ((*entry)->Value == value))
            {
                // Found the place where matching entry is.
                break;
            }

            ++chainLength;
            entry = &(*entry)->Next;
        }

        // Entry couldn't be found here, but if we need to place a new one, this is the location.
        return entry;
    }

    void StringPool::Rehash()
    {
        std::vector<Entry*> mapping{this->_mapping.size() * 2};
        size_t const old_size = this->_mapping.size();

        for (size_t i = 0; i < old_size; ++i)
        {
            Entry* entry = this->_mapping[i];

            while (entry != nullptr)
            {
                Entry* const next = entry->Next;
                size_t const index = entry->Hash % mapping.size();
                entry->Next = mapping[index];
                mapping[index] = entry;
                entry = next;
            }
        }

        this->_mapping = std::move(mapping);
    }

    std::string_view StringPool::Get(std::string_view value)
    {
        if (this->_count >= (this->_mapping.size() * RehashFactor))
        {
            this->Rehash();
        }

        uint64_t const hash = hash::Fnv1a64::FromString(value);

        size_t chain_length{};

        Entry** entry = this->TryFind(hash, value, chain_length);

        if (*entry == nullptr)
        {
            // Entry doesn't exist, create a new one.
            *entry = this->_entries.Emplace(
                nullptr,
                this->Intern(value),
                hash);
            ++this->_count;
        }

        return (*entry)->Value;
    }

    void StringPool::Dump() const
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
                    fmt::println("  hash: {:016X}, value: {}", entry->Hash, entry->Value);
                    entry = entry->Next;
                } while (entry != nullptr);
            }
        }
    }

    void StringPool::Enumerate(void* context, bool (*callback)(void*, std::string_view)) const
    {
        for (Entry const* bucket : this->_mapping)
        {
            Entry const* current = bucket;

            while (current != nullptr)
            {
                if (callback(context, current->Value))
                {
                    return;
                }

                current = current->Next;
            }
        }
    }
}
