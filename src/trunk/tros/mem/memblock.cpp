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
#include <trunk/tros/mem/memblock.h>

#include <assert.h>
#include <kmlayout.h>

#include <tklib/math.h>

namespace trunk::mem
{
    static MemoryRegion s_memory_regions[MAX_MEMBLOCK_REGIONS];
    static MemoryRegion s_reserved_regions[MAX_MEMBLOCK_REGIONS];

    static usize s_memory_count   = 0;
    static usize s_reserved_count = 0;

    static u64 s_total_free     = 0;
    static u64 s_total_reserved = 0;

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : insertion_sort_regions                                             *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Sort regions by base address                                       *
         ********************************************************************************/
        void insertion_sort_regions(MemoryRegion *regions, usize count) noexcept
        {
            for (usize i = 1; i < count; ++i) {
                MemoryRegion key = regions[i];

                isize j = static_cast<isize>(i) - 1;

                while (j >= 0 && regions[j].base > key.base) {
                    regions[j + 1] = regions[j];
                    --j;
                }

                regions[j + 1] = key;
            }
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : merge_reserved_regions                                             *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Merge overlapping reserved regions into one entry                  *
         ********************************************************************************/
        void merge_reserved_regions() noexcept
        {
            if (s_reserved_count < 2)
                return;

            usize write = 0;
            for (usize read = 1; read < s_reserved_count; ++read) {
                const u64 cur_end = s_reserved_regions[write].base + s_reserved_regions[write].size;
                const u64 next_start = s_reserved_regions[read].base;
                const u64 next_end   = next_start + s_reserved_regions[read].size;

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
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : carve_free_region                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Remove [base, base + size) from the free list.                     *
         ********************************************************************************/
        bool carve_free_region(u64 base, u64 size) noexcept
        {
            const u64 end = base + size;

            for (usize i = 0; i < s_memory_count; ++i) {
                const u64 region_start = s_memory_regions[i].base;
                const u64 region_end   = region_start + s_memory_regions[i].size;

                if (base < region_start || end > region_end)
                    continue;

                const u64 left_size  = base - region_start;
                const u64 right_size = region_end - end;

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

    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_init                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialization function for memblock                               *
     ********************************************************************************/
    void memblock_init(const boot::BootInfo &boot_info) noexcept
    {
        s_memory_count   = 0;
        s_reserved_count = 0;

        s_total_free     = 0;
        s_total_reserved = 0;

        for (usize i = 0; i < boot_info.mmap_count; ++i) {
            const auto &entry = boot_info.mmap[i];

            if (entry.available()) {
                ASSERT(s_memory_count < MAX_MEMBLOCK_REGIONS,
                       "S_MEMORY_COUNT EXCEEDED MAX_MEMBLOCK_REGIONS");

                s_memory_regions[s_memory_count++]  = {entry.base, entry.length};
                s_total_free                       += entry.length;
            } else {
                memblock_reserve(entry.base, entry.length);
            }
        }

        u64 k_start = reinterpret_cast<u64>(__kernel_phys_start);
        u64 k_end   = reinterpret_cast<u64>(__kernel_phys_end);
        memblock_reserve(k_start, k_end - k_start);

        insertion_sort_regions(s_memory_regions, s_memory_count);
        insertion_sort_regions(s_reserved_regions, s_reserved_count);

        merge_reserved_regions();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_alloc                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate a new chunk inside the memblock region                    *
     ********************************************************************************/
    NO_DISCARD u64 memblock_alloc(u64 size, u64 alignment) noexcept
    {
        if (size == 0 || alignment == 0) UNLIKELY {
            return 0;
        }

        ASSERT(tklib::math::is_power_of_two(alignment),
               "memblock_alloc: alignment must be a power of two");

        for (usize i = 0; i < s_memory_count; ++i) {
            const u64 region_start = s_memory_regions[i].base;
            const u64 region_end   = region_start + s_memory_regions[i].size;

            u64 candidate = tklib::math::align_up(region_start, alignment);

            while (candidate + size <= region_end) {
                bool overlapped = false;

                for (usize j = 0; j < s_reserved_count; ++j) {
                    const u64 res_start = s_reserved_regions[j].base;
                    const u64 res_end   = res_start + s_reserved_regions[j].size;

                    if (res_start >= candidate + size)
                        break;

                    if (candidate < res_end && (candidate + size) > res_start) {
                        overlapped = true;
                        candidate  = tklib::math::align_up(res_end, alignment);
                        break;
                    }
                }

                if (!overlapped) {
                    carve_free_region(candidate, size);
                    memblock_reserve(candidate, size);
                    return candidate;
                }
            }
        }

        return 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_reserve                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reserve a region inside the memblock                               *
     ********************************************************************************/
    void memblock_reserve(u64 base, u64 size) noexcept
    {
        if (size == 0)
            return;

        ASSERT(!tklib::math::add_would_overflow(base, size),
               "memblock_reserve: base + size overflows u64");
        ASSERT(s_reserved_count < MAX_MEMBLOCK_REGIONS,
               "Reserved region count exceeds MAX_MEMBLOCK_REGIONS");

        s_reserved_regions[s_reserved_count++]  = {base, size};
        s_total_reserved                       += size;

        insertion_sort_regions(s_reserved_regions, s_reserved_count);
        merge_reserved_regions();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_is_reserved                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if any byte in [base, base + size) is reserved.       *
     ********************************************************************************/
    NO_DISCARD bool memblock_is_reserved(u64 base, u64 size) noexcept
    {
        if (size == 0)
            return false;

        const u64 end = base + size;

        for (usize i = 0; i < s_reserved_count; ++i) {
            const u64 res_start = s_reserved_regions[i].base;
            const u64 res_end   = res_start + s_reserved_regions[i].size;

            if (res_start >= end)
                break;

            if (base < res_end && end > res_start)
                return true;
        }

        return false;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_total_free                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total free bytes remaining in the memory pool.             *
     ********************************************************************************/
    NO_DISCARD u64 memblock_total_free() noexcept
    {
        return s_total_free;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_total_reserved                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total reserved bytes across all reserved regions.          *
     ********************************************************************************/
    NO_DISCARD u64 memblock_total_reserved() noexcept
    {
        return s_total_reserved;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_get_region_count                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current region count.                                      *
     ********************************************************************************/
    NO_DISCARD usize memblock_get_region_count() noexcept
    {
        return s_memory_count;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_get_region                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get a region at the passed in index.                               *
     ********************************************************************************/
    NO_DISCARD MemoryRegion memblock_get_region(usize index) noexcept
    {
        ASSERT(index < s_memory_count, "INDEX IN MEMBLOCK_GET_REGION EXCEEDS MEMORY COUNT.");
        return s_memory_regions[index];
    }

} // namespace trunk::mem