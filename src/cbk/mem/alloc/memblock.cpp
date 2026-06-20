/* *******************************************************************************
 *                                                                               *
 *  Copyright 2026 Trollycat                                                     *
 *                                                                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");              *
 *  you may not use this file except in compliance with the License.             *
 *  You may obtain a copy of the License at                                      *
 *                                                                               *
 *      http://www.apache.org/licenses/LICENSE-2.0                               *
 *                                                                               *
 *  Unless required by applicable law or agreed to in writing, software          *
 *  distributed under the License is distributed on an "AS IS" BASIS,            *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     *
 *  See the License for the specific language governing permissions and          *
 *  limitations under the License.                                               *
 *                                                                               *
 *********************************************************************************
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Memory block                                                       *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Memory allocator for early boot stage.                             *
 ********************************************************************************/
#include <cbk/mem/alloc/memblock.h>

#include <cbk/mem/alloc/freelist.h>

#include <assert.h>
#include <kmlayout.h>

#include <tklib/math.h>

namespace trunk::mem
{
    static MemoryRegion s_memory_regions[MAX_MEMBLOCK_REGIONS];
    static MemoryRegion s_reserved_regions[MAX_MEMBLOCK_REGIONS];

    static SIZE_T s_memory_count   = 0;
    static SIZE_T s_reserved_count = 0;

