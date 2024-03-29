#pragma once
#include <cstddef>

#include "PageAllocator.hxx"
#include "weave/Bitwise.hxx"
#include "weave/bugcheck/Assert.hxx"
#include "weave/bugcheck/BugCheck.hxx"

namespace weave::memory
{
    inline constexpr size_t PageSize = size_t{4} << 10u;
    inline constexpr size_t PageAlignment = size_t{4} << 10u;
    inline constexpr size_t PageGranularity = size_t{64} << 10u;

    struct PageAllocationResult final
    {
        void* Pointer;
        size_t Size;
        size_t Alignment;
    };

    PageAllocationResult PageAllocate(size_t size);

    void PageDeallocate(void* pointer);
}

#define WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION() \
    fmt::println("{}", __FUNCTION__)

namespace weave::memory::v2
{
    inline constexpr size_t PageFrameGranularity = size_t{64} << 10u;

    inline constexpr uint32_t PageFrameFlags_None = 0u;
    inline constexpr uint32_t PageFrameFlags_Used = 1u << 1u;
    inline constexpr uint32_t PageFrameFlags_Fixed = 1u << 2u;

    struct PageFrameDescriptor;

    struct PageFrameDescriptorListEntry final
    {
        PageFrameDescriptor* FLink;
        PageFrameDescriptor* BLink;
    };
    static_assert(sizeof(PageFrameDescriptorListEntry) == 16);

    struct PageFrameDescriptorList final
    {
        PageFrameDescriptor* Head;
        PageFrameDescriptor* Tail;
    };
    static_assert(sizeof(PageFrameDescriptorList) == 16);

    struct PageFrameDescriptor final
    {
        PageFrameDescriptorListEntry Entry;
        std::byte* Base;
        std::byte* Limit;
        void* UserPointer;
        uint32_t Tag;
        uint32_t Flags;
    };
    static_assert(sizeof(PageFrameDescriptor) == 48);

    inline void PageFrameDescriptor_InitializeAsLookaside(PageFrameDescriptor& descriptor)
    {
        descriptor.Entry.FLink = nullptr;
        descriptor.Entry.BLink = nullptr;
        descriptor.Base = 0;
        descriptor.Limit = 0;
        descriptor.UserPointer = nullptr;
        descriptor.Tag = 0;
        descriptor.Flags = 0;
    }

    inline bool PageFrameDescriptor_Contains(PageFrameDescriptor const& descriptor, std::byte* address)
    {
        return (descriptor.Base <= address) && (address < descriptor.Limit);
    }

    inline bool PageFrameDescriptor_Overlaps(PageFrameDescriptor const& descriptor, std::byte* base, std::byte* limit)
    {
        return (descriptor.Base < limit) && (base < descriptor.Limit);
    }

    inline bool PageFrameDescriptor_Overlaps(PageFrameDescriptor const& descriptor, PageFrameDescriptor const& other)
    {
        return PageFrameDescriptor_Overlaps(descriptor, other.Base, other.Limit);
    }

    inline bool PageFrameDescriptor_IsUsed(PageFrameDescriptor const& descriptor)
    {
        return (descriptor.Flags & PageFrameFlags_Used) != 0;
    }

    inline bool PageFrameDescriptor_IsFree(PageFrameDescriptor const& descriptor)
    {
        return (descriptor.Flags & PageFrameFlags_Used) == 0;
    }

    inline bool PageFrameDescriptor_IsFixed(PageFrameDescriptor const& descriptor)
    {
        return (descriptor.Flags & PageFrameFlags_Fixed) != 0;
    }

    inline void PageFrameDescriptor_SetUsed(PageFrameDescriptor& descriptor)
    {
        descriptor.Flags |= PageFrameFlags_Used;
    }

    inline void PageFrameDescriptor_SetFree(PageFrameDescriptor& descriptor)
    {
        descriptor.Flags &= ~PageFrameFlags_Used;
    }

