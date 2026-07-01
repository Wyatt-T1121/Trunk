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

    struct Memblock;
    extern Memblock g_Memblock;

    struct BootAllocation
    {
        QWORD phys_addr;
        QWORD size;
        BOOL is_free;
    };

    struct MemblockType
    {
        SIZE_T cnt;
        SIZE_T max;
        QWORD total_size;
        PBOOT_ALLOCATION regions;
        PCSTR name;

        NO_DISCARD BOOL
        Add(QWORD phys_addr, QWORD size, BOOL is_free) noexcept
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

        NO_DISCARD BOOL
        Grow() noexcept;

        NO_DISCARD BOOL
        Intersects(QWORD phys_addr, QWORD size) noexcept
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
    extern BOOT_ALLOCATION initial_reserved_pool[];

    extern PBOOT_ALLOCATION boot_blocks;
    extern SIZE_T boot_blocks_count;

    NO_DISCARD INLINE BOOL
    MemblockType::Grow() noexcept
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

        regions = new_regions;
        max     = new_max;

        if (g_Memblock.reserved.cnt < g_Memblock.reserved.max) {
            SIZE_T r_cnt = g_Memblock.reserved.cnt;

            g_Memblock.reserved.regions[r_cnt].phys_addr  = new_array_phys;
            g_Memblock.reserved.regions[r_cnt].size       = new_size_bytes;
            g_Memblock.reserved.regions[r_cnt].is_free    = FALSE;
            g_Memblock.reserved.total_size               += new_size_bytes;
            g_Memblock.reserved.cnt++;
            return TRUE;
        }

        return FALSE;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockInitialize                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the memory block allocator                              *
     ********************************************************************************/
    VOID
    MmMemblockInitialize(const boot::BootInfo &info) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockSetCurrentLimit                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Set limit on highest physical address allowed to be given out      *
     ********************************************************************************/
    VOID
    MmMemblockSetCurrentLimit(QWORD limit) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockGetCurrentLimit                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current limit set by the function above                    *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockGetCurrentLimit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockAllowResize                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allows tracking arrays to expand if they run out of slots          *
     ********************************************************************************/
    VOID
    MmMemblockAllowResize() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockSetBottomUp                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flips the search direction for the allocator                       *
     *            If enabled, LOWEST-TO-HIGHEST, if disabled, HIGHEST-TO-LOWEST      *
     ********************************************************************************/
    VOID
    MmMemblockSetBottomUp(BOOL enable) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockIsBottomUp                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if set to bottom-up allocator                               *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockIsBottomUp() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockEnforceMemoryLimit                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Chops off any memory regions that exist above 'limit'              *
     ********************************************************************************/
    VOID
    MmMemblockEnforceMemoryLimit(QWORD limit) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockCapMemoryRange                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clips everything outside of the window away                        *
     ********************************************************************************/
    VOID
    MmMemblockCapMemoryRange(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockDiscard                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Untracks and releases the temporary arrays                         *
     ********************************************************************************/
    VOID
    MmMemblockDiscard() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockAdd                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Insert a raw chunk of RAM into tracking pool                       *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockAdd(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockRemove                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Removes a raw chunk of RAM from the tracking pool                  *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockRemove(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockReserve                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Marks a chunk of added RAM as 'in-use'                             *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockReserve(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockReserveKernelRegions                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Same as MmMemblockReserve, but specialized for the kernel regions  *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockReserveKernelRegions(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockPhysicalFree                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Un-reserves a chunk of RAM and adds to 'free' pool                 *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockPhysicalFree(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockTrimMemory                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clips off the tiny unaligned edges                                 *
     ********************************************************************************/
    VOID
    MmMemblockTrimMemory(QWORD align);

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockAllocate                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds a free chunk of memory matching size and alignment and       *
     *                                              reserves it                      *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockAllocate(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockAllocateRaw                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Same as MmMemblockAllocate but doesn't zero the memory             *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockAllocateRaw(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockAllocateFromAddress                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Alloc a chunk of memory but block must start at or above min_addr  *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockAllocateFromAddress(QWORD size, QWORD align, QWORD min_addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockAllocateLow                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Exact opposite of MmMemblockAllocateFromAddress                    *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockAllocateLow(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockAllocateOrCrashKernel                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Standard alloc but panics if it fails (used for core systems)      *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockAllocateOrCrashKernel(QWORD size, QWORD align) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockFree                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unreserves the exact region in the tracking list                   *
     ********************************************************************************/
    VOID
    MmMemblockFree(PVOID ptr, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockIsAddressValid                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if the address is inside a valid RAM block                   *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockIsAddressValid(QWORD addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockIsAddressReserved                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a address sits inside a block that's currently not free   *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockIsAddressReserved(QWORD addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockIsRegionMemory                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if the memory region rests within valid RAM sectors          *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockIsRegionMemory(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockIsRegionReserved                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a piece of memory crosses over area marked as reserved   *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockIsRegionReserved(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockIsRegionFreeForMap                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a region is free of special usage constraints like NOMAP  *
     ********************************************************************************/
    NO_DISCARD BOOL
    MmMemblockIsRegionFreeForMap(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockPhysicalMemorySize                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Gets the total amount of RAM discovered on the system              *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockPhysicalMemorySize() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockCurrentlyReservedSize                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Calculate how many bytes are currently tied up in boot-stage       *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockCurrentlyReservedSize() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockGetSizeLimit                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns current allocation limit cap (only if set)                 *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockGetSizeLimit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockGetStartOfDRam                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Locate the lowest address of RAM                                   *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockGetStartOfDRam() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockGetEndOfDRam                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Locate the highest address of RAM                                  *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmMemblockGetEndOfDRam() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockFreeAll                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Registers free blocks into our physical page manager               *
     ********************************************************************************/
    VOID
    MmMemblockFreeAll() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMemblockFreeLate                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Delivers patch of memory to page allocator                         *
     ********************************************************************************/
    VOID
    MmMemblockFreeLate(QWORD base, QWORD size) noexcept;
} // namespace cbk::mem