    static QWORD s_total_free     = 0;
    static QWORD s_total_reserved = 0;

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InsertionSortRegions                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Sort regions by base address                                       *
         ********************************************************************************/
        VOID InsertionSortRegions(MemoryRegion *regions, SIZE_T count) noexcept
        {
            for (SIZE_T i = 1; i < count; ++i) {
                MemoryRegion key = regions[i];

                LONG_PTR j = static_cast<LONG_PTR>(i) - 1;

                while (j >= 0 && regions[j].base > key.base) {
                    regions[j + 1] = regions[j];
                    --j;
                }

                regions[j + 1] = key;
            }
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MergeReservedRegions                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Merge overlapping reserved regions into one entry                  *
         ********************************************************************************/
        VOID MergeReservedRegions() noexcept
        {
            if (s_reserved_count < 2)
                return;

            SIZE_T write = 0;
            for (SIZE_T read = 1; read < s_reserved_count; ++read) {
                const QWORD cur_end =
                    s_reserved_regions[write].base + s_reserved_regions[write].size;
                const QWORD next_start = s_reserved_regions[read].base;
                const QWORD next_end   = next_start + s_reserved_regions[read].size;

                if (next_start <= cur_end) {
                    if (next_end > cur_end)
                        s_reserved_regions[write].size = next_end - s_reserved_regions[write].base;
                } else {
                    ++write;
                    s_reserved_regions[write] = s_reserved_regions[read];
                }
            }
            s_reserved_count = write + 1;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MergeMemoryRegions                                                  *
         * DATE    : 2026                                                                *
         * PURPOSE : Merge contiguous available free memory regions                      *
         ********************************************************************************/
        VOID MergeMemoryRegions() noexcept
        {
            if (s_memory_count < 2)
                return;

            SIZE_T write = 0;
            for (SIZE_T read = 1; read < s_memory_count; ++read) {
                const QWORD cur_end = s_memory_regions[write].base + s_memory_regions[write].size;
                const QWORD next_start = s_memory_regions[read].base;
                const QWORD next_end   = next_start + s_memory_regions[read].size;

                if (next_start == cur_end) {
                    s_memory_regions[write].size = next_end - s_memory_regions[write].base;
                } else {
                    ++write;
                    s_memory_regions[write] = s_memory_regions[read];
                }
            }
            s_memory_count = write + 1;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : CarveFreeRegion                                                    *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Remove [base, base + size) from the free list.                     *
         ********************************************************************************/
        BOOL CarveFreeRegion(QWORD base, QWORD size) noexcept
        {
            const QWORD end = base + size;

            for (SIZE_T i = 0; i < s_memory_count; ++i) {
                const QWORD region_start = s_memory_regions[i].base;
                const QWORD region_end   = region_start + s_memory_regions[i].size;

                if (base < region_start || end > region_end)
                    continue;

                const QWORD left_size  = base - region_start;
                const QWORD right_size = region_end - end;

                if (left_size > 0 && right_size > 0) {
                    s_memory_regions[i].size = left_size;
                    if (s_memory_count < MAX_MEMBLOCK_REGIONS)
                        s_memory_regions[s_memory_count++] = {end, right_size};
                } else if (left_size > 0) {
                    s_memory_regions[i].size = left_size;
                } else if (right_size > 0) {
                    s_memory_regions[i].base = end;
                    s_memory_regions[i].size = right_size;
                } else {
                    s_memory_regions[i] = s_memory_regions[--s_memory_count];
                }

                s_total_free -= size;
                return true;
            }

            return false;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : CarveReservedRegion                                                 *
         * DATE    : 2026                                                                *
         * PURPOSE : Remove [base, base + size) from the reserved tracking array.        *
         ********************************************************************************/
        BOOL CarveReservedRegion(QWORD base, QWORD size) noexcept
        {
            const QWORD end = base + size;

            for (SIZE_T i = 0; i < s_reserved_count; ++i) {
                const QWORD region_start = s_reserved_regions[i].base;
                const QWORD region_end   = region_start + s_reserved_regions[i].size;

                if (base < region_start || end > region_end)
                    continue;

                const QWORD left_size  = base - region_start;
                const QWORD right_size = region_end - end;

                if (left_size > 0 && right_size > 0) {
                    s_reserved_regions[i].size = left_size;
                    if (s_reserved_count < MAX_MEMBLOCK_REGIONS)
                        s_reserved_regions[s_reserved_count++] = {end, right_size};
                } else if (left_size > 0) {
                    s_reserved_regions[i].size = left_size;
                } else if (right_size > 0) {
                    s_reserved_regions[i].base = end;
                    s_reserved_regions[i].size = right_size;
                } else {
                    s_reserved_regions[i] = s_reserved_regions[--s_reserved_count];
                }

                s_total_reserved -= size;
                return true;
            }

            return false;
        }

    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockInit                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialization function for memblock                               *
     ********************************************************************************/
    VOID MemblockInit(const boot::BootInfo &boot_info) noexcept
    {
        s_memory_count   = 0;
        s_reserved_count = 0;

        s_total_free     = 0;
        s_total_reserved = 0;

        QWORD max_phys_addr = 0;

        for (SIZE_T i = 0; i < boot_info.mmap_count; ++i) {
            const auto &entry = boot_info.mmap[i];

            if (entry.base + entry.length > max_phys_addr)
                max_phys_addr = entry.base + entry.length;

            if (entry.Available()) {
                ASSERT(s_memory_count < MAX_MEMBLOCK_REGIONS,
                       "S_MEMORY_COUNT EXCEEDED MAX_MEMBLOCK_REGIONS");

                s_memory_regions[s_memory_count++]  = {entry.base, entry.length};
                s_total_free                       += entry.length;
            } else {
                MemblockReserve(entry.base, entry.length);
            }
        }

        if (max_phys_addr > 0)
            mm_highest_physical_page = (max_phys_addr - 1) >> 12;

        QWORD k_start = reinterpret_cast<QWORD>(__kernel_phys_start);
        QWORD k_end   = reinterpret_cast<QWORD>(__kernel_phys_end);
        MemblockReserve(k_start, k_end - k_start);

        InsertionSortRegions(s_memory_regions, s_memory_count);
        InsertionSortRegions(s_reserved_regions, s_reserved_count);

        MergeReservedRegions();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAlloc                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate a new chunk inside the memblock region                    *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAlloc(QWORD size, QWORD alignment) noexcept
    {
        if (size == 0 || alignment == 0) UNLIKELY
            return 0;

        ASSERT(tklib::math::is_power_of_two(alignment),
               "MemblockAlloc: alignment must be a power of two");

        for (SIZE_T i = 0; i < s_memory_count; ++i) {
            const QWORD region_start = s_memory_regions[i].base;
            const QWORD region_end   = region_start + s_memory_regions[i].size;

            QWORD candidate = tklib::math::align_up(region_start, alignment);

            while (candidate + size <= region_end) {
                BOOL overlapped = false;

                for (SIZE_T j = 0; j < s_reserved_count; ++j) {
                    const QWORD res_start = s_reserved_regions[j].base;
                    const QWORD res_end   = res_start + s_reserved_regions[j].size;

                    if (res_start >= candidate + size)
                        break;

                    if (candidate < res_end && (candidate + size) > res_start) {
                        overlapped = true;
                        candidate  = tklib::math::align_up(res_end, alignment);
                        break;
                    }
                }

                if (!overlapped) {
                    CarveFreeRegion(candidate, size);
                    MemblockReserve(candidate, size);
                    return candidate;
                }
            }
        }

        return 0;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MemblockFree                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Free / unreserve a previously allocated chunk inside memblock       *
     ********************************************************************************/
    VOID MemblockFree(QWORD base, QWORD size) noexcept
    {
        if (size == 0)
            return;

        BOOL carved = CarveReservedRegion(base, size);

        ASSERT(carved, "MemblockFree: Target region was not found in the reserved array!");
        ASSERT(s_memory_count < MAX_MEMBLOCK_REGIONS,
               "Memory region count exceeds MAX_MEMBLOCK_REGIONS during free");

        s_memory_regions[s_memory_count++]  = {base, size};
        s_total_free                       += size;

        InsertionSortRegions(s_memory_regions, s_memory_count);
        MergeMemoryRegions();
        InsertionSortRegions(s_reserved_regions, s_reserved_count);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReserve                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reserve a region inside the memblock                               *
     ********************************************************************************/
    VOID MemblockReserve(QWORD base, QWORD size) noexcept
    {
        if (size == 0)
            return;

        ASSERT(!tklib::math::add_would_overflow(base, size),
               "MemblockReserve: base + size overflows QWORD");
        ASSERT(s_reserved_count < MAX_MEMBLOCK_REGIONS,
               "Reserved region count exceeds MAX_MEMBLOCK_REGIONS");

        s_reserved_regions[s_reserved_count++]  = {base, size};
        s_total_reserved                       += size;

        InsertionSortRegions(s_reserved_regions, s_reserved_count);
        MergeReservedRegions();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsReserved                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if any byte in [base, base + size) is reserved.       *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsReserved(QWORD base, QWORD size) noexcept
    {
        if (size == 0)
            return false;

        const QWORD end = base + size;

        for (SIZE_T i = 0; i < s_reserved_count; ++i) {
            const QWORD res_start = s_reserved_regions[i].base;
            const QWORD res_end   = res_start + s_reserved_regions[i].size;

            if (res_start >= end)
                break;

            if (base < res_end && end > res_start)
                return true;
        }

        return false;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsPageFree                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a specific physical PFN is available for the free lists  *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsPageFree(PFN_NUM pfn) noexcept
    {
        QWORD byte_addr = static_cast<QWORD>(pfn) << 12;

        if (MemblockIsReserved(byte_addr, 4096))
            return false;

        for (SIZE_T i = 0; i < s_memory_count; ++i) {
            QWORD start = s_memory_regions[i].base;
            QWORD end   = start + s_memory_regions[i].size;

            if (byte_addr >= start && (byte_addr + 4096) <= end)
                return true;
        }

        return false;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockTotalFree                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total free bytes remaining in the memory pool.             *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockTotalFree() noexcept
    {
        return s_total_free;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockTotalReserved                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total reserved bytes across all reserved regions.          *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockTotalReserved() noexcept
    {
        return s_total_reserved;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockGetRegionCount                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current region count.                                      *
     ********************************************************************************/
    NO_DISCARD SIZE_T MemblockGetRegionCount() noexcept
    {
        return s_memory_count;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockGetRegion                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get a region at the passed in index.                               *
     ********************************************************************************/
    NO_DISCARD MemoryRegion MemblockGetRegion(SIZE_T index) noexcept
    {
        ASSERT(index < s_memory_count, "INDEX IN MemblockGetRegion EXCEEDS MEMORY COUNT.");
        return s_memory_regions[index];
    }

} // namespace trunk::mem