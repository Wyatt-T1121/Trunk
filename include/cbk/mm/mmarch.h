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
 *  MODULE  : Page definitions                                                   *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores definitions for page handling                               *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

#include <cbk/hal/io.h>

#include <kmlayout.h>

#include <cbk/mm/list.h>

namespace cbk::mem
{

    CONSTEXPR QWORD KERNEL_VMA   = 0xFFFFFFFF80000000ULL;
    CONSTEXPR QWORD PHYSMAP_BASE = 0xFFFF800000000000ULL;

    CONSTEXPR QWORD PHYS_ADDR_MAX        = 0xFFFFFFFFFFFFFFFFULL;
    CONSTEXPR QWORD PHYS_ADDR_48_BIT_MAX = 0x0000FFFFFFFFFFFFULL;
    CONSTEXPR QWORD PHYS_ADDR_52_BIT_MAX = 0x000FFFFFFFFFFFFFULL;
    CONSTEXPR QWORD PHYS_ADDR_16MB_MAX   = 0x01000000ULL;
    CONSTEXPR QWORD PHYS_ADDR_4GB_MAX    = 0x100000000ULL;

    CONSTEXPR QWORD KB = 1024ULL;
    CONSTEXPR QWORD MB = 1024ULL * KB;
    CONSTEXPR QWORD GB = 1024ULL * MB;

    CONSTEXPR QWORD PAGE_SIZE  = 4096;
    CONSTEXPR QWORD PAGE_MASK  = ~(PAGE_SIZE - 1);
    CONSTEXPR QWORD PAGE_SHIFT = 12;

    CONSTEXPR QWORD HUGE_PAGE_SIZE = 2 * MB;
    CONSTEXPR QWORD HUGE_MASK      = HUGE_PAGE_SIZE - 1;
    CONSTEXPR QWORD PAGE_HUGE      = QWORD{1} << 7;

    CONSTEXPR QWORD PAGE_WRITE_THROUGH = 1ULL << 3;
    CONSTEXPR QWORD PAGE_CACHE_DISABLE = 1ULL << 4;
    CONSTEXPR QWORD PAGE_PAT           = 1ULL << 7;

    CONSTEXPR QWORD PAGE_PRESENT  = (1ULL << 0);
    CONSTEXPR QWORD PAGE_WRITABLE = (1ULL << 1);
    CONSTEXPR QWORD PAGE_USER     = (1ULL << 2);
    CONSTEXPR QWORD PAGE_PWT      = (1ULL << 3);
    CONSTEXPR QWORD PAGE_PCD      = (1ULL << 4);
    CONSTEXPR QWORD PAGE_ACCESSED = (1ULL << 5);
    CONSTEXPR QWORD PAGE_DIRTY    = (1ULL << 6);
    CONSTEXPR QWORD PAGE_GLOBAL   = (1ULL << 8);
    CONSTEXPR QWORD PAGE_NX       = (1ULL << 63);

    CONSTEXPR ULONG PAGE_NOACCESS          = 0x00000001;
    CONSTEXPR ULONG PAGE_READONLY          = 0x00000002;
    CONSTEXPR ULONG PAGE_READWRITE         = 0x00000004;
    CONSTEXPR ULONG PAGE_WRITECOPY         = 0x00000008;
    CONSTEXPR ULONG PAGE_EXECUTE           = 0x00000010;
    CONSTEXPR ULONG PAGE_EXECUTE_READ      = 0x00000020;
    CONSTEXPR ULONG PAGE_EXECUTE_READWRITE = 0x00000040;
    CONSTEXPR ULONG PAGE_EXECUTE_WRITECOPY = 0x00000080;
    CONSTEXPR ULONG PAGE_GUARD             = 0x00000100;
    CONSTEXPR ULONG PAGE_NOCACHE           = 0x00000200;
    CONSTEXPR ULONG PAGE_WRITECOMBINE      = 0x00000400;

    CONSTEXPR ULONG VAD_STATE_COMMITTED = 0x02;

    CONSTEXPR QWORD PTE_AVAIL = 0xE00;
    CONSTEXPR QWORD PTE_USER  = PTE_AVAIL | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

