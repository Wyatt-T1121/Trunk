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
 *  MODULE  : Memblock                                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Early boot-stage allocator, PFN is not available at this time      *
 ********************************************************************************/
#include <cbk/mm/memblock.h>

namespace cbk::mem
{
    BOOT_ALLOCATION initial_memory_pool[INITIAL_POOL_SIZE]   = {};
    BOOT_ALLOCATION initial_reserved_pool[INITIAL_POOL_SIZE] = {};

    PBOOT_ALLOCATION boot_blocks = nullptr;
    SIZE_T boot_blocks_count     = 0;

    Memblock g_Memblock{.bottom_up    = FALSE,
                        .allow_resize = FALSE,
                        .curr_limit   = 0,
                        .memory       = {.cnt        = 0,
                                         .max        = INITIAL_POOL_SIZE,
                                         .total_size = 0,
                                         .regions    = initial_memory_pool,
                                         .name       = "memory"},
                        .reserved     = {.cnt        = 0,
                                         .max        = INITIAL_POOL_SIZE,
                                         .total_size = 0,
                                         .regions    = initial_reserved_pool,
                                         .name       = "reserved"}};

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : FindMaxAddr                                                        *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Translates a memory size limit into a physical address             *
         ********************************************************************************/
        NO_DISCARD QWORD FindMaxAddr(QWORD limit) noexcept
        {
            QWORD max_addr       = PHYS_ADDR_MAX;
            PBOOT_ALLOCATION reg = nullptr;

            ForEachMemoryRegion(i, &g_Memblock.memory, reg)
            {
                if (limit <= reg->size) {
                    max_addr = reg->phys_addr + limit;
                    break;
                }

                limit -= reg->size;
            }
            return max_addr;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MemblockRemoveRegion                                                *
         * DATE    : 2026                                                                *
         * PURPOSE : Erase an array element and shift the rest left                      *
         ********************************************************************************/
        VOID MemblockRemoveRegion(MemblockType *type, SIZE_T idx) noexcept
        {
            if (idx >= type->cnt)
                return;

            type->total_size -= type->regions[idx].size;

            for (SIZE_T j = idx; j < type->cnt - 1; j++)
                type->regions[j] = type->regions[j + 1];

            type->cnt--;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MemblockTypeRemoveRange                                            *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Cut, slice, or delete tracking blocks                              *
         ********************************************************************************/
        NO_DISCARD BOOL MemblockTypeRemoveRange(MemblockType *type, QWORD base, QWORD size) noexcept
        {
            if (!size || !type->cnt)
                return FALSE;

            QWORD end = base + size;

            for (SIZE_T i = 0; i < type->cnt;) {

                QWORD r_base = type->regions[i].phys_addr;
                QWORD r_size = type->regions[i].size;
                QWORD r_end  = r_base + r_size;

                if (r_base >= end || r_end <= base) {
                    i++;
                    continue;
                }

                if (r_base >= base && r_end <= end) {
                    MemblockRemoveRegion(type, i);
                    continue;
                }

                if ((r_base >= base && r_base < end) || (r_end > base && r_end <= end)) {

                    QWORD clip_start = (r_base > base) ? r_base : base;
                    QWORD clip_end   = (r_end < end) ? r_end : end;
                    QWORD clipped    = clip_end - clip_start;

                    if (r_base >= base && r_base < end)
                        type->regions[i].phys_addr = end;

                    type->regions[i].size -= clipped;
                    type->total_size      -= clipped;
                    i++;

                    continue;
                }

                if (r_base < base && r_end > end) {

                    if (type->cnt >= type->max)
                        if (!type->Grow())
                            return FALSE;

                    QWORD first_chunk_size  = base - r_base;
                    QWORD second_chunk_base = end;
                    QWORD second_chunk_size = r_end - end;

                    type->regions[i].size = first_chunk_size;

                    for (SIZE_T j = type->cnt; j > i + 1; j--)
                        type->regions[j] = type->regions[j - 1];

                    type->cnt++;
                    type->regions[i + 1].phys_addr  = second_chunk_base;
                    type->regions[i + 1].size       = second_chunk_size;
                    type->regions[i + 1].is_free    = type->regions[i].is_free;
                    type->total_size               -= size;

                    i += 2;
                }
            }

            return TRUE;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MemblockFindInRange                                                 *
         * DATE    : 2026                                                                *
         * PURPOSE : Scans available chunks, bypassing reserved blocks                   *
         ********************************************************************************/
        NO_DISCARD QWORD MemblockFindInRange(QWORD size, QWORD align, QWORD min_addr,
                                             QWORD max_addr, BOOL bottom_up) noexcept
        {
            if (!size || g_Memblock.memory.cnt == 0)
                return 0;

            QWORD limit_min = min_addr;
            QWORD limit_max = (max_addr == 0) ? limits::QWORD_max : max_addr;

            SIZE_T start_idx = bottom_up ? 0 : g_Memblock.memory.cnt - 1;
            SIZE_T end_idx   = bottom_up ? g_Memblock.memory.cnt : static_cast<SIZE_T>(-1);
            SSIZE_T step     = bottom_up ? 1 : -1;

            for (SIZE_T i = start_idx; i != end_idx; i += step) {

                PBOOT_ALLOCATION mem_reg = &g_Memblock.memory.regions[i];

                QWORD reg_start = mem_reg->phys_addr;
                QWORD reg_end   = reg_start + mem_reg->size;

                QWORD scan_start = tklib::math::max(reg_start, limit_min);
                QWORD scan_end   = tklib::math::min(reg_end, limit_max);

                if (scan_start >= scan_end || (scan_end - scan_start) < size)
                    continue;

                if (bottom_up) {

                    QWORD candidate = tklib::math::align_up(scan_start, align);

                    while (candidate + size <= scan_end) {
                        if (!g_Memblock.reserved.Intersects(candidate, size))
                            return candidate;
                        candidate = tklib::math::align_up(candidate + align, align);
                    }

                } else {
                    QWORD candidate = tklib::math::align_down(scan_end - size, align);
                    while (candidate >= scan_start) {

                        if (!g_Memblock.reserved.Intersects(candidate, size))
                            return candidate;
                        if (candidate < align)
                            break;

                        candidate = tklib::math::align_down(candidate - align, align);
                    }
                }
            }

            return 0;
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockInitialize                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the memory block allocator                              *
     ********************************************************************************/
    VOID MemblockInitialize(const boot::BootInfo &info) noexcept
    {
        g_Memblock.memory.cnt          = 0;
        g_Memblock.memory.total_size   = 0;
        g_Memblock.reserved.cnt        = 0;
        g_Memblock.reserved.total_size = 0;
        g_Memblock.curr_limit          = PHYS_ADDR_MAX;
        g_Memblock.allow_resize        = TRUE;

        for (SIZE_T i = 0; i < info.mmap_count; i++) {
            const auto &region = info.mmap[i];

            if (region.length == 0)
                continue;
            if (region.Available())
                MemblockAdd(region.base, region.length);
            else
                MemblockReserve(region.base, region.length);
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockSetCurrentLimit                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Set limit on highest physical address allowed to be given out      *
     ********************************************************************************/
    VOID MemblockSetCurrentLimit(QWORD limit) noexcept
    {
        g_Memblock.curr_limit = limit;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockGetCurrentLimit                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current limit set by the function above                    *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockGetCurrentLimit() noexcept
    {
        return g_Memblock.curr_limit;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllowResize                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allows tracking arrays to expand if they run out of slots          *
     ********************************************************************************/
    VOID MemblockAllowResize() noexcept
    {
        g_Memblock.allow_resize = TRUE;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockSetBottomUp                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flips the search direction for the allocator                       *
     *            If enabled, LOWEST-TO-HIGHEST, if disabled, HIGHEST-TO-LOWEST      *
     ********************************************************************************/
    VOID MemblockSetBottomUp(BOOL enable) noexcept
    {
        g_Memblock.bottom_up = enable;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockBottomUp                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if set to bottom-up allocator                               *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockBottomUp() noexcept
    {
        return g_Memblock.bottom_up;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockEnforceMemoryLimit                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Chops off any memory regions that exist above 'limit'              *
     ********************************************************************************/
    VOID MemblockEnforceMemoryLimit(QWORD limit) noexcept
    {
        if (!limit)
            return;

        QWORD max_addr = FindMaxAddr(limit);
        if (max_addr == PHYS_ADDR_MAX)
            return;

        QWORD remove_size = PHYS_ADDR_MAX - max_addr;

        ASSERT(MemblockTypeRemoveRange(&g_Memblock.memory, max_addr, remove_size),
               "MemblockEnforceMemoryLimit: Failed to TypeRemoveRange: g_Memblock.memory");
        ASSERT(MemblockTypeRemoveRange(&g_Memblock.reserved, max_addr, remove_size),
               "MemblockEnforceMemoryLimit: Failed to TypeRemoveRange: g_Memblock.reserved");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockCapMemoryRange                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clips everything outside of the window away                        *
     ********************************************************************************/
    VOID MemblockCapMemoryRange(QWORD base, QWORD size) noexcept
    {
        if (!size || !g_Memblock.memory.total_size)
            return;

        if (base > 0) {
            ASSERT(MemblockTypeRemoveRange(&g_Memblock.memory, 0, base),
                   "MemblockCapMemoryRange: Failed to cap memory to 0, base");
            ASSERT(MemblockTypeRemoveRange(&g_Memblock.reserved, 0, base),
                   "MemblockCapMemoryRange: Failed to cap reserved to 0, base");
        }

        QWORD window_end = base + size;
        if (window_end < PHYS_ADDR_MAX) {
            QWORD remove_size = PHYS_ADDR_MAX - window_end;
            ASSERT(MemblockTypeRemoveRange(&g_Memblock.memory, window_end, remove_size),
                   "MemblockCapMemoryRange: window_end, remove_size, on memory failed");
            ASSERT(MemblockTypeRemoveRange(&g_Memblock.reserved, window_end, remove_size),
                   "MemblockCapMemoryRange: window_end, remove_size, on reserved failed");
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockDiscard                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Untracks and releases the temporary arrays                         *
     ********************************************************************************/
    VOID MemblockDiscard() noexcept
    {
        QWORD size;
        PVOID addr;

        if (g_Memblock.reserved.regions != initial_reserved_pool) {
            addr = g_Memblock.reserved.regions;
            size = PAGE_ALIGN(sizeof(BOOT_ALLOCATION) * g_Memblock.reserved.max);
            MemblockFree(addr, size);
        }

        if (g_Memblock.memory.regions != initial_memory_pool) {
            addr = g_Memblock.memory.regions;
            size = PAGE_ALIGN(sizeof(BOOT_ALLOCATION) * g_Memblock.memory.max);
            MemblockFree(addr, size);
        }

        g_Memblock.memory.cnt        = 0;
        g_Memblock.memory.total_size = 0;
        g_Memblock.memory.regions    = nullptr;

        g_Memblock.reserved.cnt        = 0;
        g_Memblock.reserved.total_size = 0;
        g_Memblock.reserved.regions    = nullptr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAdd                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Insert a raw chunk of RAM into tracking pool                       *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockAdd(QWORD base, QWORD size) noexcept
    {
        if (!size)
            return FALSE;
        return g_Memblock.memory.Add(base, size, true);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockRemove                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Removes a raw chunk of RAM from the tracking pool                  *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockRemove(QWORD base, QWORD size) noexcept
    {
        if (!size)
            return FALSE;
        return MemblockTypeRemoveRange(&g_Memblock.memory, base, size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReserve                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Marks a chunk of added RAM as 'in-use'                             *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockReserve(QWORD base, QWORD size) noexcept
    {
        if (!size)
            return FALSE;
        return g_Memblock.reserved.Add(base, size, FALSE);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReserveKern                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Same as MemblockReserve, but specialized for the kernel regions    *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockReserveKern(QWORD base, QWORD size) noexcept
    {
        if (!size)
            return FALSE;
        return MemblockReserve(base, size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockPhysFree                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Un-reserves a chunk of RAM and adds to 'free' pool                 *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockPhysFree(QWORD base, QWORD size) noexcept
    {
        if (!size)
            return FALSE;
        return MemblockTypeRemoveRange(&g_Memblock.reserved, base, size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockTrimMemory                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clips off the tiny unaligned edges                                 *
     ********************************************************************************/
    VOID MemblockTrimMemory(QWORD align)
    {
        if (align <= 1)
            return;

        QWORD start, end, orig_start, orig_end;

        PBOOT_ALLOCATION r = nullptr;

        ForEachMemoryRegion(i, &g_Memblock.memory, r)
        {
            orig_start = r->phys_addr;
            orig_end   = r->phys_addr + r->size;

            start = tklib::math::align_up(orig_start, align);
            end   = tklib::math::align_down(orig_end, align);

            if (start == orig_start && end == orig_end)
                continue;

            if (start < end) {
                r->phys_addr = start;
                r->size      = end - start;
            } else {
                MemblockRemoveRegion(&g_Memblock.memory, r - g_Memblock.memory.regions);
                r--;
            }
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocFrom                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Alloc a chunk of memory but block must start at or above min_addr  *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocFrom(QWORD size, QWORD align, QWORD min_addr) noexcept
    {
        QWORD addr = MemblockFindInRange(size, align, min_addr, 0, g_Memblock.bottom_up);
        if (addr && MemblockReserve(addr, size)) {
            tklib::memset(reinterpret_cast<PVOID>(addr), 0, size);
            return addr;
        }

        return 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAlloc                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds a free chunk of memory matching size and alignment and       *
     *                                              reserves it                      *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAlloc(QWORD size, QWORD align) noexcept
    {
        return MemblockAllocFrom(size, align, 0);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocRaw                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Same as MemblockAlloc but doesn't zero the memory                  *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocRaw(QWORD size, QWORD align) noexcept
    {
        QWORD addr = MemblockFindInRange(size, align, 0, 0, g_Memblock.bottom_up);
        if (addr && MemblockReserve(addr, size))
            return addr;
        return 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocLow                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Exact opposite of MemblockAllocFrom                                *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocLow(QWORD size, QWORD align) noexcept
    {
        QWORD addr = MemblockFindInRange(size, align, 0, 0, TRUE);
        if (addr && MemblockReserve(addr, size)) {
            tklib::memset(reinterpret_cast<PVOID>(addr), 0, size);
            return addr;
        }

        return 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocOrPanic                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Standard alloc but panics if it fails (used for core systems)      *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocOrPanic(QWORD size, QWORD align) noexcept
    {
        QWORD addr = MemblockAlloc(size, align);
        ASSERT(!addr, "MemblockAllocOrPanic: Failed to allocate addr");
        return addr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockFree                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unreserves the exact region in the tracking list                   *
     ********************************************************************************/
    VOID MemblockFree(PVOID ptr, SIZE_T size) noexcept
    {
        if (ptr)
            MemblockPhysFree(KvaddrToPaddr(reinterpret_cast<QWORD>(ptr)), size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsMemory                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if the address is inside a valid RAM block                   *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsMemory(QWORD addr) noexcept
    {
        return g_Memblock.memory.Intersects(addr, 1);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsReserved                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a address sits inside a block that's currently not free   *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsReserved(QWORD addr) noexcept
    {
        return g_Memblock.reserved.Intersects(addr, 1);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsRegionMemory                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if the memory region rests within valid RAM sectors          *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsRegionMemory(QWORD base, QWORD size) noexcept
    {
        if (size == 0 || g_Memblock.memory.cnt == 0)
            return FALSE;

        QWORD end = base + size;

        for (SIZE_T i = 0; i < g_Memblock.memory.cnt; i++) {
            QWORD r_start = g_Memblock.memory.regions[i].phys_addr;
            QWORD r_end   = r_start + g_Memblock.memory.regions[i].size;

            if (base >= r_start && end <= r_end)
                return TRUE;
        }

        return FALSE;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsRegionReserved                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a piece of memory crosses over area marked as reserved   *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsRegionReserved(QWORD base, QWORD size) noexcept
    {
        return g_Memblock.reserved.Intersects(base, size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsMapAnywhere                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a region is free of special usage constraints like NOMAP  *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsMapAnywhere(QWORD base, QWORD size) noexcept
    {
        if (size == 0)
            return FALSE;
        return !g_Memblock.reserved.Intersects(base, size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockPhysMemSize                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Gets the total amount of RAM discovered on the system              *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockPhysMemSize() noexcept
    {
        return g_Memblock.memory.total_size;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReservedSize                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Calculate how many bytes are currently tied up in boot-stage       *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockReservedSize() noexcept
    {
        return g_Memblock.reserved.total_size;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockMemSizeLimit                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns current allocation limit cap (only if set)                 *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockMemSizeLimit() noexcept
    {
        return g_Memblock.curr_limit;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockStartOfDRam                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Locate the lowest address of RAM                                   *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockStartOfDRam() noexcept
    {
        if (g_Memblock.memory.cnt == 0)
            return 0;
        return g_Memblock.memory.regions[0].phys_addr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockEndOfDRam                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Locate the highest address of RAM                                  *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockEndOfDRam() noexcept
    {
        if (g_Memblock.memory.cnt == 0)
            return 0;
        SIZE_T idx = g_Memblock.memory.cnt - 1;
        return (g_Memblock.memory.regions[idx].phys_addr + g_Memblock.memory.regions[idx].size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockFreeAll                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Registers free blocks into our physical page manager               *
     ********************************************************************************/
    VOID MemblockFreeAll() noexcept
    {
        if (g_Memblock.memory.cnt == 0)
            return;

        InitializeListHead(&mm_page_location_list_head[(BYTE)MM_PFN_STATE::FREE_PAGE_LIST]);
        InitializeListHead(&mm_page_location_list_head[(BYTE)MM_PFN_STATE::ZEROED_PAGE_LIST]);

        QWORD end_of_ram         = MemblockEndOfDRam();
        mm_highest_physical_page = end_of_ram / PAGE_SIZE;

        for (SIZE_T i = 0; i < g_Memblock.memory.cnt; i++) {
            QWORD mem_start = g_Memblock.memory.regions[i].phys_addr;
            QWORD mem_end   = mem_start + g_Memblock.memory.regions[i].size;

            QWORD current_addr = mem_start;

            while (current_addr < mem_end) {

                if (g_Memblock.reserved.Intersects(current_addr, 1)) {
                    for (SIZE_T r = 0; r < g_Memblock.reserved.cnt; r++) {
                        QWORD res_start = g_Memblock.reserved.regions[r].phys_addr;
                        QWORD res_end   = res_start + g_Memblock.reserved.regions[r].size;

                        if (current_addr >= res_start && current_addr < res_end) {
                            PFN_NUM start_pfn = res_start / PAGE_SIZE;
                            PFN_NUM page_cnt  = (res_end - res_start) / PAGE_SIZE;

                            for (PFN_NUM pfn = start_pfn; pfn < start_pfn + page_cnt; pfn++) {
                                mm_pfn_database[pfn].page_location = MM_PFN_STATE::ACTIVE_AND_VALID;
                                mm_pfn_database[pfn].reference_count = 1;
                            }

                            current_addr = res_end;
                            break;
                        }
                    }

                    continue;
                }

                QWORD free_start = current_addr;
                QWORD free_end   = mem_end;

                for (SIZE_T r = 0; r < g_Memblock.reserved.cnt; r++) {
                    QWORD res_start = g_Memblock.reserved.regions[r].phys_addr;
                    if (res_start > free_start && res_start < free_end)
                        free_end = res_start;
                }

                PFN_NUM start_pfn = free_start / PAGE_SIZE;
                PFN_NUM page_cnt  = (free_end - free_start) / PAGE_SIZE;

                for (PFN_NUM pfn = start_pfn; pfn < start_pfn + page_cnt; pfn++) {
                    mm_pfn_database[pfn].page_location   = MM_PFN_STATE::FREE_PAGE_LIST;
                    mm_pfn_database[pfn].reference_count = 0;

                    InsertTailList(&mm_page_location_list_head[(BYTE)MM_PFN_STATE::FREE_PAGE_LIST],
                                   &mm_pfn_database[pfn].list_entry);

                    mm_available_pages++;
                    mm_resident_available_pages++;
                }

                current_addr = free_end;
            }
        }

        mm_resident_available_at_init = mm_resident_available_pages;
        g_Memblock.allow_resize       = FALSE;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockFreeLate                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Delivers patch of memory to page allocator                         *
     ********************************************************************************/
    VOID MemblockFreeLate(QWORD base, QWORD size) noexcept
    {
        if (size == 0)
            return;

        QWORD aligned_base = PageAlignUp(base);
        QWORD aligned_end  = PageAlignDown(base + size);

        if (aligned_end <= aligned_base)
            return;

        PFN_NUM start_pfn = aligned_base / PAGE_SIZE;
        PFN_NUM page_cnt  = (aligned_end - aligned_base) / PAGE_SIZE;

        for (PFN_NUM pfn = start_pfn; pfn < start_pfn + page_cnt; pfn++) {
            if (mm_pfn_database[pfn].page_location == MM_PFN_STATE::FREE_PAGE_LIST ||
                mm_pfn_database[pfn].page_location == MM_PFN_STATE::ZEROED_PAGE_LIST) {
                continue;
            }

            mm_pfn_database[pfn].page_location   = MM_PFN_STATE::FREE_PAGE_LIST;
            mm_pfn_database[pfn].reference_count = 0;
            mm_pfn_database[pfn].rmap_list_head  = nullptr;

            InsertTailList(&mm_page_location_list_head[(BYTE)MM_PFN_STATE::FREE_PAGE_LIST],
                           &mm_pfn_database[pfn].list_entry);

            mm_available_pages++;
            mm_resident_available_pages++;
        }
    }
} // namespace cbk::mem