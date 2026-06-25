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
#pragma once

#include <assert.h>
#include <types.h>

#include <cbk/mm/freelist.h>
#include <cbk/mm/list.h>
#include <cbk/mm/mmarch.h>
#include <cbk/mm/pfn.h>

#include <boot/trldr/mb2/boot.h>

#include <tklib/math.h>
#include <tklib/string.h>

// Memblock is used when we're in early boot-stage, and need to allocate some space to startup the
// advanced memory management system.
// For example, the PFN database needs an allocated spot in memory for it's structures
// After the system is allocated and setup, memblock is discarded and never used again
// Think of it like the 'initramfs' of memory management

// DOES NOT SUPPORT TABLE:
//        Hotplugging RAM
//        NUMA CPU node ID
//        Mirrored memory
//        Nomap for ARM

// Hotplugging RAM - support in the far future
// NUMA CPU node ID - support after scheduling
// Mirrored memory - support after scheduling
// Nomap from ARM - don't support, x86 only os

#define ForEachMemoryRegion(i, type, reg)                                                          \
    for (SIZE_T i = 0; i < (type)->cnt && ((reg) = &(type)->regions[i], true); i++)

namespace cbk::mem
{
    INLINE_CONST BYTE INITIAL_POOL_SIZE = 16;
    struct BootAllocation
    {
        QWORD phys_addr;
        QWORD size;
        BOOL is_free;
    };

    using BOOT_ALLOCATION  = BootAllocation;
    using PBOOT_ALLOCATION = BootAllocation *;

    struct Memblock;
    extern Memblock g_Memblock;

    struct MemblockType
    {
        SIZE_T cnt;
        SIZE_T max;
        QWORD total_size;
        PBOOT_ALLOCATION regions;
        PCSTR name;

        NO_DISCARD BOOL Add(QWORD phys_addr, QWORD size, BOOL is_free) noexcept
        {
            if (size == 0)
                return FALSE;

            QWORD new_end = phys_addr + size;

            for (SIZE_T i = 0; i < cnt; i++) {
                if (regions[i].is_free != is_free)
                    continue;

                QWORD r_start = regions[i].phys_addr;
                QWORD r_end   = r_start + regions[i].size;

                if (phys_addr <= r_end && new_end >= r_start) {
                    QWORD merged_start = (phys_addr < r_start) ? phys_addr : r_start;
                    QWORD merged_end   = (new_end > r_end) ? new_end : r_end;

                    total_size           -= regions[i].size;
                    regions[i].phys_addr  = merged_start;
                    regions[i].size       = merged_end - merged_start;
                    total_size           += regions[i].size;
                    return TRUE;
                }
            }

            if (cnt >= max)
                if (!Grow())
                    return FALSE;

            SIZE_T insert_idx = cnt;
            for (SIZE_T i = 0; i < cnt; i++) {
                if (phys_addr < regions[i].phys_addr) {
                    insert_idx = i;
                    break;
                }
            }

            for (SIZE_T i = cnt; i > insert_idx; i--)
                regions[i] = regions[i - 1];

            regions[insert_idx].phys_addr = phys_addr;
            regions[insert_idx].size      = size;
            regions[insert_idx].is_free   = is_free;

            total_size += size;
            cnt++;
            return TRUE;
        }

        NO_DISCARD BOOL Grow() noexcept;

        NO_DISCARD BOOL Intersects(QWORD phys_addr, QWORD size) noexcept
        {
            if (size == 0 || cnt == 0)
                return FALSE;

            QWORD end = phys_addr + size;

            for (SIZE_T i = 0; i < cnt; i++) {
                QWORD r_start = regions[i].phys_addr;
                QWORD r_end   = r_start + regions[i].size;

                if (phys_addr < r_end && end > r_start)
                    return TRUE;
            }

            return FALSE;
        }
    };

    struct Memblock
    {
        BOOL bottom_up;
        BOOL allow_resize;
        QWORD curr_limit;

        MemblockType memory;
        MemblockType reserved;
    };

    extern BOOT_ALLOCATION initial_memory_pool[];
    extern BootAllocation initial_reserved_pool[];

    extern PBOOT_ALLOCATION boot_blocks;
    extern SIZE_T boot_blocks_count;