    CONSTEXPR QWORD NO_OF_PT_ENTRIES = 512;

#define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE - 1) & PAGE_MASK)

    struct ArchAspace
    {
        QWORD *pml4_virt;
        QWORD pml4_phys;
        QWORD base;
        SIZE_T size;
    };

    struct MmRmapEntry
    {
        LIST_ENTRY list_entry;
        ArchAspace *space;
        PVOID virtual_address;
    };

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PPN                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Gets physical page number from address                              *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR ULONG_PTR PPN(ULONG_PTR la) noexcept
    {
        return la >> 12;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : VPN                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Gets virtual page number from address                               *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR ULONG_PTR VPN(ULONG_PTR la) noexcept
    {
        return PPN(la);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PML4X                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PML4 table index                                       *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR QWORD PML4X(ULONG_PTR la) noexcept
    {
        return (la >> 39) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDPTX                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PDP table index                                        *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR QWORD PDPTX(ULONG_PTR la) noexcept
    {
        return (la >> 30) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page directory index                                   *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR QWORD PDX(ULONG_PTR la) noexcept
    {
        return (la >> 21) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page table index                                       *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR QWORD PTX(ULONG_PTR la) noexcept
    {
        return (la >> 12) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGOFF                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the byte offset within a page                              *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR QWORD PGOFF(ULONG_PTR la) noexcept
    {
        return la & 0xFFF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGADDR                                                              *
     * DATE    : 2026                                                                *
     * PURPOSE : Constructs a virtual address from indexes                           *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR ULONG_PTR PGADDR(QWORD m, QWORD d, QWORD p, QWORD t, QWORD o) noexcept
    {
        return (m << 39) | (d << 30) | (p << 21) | (t << 12) | o;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTE_ADDR                                                            *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the physical address from an entry                         *
     ********************************************************************************/
    NO_DISCARD CONSTEXPR ULONG_PTR PTE_ADDR(ULONG_PTR pte) noexcept
    {
        return pte & 0x000FFFFFFFFFF000ULL;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PageAlignDown                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address down to page boundary                             *
     ********************************************************************************/
    NO_DISCARD static INLINE QWORD PageAlignDown(QWORD addr) noexcept
    {
        return addr & PAGE_MASK;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PageAlignUp                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address up to page boundary                               *
     ********************************************************************************/
    NO_DISCARD static INLINE QWORD PageAlignUp(QWORD addr) noexcept
    {
        return (addr + PAGE_SIZE - 1) & PAGE_MASK;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPageAligned                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns TRUE if addr is 4KB aligned                                *
     ********************************************************************************/
    NO_DISCARD static INLINE BOOL IsPageAligned(QWORD addr) noexcept
    {
        return (addr & (PAGE_SIZE - 1)) == 0;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PaddrToKvaddr                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a physical address to a virtual address                    *
     ********************************************************************************/
    NO_DISCARD static INLINE PVOID PaddrToKvaddr(QWORD paddr) noexcept
    {
        return reinterpret_cast<PVOID>(paddr + PHYSMAP_BASE);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : KvaddrToPaddr                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a virtual address to a physical adddress                   *
     ********************************************************************************/
    NO_DISCARD static INLINE QWORD KvaddrToPaddr(QWORD kvaddr) noexcept
    {
        if (kvaddr >= KERNEL_VMA) {
            QWORD k_phys_start = reinterpret_cast<QWORD>(__kernel_phys_start);
            return (kvaddr - KERNEL_VMA) + k_phys_start;
        }
        return kvaddr - PHYSMAP_BASE;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TlbFlushPage                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the TLB entry for a single virtual address on this CPU       *
     ********************************************************************************/
    static INLINE VOID TlbFlushPage(QWORD va) noexcept
    {
        hal::InvLpg(va);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TlbFlushAll                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the entire TLB by reloading CR3                              *
     ********************************************************************************/
    static INLINE VOID TlbFlushAll() noexcept
    {
        QWORD cr3 = hal::ReadCr3();
        hal::WriteCr3(cr3);
    }
} // namespace cbk::mem