    inline void PageFrameDescriptor_SetFixed(PageFrameDescriptor& descriptor)
    {
        descriptor.Flags |= PageFrameFlags_Fixed;
    }

    inline void PageFrameDescriptor_ClearFixed(PageFrameDescriptor& descriptor)
    {
        descriptor.Flags &= ~PageFrameFlags_Fixed;
    }

    inline uintptr_t PageFrameDescriptor_GetSize(PageFrameDescriptor const& descriptor)
    {
        return descriptor.Limit - descriptor.Base;
    }

    inline void PageFrameDescriptorList_Initialize(PageFrameDescriptorList& list)
    {
        list.Head = nullptr;
        list.Tail = nullptr;
    }

    inline size_t PageFrameDescriptorList_GetCount(PageFrameDescriptorList const& list)
    {
        size_t count = 0;

        for (PageFrameDescriptor const* descriptor = list.Head; descriptor != nullptr; descriptor = descriptor->Entry.FLink)
        {
            count++;
        }

        return count;
    }

    inline void PageFrameDescriptorList_PushBack(PageFrameDescriptorList& list, PageFrameDescriptor& entry)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        if (list.Head == nullptr)
        {
            WEAVE_ASSERT(list.Tail == nullptr);
            list.Head = &entry;
            list.Tail = &entry;

            entry.Entry.FLink = nullptr;
            entry.Entry.BLink = nullptr;
        }
        else
        {
            WEAVE_ASSERT(list.Tail != nullptr);

            list.Tail->Entry.FLink = &entry;
            entry.Entry.BLink = list.Tail;
            entry.Entry.FLink = nullptr;
            list.Tail = &entry;
        }
    }

    inline void PageFrameDescriptorList_PushFront(PageFrameDescriptorList& list, PageFrameDescriptor& entry)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        if (list.Head == nullptr)
        {
            WEAVE_ASSERT(list.Tail == nullptr);
            list.Head = &entry;
            list.Tail = &entry;

            entry.Entry.FLink = nullptr;
            entry.Entry.BLink = nullptr;
        }
        else
        {
            WEAVE_ASSERT(list.Tail != nullptr);

            list.Head->Entry.BLink = &entry;
            entry.Entry.FLink = list.Head;
            entry.Entry.BLink = nullptr;
            list.Head = &entry;
        }
    }

    inline void PageFrameDescriptorList_Remove(PageFrameDescriptorList& list, PageFrameDescriptor& entry)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        if (entry.Entry.BLink != nullptr)
        {
            entry.Entry.BLink->Entry.FLink = entry.Entry.FLink;
        }
        else
        {
            list.Head = entry.Entry.FLink;
        }

        if (entry.Entry.FLink != nullptr)
        {
            entry.Entry.FLink->Entry.BLink = entry.Entry.BLink;
        }
        else
        {
            list.Tail = entry.Entry.BLink;
        }
    }

    inline PageFrameDescriptor* PageFrameDescriptorList_PopFront(PageFrameDescriptorList& list)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        if (list.Head == nullptr)
        {
            return nullptr;
        }

        auto entry = list.Head;
        list.Head = entry->Entry.FLink;

        if (list.Head == nullptr)
        {
            list.Tail = nullptr;
        }
        else
        {
            list.Head->Entry.BLink = nullptr;
        }

        return entry;
    }

    inline PageFrameDescriptor* PageFrameDescriptorList_PopBack(PageFrameDescriptorList& list)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        if (list.Tail == nullptr)
        {
            return nullptr;
        }

        auto entry = list.Tail;
        list.Tail = entry->Entry.BLink;

        if (list.Tail == nullptr)
        {
            list.Head = nullptr;
        }
        else
        {
            list.Tail->Entry.FLink = nullptr;
        }

        return entry;
    }

    inline void PageFrameDescriptorList_InsertAfter(PageFrameDescriptorList& list, PageFrameDescriptor& entry, PageFrameDescriptor& after)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        entry.Entry.FLink = after.Entry.FLink;
        entry.Entry.BLink = &after;
        after.Entry.FLink = &entry;

        if (entry.Entry.FLink != nullptr)
        {
            entry.Entry.FLink->Entry.BLink = &entry;
        }
        else
        {
            list.Tail = &entry;
        }
    }

    inline void PageFrameDescriptorList_InsertBefore(PageFrameDescriptorList& list, PageFrameDescriptor& entry, PageFrameDescriptor& before)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        entry.Entry.FLink = &before;
        entry.Entry.BLink = before.Entry.BLink;
        before.Entry.BLink = &entry;

        if (entry.Entry.BLink != nullptr)
        {
            entry.Entry.BLink->Entry.FLink = &entry;
        }
        else
        {
            list.Head = &entry;
        }
    }

    struct PageFrameDatabaseHeader final
    {
        // List of page frame regions with free memory.
        PageFrameDescriptorList Allocated{};

        // List of free lookaside entries. Used to get page frame region descriptors
        PageFrameDescriptorList Lookaside{};
    };

    struct PageFrameDatabase final
    {
        PageFrameDatabaseHeader Header;
        PageFrameDescriptor Descriptors[(PageFrameGranularity - sizeof(PageFrameDatabaseHeader)) / sizeof(PageFrameDescriptor)];
        //PageFrameDescriptor Descriptors[1];
    };
    static_assert(sizeof(PageFrameDatabase) <= PageFrameGranularity);

    inline void PageFrameDatabase_Initialize(PageFrameDatabase& database);
    inline void* PageFrameDatabase_AllocatePages(PageFrameDatabase& database, size_t size, size_t alignment, void* user_data);
    inline PageFrameDescriptor& PageFrameDatabase_AllocateDescriptor(PageFrameDatabase& database);
    inline PageFrameDescriptor* PageFrameDatabase_FindBestFreePageFrameDescriptor(
        PageFrameDatabase const& database,
        size_t size);

    inline void PageFrameDatabase_Validate(PageFrameDatabase const& database);

    // Initializes Page Frame Database.
    inline void PageFrameDatabase_Initialize(PageFrameDatabase& database)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        //
        // Initialize page frame descriptor lists.
        //

        PageFrameDescriptorList_Initialize(database.Header.Allocated);
        PageFrameDescriptorList_Initialize(database.Header.Lookaside);

        //
        // Populate look-aside list with pre-allocated descriptors.
        //

        for (PageFrameDescriptor& descriptor : database.Descriptors)
        {
            PageFrameDescriptorList_PushBack(database.Header.Lookaside, descriptor);
        }
    }

    inline void* PageFrameDatabase_AllocatePages(PageFrameDatabase& database, size_t size, size_t alignment, void* user_data)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        WEAVE_ASSERT(bitwise::IsAligned(size, PageFrameGranularity));
        WEAVE_ASSERT(bitwise::IsAligned(alignment, PageFrameGranularity));

        //
        // Find free page frame descriptor which can hold requested memory region.
        //

        PageFrameDescriptor* free_descriptor = PageFrameDatabase_FindBestFreePageFrameDescriptor(database, size);

        if (free_descriptor == nullptr)
        {
            WEAVE_BUGCHECK("Out of memory");
        }

        WEAVE_ASSERT(PageFrameDescriptor_IsFree(*free_descriptor));

        //
        // Split free descriptor into two parts.
        //

        if (PageFrameDescriptor_GetSize(*free_descriptor) != size)
        {
            //
            // Need to split descriptor into two parts. Allocate additional descriptor and split free memory region.
            //

            PageFrameDescriptor& adjacent = PageFrameDatabase_AllocateDescriptor(database);

            //
            // Transform
            // +-----------------------------------+
            // | free_descriptor                   |
            // +-----------------------------------+
            //
            // Into
            //
            // +-----------------+-----------------+
            // | free_descriptor |   adjacent      |
            // +-----------------+-----------------+
            //

            //
            // Copy properties of free descriptor to adjacent descriptor.
            //

            adjacent = *free_descriptor;

            //
            // Update adjacent descriptor to point after free descriptor.
            //
            adjacent.Base += size;
            free_descriptor->Limit = adjacent.Base;

            WEAVE_ASSERT(PageFrameDescriptor_IsFree(adjacent));

            PageFrameDescriptorList_InsertAfter(database.Header.Allocated, adjacent, *free_descriptor);
        }

        //
        // Mark free descriptor as used for user allocations.
        //

        PageFrameDescriptor_SetUsed(*free_descriptor);
        free_descriptor->UserPointer = user_data;

        return free_descriptor->Base;
    }

    inline PageFrameDescriptor* PageFrameDatabase_FindDescriptorContainingPointer(
        PageFrameDatabase const& database,
        std::byte* address)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        for (PageFrameDescriptor* descriptor = database.Header.Allocated.Head; descriptor != nullptr; descriptor = descriptor->Entry.FLink)
        {
            if (PageFrameDescriptor_Contains(*descriptor, address))
            {
                return descriptor;
            }
        }

        return nullptr;
    }

    inline void* PageFrameDatabase_AllocatePagesExplicit(
        PageFrameDatabase& database,
        void* address,
        size_t size,
        void* user_data)
    {
        //
        // Compute memory region bounds.
        //

        std::byte* const base = bitwise::AlignDown(static_cast<std::byte*>(address), PageFrameGranularity);
        std::byte* const limit = bitwise::AlignUp(static_cast<std::byte*>(address) + size, PageFrameGranularity);

        PageFrameDescriptor* descriptor = PageFrameDatabase_FindDescriptorContainingPointer(database, base);

        if (descriptor == nullptr)
        {
            //
            // Memory region not found.
            //

            return nullptr;
        }

        if (!PageFrameDescriptor_IsFree(*descriptor))
        {
            //
            // Memory region is already allocated.
            //

            return nullptr;
        }

        //
        // This is a free memory region. Check if it can hold requested memory region.
        //

        if (!PageFrameDescriptor_Overlaps(*descriptor, base, limit))
        {
            //
            // Memory region is too small.
            //

            return nullptr;
        }

        //
        // Memory region is large enough. Allocate it.
        //

        PageFrameDescriptor& allocated = *descriptor;

        //
        // Verify and split free memory region.
        //
        // | adjacent_before | allocated | adjacent_after |
        //

        bool const need_split_before = allocated.Base != base;

        if (need_split_before)
        {
            //
            // Need to split descriptor into two parts. Allocate additional descriptor and split free memory region.
            //

            PageFrameDescriptor& adjacent = PageFrameDatabase_AllocateDescriptor(database);

            //
            // Copy properties of free descriptor to adjacent descriptor.
            //

            adjacent = allocated;

            //
            // Update adjacent descriptor to point before allocated descriptor.
            //

            adjacent.Limit = base;
            allocated.Base = base;

            WEAVE_ASSERT(PageFrameDescriptor_IsFree(adjacent));

            PageFrameDescriptorList_InsertBefore(database.Header.Allocated, adjacent, allocated);
        }

        bool const need_split_after = allocated.Limit != limit;

        if (need_split_after)
        {
            //
            // Need to split descriptor into two parts. Allocate additional descriptor and split free memory region.
            //

            PageFrameDescriptor& adjacent = PageFrameDatabase_AllocateDescriptor(database);

            //
            // Copy properties of free descriptor to adjacent descriptor.
            //

            adjacent = allocated;

            //
            // Update adjacent descriptor to point after free descriptor.
            //

            adjacent.Base = limit;
            allocated.Limit = limit;

            WEAVE_ASSERT(PageFrameDescriptor_IsFree(adjacent));

            PageFrameDescriptorList_InsertAfter(database.Header.Allocated, adjacent, *descriptor);
        }

        //
        // Mark allocated descriptor as used.
        //

        PageFrameDescriptor_SetUsed(allocated);

        //
        // Update user data.
        //

        allocated.UserPointer = user_data;

        PageFrameDatabase_Validate(database);

        return allocated.Base;
    }

    inline void PageFrameDatabase_DeallocatePages(PageFrameDatabase& database, void* pointer)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        PageFrameDescriptor* descriptor = PageFrameDatabase_FindDescriptorContainingPointer(database, static_cast<std::byte*>(pointer));

        if (descriptor == nullptr)
        {
            WEAVE_BUGCHECK("Invalid pointer");
        }

        WEAVE_ASSERT(PageFrameDescriptor_IsUsed(*descriptor));

        PageFrameDescriptor_SetFree(*descriptor);

        //
        // Coalesce free memory regions.
        //

        if ((descriptor->Entry.FLink != nullptr) && PageFrameDescriptor_IsFree(*descriptor->Entry.FLink))
        {
            //
            // Merge descriptor with next free descriptor.
            //

            PageFrameDescriptor* next = descriptor->Entry.FLink;
            descriptor->Limit = next->Limit;

            //
            // Return descriptor to look-aside list.
            //

            PageFrameDescriptorList_Remove(database.Header.Allocated, *next);
            PageFrameDescriptorList_PushBack(database.Header.Lookaside, *next);
        }

        if ((descriptor->Entry.BLink != nullptr) && PageFrameDescriptor_IsFree(*descriptor->Entry.BLink))
        {
            //
            // Merge descriptor with previous free descriptor.
            //

            PageFrameDescriptor* prev = descriptor->Entry.BLink;
            prev->Limit = descriptor->Limit;

            //
            // Return descriptor to look-aside list.
            //

            PageFrameDescriptorList_Remove(database.Header.Allocated, *descriptor);
            PageFrameDescriptorList_PushBack(database.Header.Lookaside, *descriptor);
        }

        PageFrameDatabase_Validate(database);
    }

    inline void PageFrameDatabase_InsertAllocatedDescriptor(
        PageFrameDatabase& database,
        PageFrameDescriptor& descriptor)
    {
        //
        // Find best place to insert this descriptor.
        //

        PageFrameDescriptor* current = database.Header.Allocated.Head;

        while (current != nullptr)
        {
            if (descriptor.Base < current->Base)
            {
                break;
            }

            current = current->Entry.FLink;
        }

        if (current == nullptr)
        {
            //
            // Insert descriptor at the end of the list.
            //

            PageFrameDescriptorList_PushBack(database.Header.Allocated, descriptor);
        }
        else
        {
            //
            // Insert descriptor before current.
            //

            PageFrameDescriptorList_InsertBefore(database.Header.Allocated, descriptor, *current);
        }
    }

    inline PageFrameDescriptor* PageFrameDatabase_FindBestFreePageFrameDescriptor(
        PageFrameDatabase const& database,
        size_t size)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        PageFrameDescriptor* best_descriptor = nullptr;
        size_t best_size = 0;

        for (PageFrameDescriptor* descriptor = database.Header.Allocated.Head; descriptor != nullptr; descriptor = descriptor->Entry.FLink)
        {
            if (PageFrameDescriptor_IsUsed(*descriptor))
            {
                //
                // Only free descriptors are considered.
                //
                continue;
            }

            //
            // Compute descriptor size.
            //

            size_t const descriptor_size = PageFrameDescriptor_GetSize(*descriptor);

            if (descriptor_size == size)
            {
                //
                // Exact match found.
                //

                return descriptor;
            }

            if (descriptor_size >= size)
            {
                //
                // Check if descriptor is better than current best.
                //

                if (best_descriptor == nullptr || descriptor_size < best_size)
                {
                    best_descriptor = descriptor;
                    best_size = descriptor_size;
                }
            }
        }

        return best_descriptor;
    }

    inline PageFrameDescriptor& PageFrameDatabase_AllocateDescriptor(PageFrameDatabase& database)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        PageFrameDatabase_Validate(database);

        //
        // Try to get descriptor from look-aside list.
        //

        if (PageFrameDescriptor* descriptor = PageFrameDescriptorList_PopFront(database.Header.Lookaside))
        {
            return *descriptor;
        }


        //
        // No free descriptors available. Allocate a new page from list of free pages.
        //

        PageFrameDescriptor* allocated = PageFrameDatabase_FindBestFreePageFrameDescriptor(database, PageFrameGranularity);

        if (allocated == nullptr)
        {
            WEAVE_BUGCHECK("Out of memory");
        }

        WEAVE_ASSERT(PageFrameDescriptor_IsFree(*allocated));

        //
        // Populate look-aside list with new descriptors.
        //

        std::byte* const base = allocated->Base;
        std::byte* const limit = base + PageFrameGranularity;

        PageFrameDescriptor* first = reinterpret_cast<PageFrameDescriptor*>(base);
        PageFrameDescriptor* last = reinterpret_cast<PageFrameDescriptor*>(limit);

        for (PageFrameDescriptor* descriptor = first; descriptor < last; descriptor++)
        {
            PageFrameDescriptor_InitializeAsLookaside(*descriptor);
            PageFrameDescriptorList_PushBack(database.Header.Lookaside, *descriptor);
        }

        PageFrameDatabase_Validate(database);

        if (PageFrameDescriptor_GetSize(*allocated) != PageFrameGranularity)
        {
            //
            // Need to split descriptor into two parts. Allocate additional descriptor and split free memory region.
            //

            PageFrameDescriptor* adjacent = PageFrameDescriptorList_PopFront(database.Header.Lookaside);
            WEAVE_ASSERT(adjacent != nullptr);

            //
            // Copy properties of free descriptor to adjacent descriptor.
            //

            *adjacent = *allocated;

            //
            // Update adjacent descriptor to point after free descriptor.
            //
            adjacent->Base += PageFrameGranularity;
            allocated->Limit = adjacent->Base;

            WEAVE_ASSERT(PageFrameDescriptor_IsFree(*adjacent));
            WEAVE_ASSERT(!PageFrameDescriptor_IsFixed(*adjacent));

            PageFrameDatabase_InsertAllocatedDescriptor(database, *adjacent);
        }

        //
        // Mark free descriptor as used for internal allocations.
        //

        PageFrameDescriptor_SetUsed(*allocated);
        WEAVE_ASSERT(!PageFrameDescriptor_IsFixed(*allocated));
        PageFrameDescriptor_SetFixed(*allocated);

        //
        // Return new descriptor from look-aside list.
        //

        PageFrameDatabase_Validate(database);

        return *PageFrameDescriptorList_PopFront(database.Header.Lookaside);
    }

    inline void PageFrameDescriptorList_Validate(PageFrameDescriptorList const& list)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        //
        // Verify that list is consistent.
        //

        if (list.Head == nullptr)
        {
            WEAVE_ASSERT(list.Tail == nullptr);
        }
        else
        {
            WEAVE_ASSERT(list.Tail != nullptr);
        }

        //
        // Verify that list is doubly linked.
        //

        for (PageFrameDescriptor const* descriptor = list.Head; descriptor != nullptr; descriptor = descriptor->Entry.FLink)
        {
            if (descriptor->Entry.FLink != nullptr)
            {
                WEAVE_ASSERT(descriptor->Entry.FLink->Entry.BLink == descriptor);
            }
        }

        for (PageFrameDescriptor const* descriptor = list.Tail; descriptor != nullptr; descriptor = descriptor->Entry.BLink)
        {
            if (descriptor->Entry.BLink != nullptr)
            {
                WEAVE_ASSERT(descriptor->Entry.BLink->Entry.FLink == descriptor);
            }
        }
    }

    inline void PageFrameDatabase_Validate(PageFrameDatabase const& database)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        //
        // Verify that all descriptors are adjacent. Holes are allowed.
        //

        fmt::println("descriptors: --->");
        for (PageFrameDescriptor const* descriptor = database.Header.Allocated.Head; descriptor != nullptr; descriptor = descriptor->Entry.FLink)
        {
            WEAVE_ASSERT(descriptor->Base < descriptor->Limit);
            WEAVE_ASSERT(bitwise::IsAligned(descriptor->Base, PageFrameGranularity));
            WEAVE_ASSERT(bitwise::IsAligned(descriptor->Limit, PageFrameGranularity));

            if (descriptor->Entry.FLink != nullptr)
            {
                WEAVE_ASSERT(!PageFrameDescriptor_Overlaps(*descriptor, *descriptor->Entry.FLink));
                WEAVE_ASSERT(descriptor->Limit <= descriptor->Entry.FLink->Base);
            }

            if (descriptor->Entry.BLink != nullptr)
            {
                WEAVE_ASSERT(!PageFrameDescriptor_Overlaps(*descriptor, *descriptor->Entry.BLink));
                WEAVE_ASSERT(descriptor->Base >= descriptor->Entry.BLink->Limit);
            }

            fmt::println("Descriptor: {} - {}: flags: {:#08x}, tag: {:#08x}", fmt::ptr(descriptor->Base), fmt::ptr(descriptor->Limit), descriptor->Flags, descriptor->Tag);
        }
        fmt::println("descriptors: <---");

        fmt::println("allocated list: {} descriptors", PageFrameDescriptorList_GetCount(database.Header.Allocated));
        PageFrameDescriptorList_Validate(database.Header.Allocated);
        fmt::println("lookaside list: {} descriptors", PageFrameDescriptorList_GetCount(database.Header.Lookaside));
        PageFrameDescriptorList_Validate(database.Header.Lookaside);
    }

    inline void PageFrameDatabase_CreatePageFrame(
        PageFrameDatabase& database,
        void* memory,
        size_t size,
        void* user_pointer,
        uint32_t tag)
    {
        WEAVE_MEMORY_PAGE_FRAME_ALLOCATOR_FUNCTION();

        WEAVE_ASSERT(bitwise::IsAligned(memory, PageFrameGranularity));
        WEAVE_ASSERT(bitwise::IsAligned(size, PageFrameGranularity));

        //
        // Compute memory region bounds.
        //

        size_t const aligned_size = bitwise::AlignUp(size, PageFrameGranularity);
        std::byte* const base = bitwise::AlignDown(static_cast<std::byte*>(memory), PageFrameGranularity);
        std::byte* const limit = base + aligned_size;


        //
        // Verify that memory region does not overlap with existing page frame regions.
        //

        for (PageFrameDescriptor const* descriptor = database.Header.Allocated.Head; descriptor != nullptr; descriptor = descriptor->Entry.FLink)
        {
            if (PageFrameDescriptor_Overlaps(*descriptor, base, limit))
            {
                WEAVE_BUGCHECK("Memory region overlaps with existing page frame region");
            }
        }


        //
        // Create a page frame descriptor for mapped memory region.
        //

        PageFrameDescriptor& descriptor = PageFrameDatabase_AllocateDescriptor(database);
        descriptor.Base = base;
        descriptor.Limit = limit;
        descriptor.UserPointer = user_pointer;
        descriptor.Tag = tag;
        descriptor.Flags = PageFrameFlags_None;

        //
        // Insert descriptor into list of allocated page frame regions.
        //

        PageFrameDatabase_InsertAllocatedDescriptor(database, descriptor);

        PageFrameDatabase_Validate(database);
    }
}