    NO_DISCARD BOOL MemblockType::Grow() noexcept
    {

        if (!g_Memblock.allow_resize)
            return FALSE;

        SIZE_T new_max        = max * 2;
        SIZE_T new_size_bytes = new_max * sizeof(BOOT_ALLOCATION);
        QWORD new_array_phys  = 0;

        for (SIZE_T i = 0; i < g_Memblock.memory.cnt; i++) {
            if (g_Memblock.memory.regions[i].is_free &&
                g_Memblock.memory.regions[i].size >= new_size_bytes) {

                new_array_phys = g_Memblock.memory.regions[i].phys_addr;

                g_Memblock.memory.regions[i].phys_addr += new_size_bytes;
                g_Memblock.memory.regions[i].size      -= new_size_bytes;
                g_Memblock.memory.total_size           -= new_size_bytes;

                break;
            }
        }

        if (new_array_phys == 0)
            return FALSE;

        PBOOT_ALLOCATION new_regions =
            reinterpret_cast<PBOOT_ALLOCATION>(PaddrToKvaddr(new_array_phys));

        if (new_regions == nullptr)
            return FALSE;

        for (SIZE_T i = 0; i < cnt; i++)
            new_regions[i] = regions[i];

        if (g_Memblock.reserved.cnt < g_Memblock.reserved.max) {

            SIZE_T r_cnt = g_Memblock.reserved.cnt;

            g_Memblock.reserved.regions[r_cnt].phys_addr  = new_array_phys;
            g_Memblock.reserved.regions[r_cnt].size       = new_size_bytes;
            g_Memblock.reserved.regions[r_cnt].is_free    = FALSE;
            g_Memblock.reserved.total_size               += new_size_bytes;
            g_Memblock.reserved.cnt++;

        } else
            return FALSE;

        regions = new_regions;
        max     = new_max;
        return TRUE;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockInitialize                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the memory block allocator                              *
     ********************************************************************************/
    VOID MemblockInitialize(const boot::BootInfo &info) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockSetCurrentLimit                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Set limit on highest physical address allowed to be given out      *
     ********************************************************************************/
    VOID MemblockSetCurrentLimit(QWORD limit) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockGetCurrentLimit                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current limit set by the function above                    *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockGetCurrentLimit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllowResize                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allows tracking arrays to expand if they run out of slots          *
     ********************************************************************************/
    VOID MemblockAllowResize() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockSetBottomUp                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flips the search direction for the allocator                       *
     *            If enabled, LOWEST-TO-HIGHEST, if disabled, HIGHEST-TO-LOWEST      *
     ********************************************************************************/
    VOID MemblockSetBottomUp(BOOL enable) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockBottomUp                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if set to bottom-up allocator                               *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockBottomUp() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockEnforceMemoryLimit                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Chops off any memory regions that exist above 'limit'              *
     ********************************************************************************/
    VOID MemblockEnforceMemoryLimit(QWORD limit) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockCapMemoryRange                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clips everything outside of the window away                        *
     ********************************************************************************/
    VOID MemblockCapMemoryRange(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockDiscard                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Untracks and releases the temporary arrays                         *
     ********************************************************************************/
    VOID MemblockDiscard() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAdd                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Insert a raw chunk of RAM into tracking pool                       *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockAdd(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockRemove                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Removes a raw chunk of RAM from the tracking pool                  *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockRemove(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReserve                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Marks a chunk of added RAM as 'in-use'                             *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockReserve(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReserveKern                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Same as MemblockReserve, but specialized for the kernel regions    *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockReserveKern(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockPhysFree                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Un-reserves a chunk of RAM and adds to 'free' pool                 *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockPhysFree(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockTrimMemory                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clips off the tiny unaligned edges                                 *
     ********************************************************************************/
    VOID MemblockTrimMemory(QWORD align);

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAlloc                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds a free chunk of memory matching size and alignment and       *
     *                                              reserves it                      *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAlloc(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocRaw                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Same as MemblockAlloc but doesn't zero the memory                  *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocRaw(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocFrom                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Alloc a chunk of memory but block must start at or above min_addr  *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocFrom(QWORD size, QWORD align, QWORD min_addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocLow                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Exact opposite of MemblockAllocFrom                                *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocLow(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAllocOrPanic                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Standard alloc but panics if it fails (used for core systems)      *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAllocOrPanic(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockFree                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unreserves the exact region in the tracking list                   *
     ********************************************************************************/
    VOID MemblockFree(PVOID ptr, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsMemory                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if the address is inside a valid RAM block                   *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsMemory(QWORD addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsReserved                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a address sits inside a block that's currently not free   *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsReserved(QWORD addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsRegionMemory                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if the memory region rests within valid RAM sectors          *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsRegionMemory(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsRegionReserved                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a piece of memory crosses over area marked as reserved   *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsRegionReserved(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsMapAnywhere                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a region is free of special usage constraints like NOMAP  *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsMapAnywhere(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockPhysMemSize                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Gets the total amount of RAM discovered on the system              *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockPhysMemSize() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReservedSize                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Calculate how many bytes are currently tied up in boot-stage       *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockReservedSize() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockMemSizeLimit                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns current allocation limit cap (only if set)                 *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockMemSizeLimit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockStartOfDRam                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Locate the lowest address of RAM                                   *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockStartOfDRam() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockEndOfDRam                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Locate the highest address of RAM                                  *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockEndOfDRam() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockFreeAll                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Registers free blocks into our physical page manager               *
     ********************************************************************************/
    VOID MemblockFreeAll() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockFreeLate                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Delivers patch of memory to page allocator                         *
     ********************************************************************************/
    VOID MemblockFreeLate(QWORD base, QWORD size) noexcept;
} // namespace cbk::mem