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

namespace trunk::mem
{
    constexpr u64 KB = 1024ULL;
    constexpr u64 MB = 1024ULL * KB;
    constexpr u64 GB = 1024ULL * MB;

    constexpr u64 PAGE_SIZE  = 4096;
    constexpr u64 PAGE_MASK  = ~(PAGE_SIZE - 1);
    constexpr u64 PAGE_SHIFT = 12;

    constexpr u64 HUGE_PAGE_SIZE = 2 * MB;
    constexpr u64 HUGE_MASK      = HUGE_PAGE_SIZE - 1;
    constexpr u64 PAGE_HUGE      = u64{1} << 7;

    constexpr u64 PAGE_WRITE_THROUGH = 1ULL << 3;
    constexpr u64 PAGE_CACHE_DISABLE = 1ULL << 4;
    constexpr u64 PAGE_PAT           = 1ULL << 7;

    constexpr u64 PAGE_PRESENT  = (1ULL << 0);
    constexpr u64 PAGE_WRITABLE = (1ULL << 1);
    constexpr u64 PAGE_USER     = (1ULL << 2);
    constexpr u64 PAGE_PWT      = (1ULL << 3);
    constexpr u64 PAGE_PCD      = (1ULL << 4);
    constexpr u64 PAGE_ACCESSED = (1ULL << 5);
    constexpr u64 PAGE_DIRTY    = (1ULL << 6);
    constexpr u64 PAGE_GLOBAL   = (1ULL << 8);
    constexpr u64 PAGE_NX       = (1ULL << 63);

    constexpr u64 PTE_AVAIL = 0xE00;
    constexpr u64 PTE_USER  = PTE_AVAIL | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

    constexpr u64 NO_OF_PT_ENTRIES = 512;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PPN                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Gets physical page number from address                              *
     ********************************************************************************/
    NO_DISCARD constexpr uptr PPN(uptr la) noexcept
    {
        return la >> 12;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : VPN                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Gets virtual page number from address                               *
     ********************************************************************************/
    NO_DISCARD constexpr uptr VPN(uptr la) noexcept
    {
        return PPN(la);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PML4X                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PML4 table index                                       *
     ********************************************************************************/
    NO_DISCARD constexpr u64 PML4X(uptr la) noexcept
    {
        return (la >> 39) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDPTX                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PDP table index                                        *
     ********************************************************************************/
    NO_DISCARD constexpr u64 PDPTX(uptr la) noexcept
    {
        return (la >> 30) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page directory index                                   *
     ********************************************************************************/
    NO_DISCARD constexpr u64 PDX(uptr la) noexcept
    {
        return (la >> 21) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page table index                                       *
     ********************************************************************************/
    NO_DISCARD constexpr u64 PTX(uptr la) noexcept
    {
        return (la >> 12) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGOFF                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the byte offset within a page                              *
     ********************************************************************************/
    NO_DISCARD constexpr u64 PGOFF(uptr la) noexcept
    {
        return la & 0xFFF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGADDR                                                              *
     * DATE    : 2026                                                                *
     * PURPOSE : Constructs a virtual address from indexes                           *
     ********************************************************************************/
    NO_DISCARD constexpr uptr PGADDR(u64 m, u64 d, u64 p, u64 t, u64 o) noexcept
    {
        return (m << 39) | (d << 30) | (p << 21) | (t << 12) | o;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTE_ADDR                                                            *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the physical address from an entry                         *
     ********************************************************************************/
    NO_DISCARD constexpr uptr PTE_ADDR(uptr pte) noexcept
    {
        return pte & 0x000FFFFFFFFFF000ULL;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : page_align_down                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address down to page boundary                             *
     ********************************************************************************/
    NO_DISCARD static inline u64 page_align_down(u64 addr) noexcept
    {
        return addr & PAGE_MASK;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : page_align_up                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address up to page boundary                               *
     ********************************************************************************/
    NO_DISCARD static inline u64 page_align_up(u64 addr) noexcept
    {
        return (addr + PAGE_SIZE - 1) & PAGE_MASK;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : is_page_aligned                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if addr is 4KB aligned                                *
     ********************************************************************************/
    NO_DISCARD static inline bool is_page_aligned(u64 addr) noexcept
    {
        return (addr & (PAGE_SIZE - 1)) == 0;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : paddr_to_kvaddr                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a physical address to a virtual address                    *
     ********************************************************************************/
    NO_DISCARD static inline void *paddr_to_kvaddr(u64 paddr) noexcept
    {
        return reinterpret_cast<void *>(paddr + 0xFFFFFFFF80000000ULL);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : kvaddr_to_paddr                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a virtual address to a physical adddress                   *
     ********************************************************************************/
    NO_DISCARD static inline void *kvaddr_to_paddr(u64 kvaddr) noexcept
    {
        return reinterpret_cast<void *>(kvaddr - 0xFFFFFFFF80000000ULL);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tlb_flush_page                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the TLB entry for a single virtual address on this CPU       *
     ********************************************************************************/
    static inline void tlb_flush_page(u64 va) noexcept
    {
        hal::InvLpg(va);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tlb_flush_all                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the entire TLB by reloading CR3                              *
     ********************************************************************************/
    static inline void tlb_flush_all() noexcept
    {
        u64 cr3 = hal::ReadCr3();
        hal::WriteCr3(cr3);
    }
} // namespace trunk::mem