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
    constexpr QWORD KB = 1024ULL;
    constexpr QWORD MB = 1024ULL * KB;
    constexpr QWORD GB = 1024ULL * MB;

    constexpr QWORD PAGE_SIZE  = 4096;
    constexpr QWORD PAGE_MASK  = ~(PAGE_SIZE - 1);
    constexpr QWORD PAGE_SHIFT = 12;

    constexpr QWORD HUGE_PAGE_SIZE = 2 * MB;
    constexpr QWORD HUGE_MASK      = HUGE_PAGE_SIZE - 1;
    constexpr QWORD PAGE_HUGE      = QWORD{1} << 7;

    constexpr QWORD PAGE_WRITE_THROUGH = 1ULL << 3;
    constexpr QWORD PAGE_CACHE_DISABLE = 1ULL << 4;
    constexpr QWORD PAGE_PAT           = 1ULL << 7;

    constexpr QWORD PAGE_PRESENT  = (1ULL << 0);
    constexpr QWORD PAGE_WRITABLE = (1ULL << 1);
    constexpr QWORD PAGE_USER     = (1ULL << 2);
    constexpr QWORD PAGE_PWT      = (1ULL << 3);
    constexpr QWORD PAGE_PCD      = (1ULL << 4);
    constexpr QWORD PAGE_ACCESSED = (1ULL << 5);
    constexpr QWORD PAGE_DIRTY    = (1ULL << 6);
    constexpr QWORD PAGE_GLOBAL   = (1ULL << 8);
    constexpr QWORD PAGE_NX       = (1ULL << 63);

    constexpr QWORD PTE_AVAIL = 0xE00;
    constexpr QWORD PTE_USER  = PTE_AVAIL | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

    constexpr QWORD NO_OF_PT_ENTRIES = 512;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PPN                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Gets physical page number from address                              *
     ********************************************************************************/
    NO_DISCARD constexpr ULONG_PTR PPN(ULONG_PTR la) noexcept
    {
        return la >> 12;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : VPN                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Gets virtual page number from address                               *
     ********************************************************************************/
    NO_DISCARD constexpr ULONG_PTR VPN(ULONG_PTR la) noexcept
    {
        return PPN(la);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PML4X                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PML4 table index                                       *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD PML4X(ULONG_PTR la) noexcept
    {
        return (la >> 39) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDPTX                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PDP table index                                        *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD PDPTX(ULONG_PTR la) noexcept
    {
        return (la >> 30) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page directory index                                   *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD PDX(ULONG_PTR la) noexcept
    {
        return (la >> 21) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page table index                                       *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD PTX(ULONG_PTR la) noexcept
    {
        return (la >> 12) & 0x1FF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGOFF                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the byte offset within a page                              *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD PGOFF(ULONG_PTR la) noexcept
    {
        return la & 0xFFF;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGADDR                                                              *
     * DATE    : 2026                                                                *
     * PURPOSE : Constructs a virtual address from indexes                           *
     ********************************************************************************/
    NO_DISCARD constexpr ULONG_PTR PGADDR(QWORD m, QWORD d, QWORD p, QWORD t, QWORD o) noexcept
    {
        return (m << 39) | (d << 30) | (p << 21) | (t << 12) | o;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTE_ADDR                                                            *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the physical address from an entry                         *
     ********************************************************************************/
    NO_DISCARD constexpr ULONG_PTR PTE_ADDR(ULONG_PTR pte) noexcept
    {
        return pte & 0x000FFFFFFFFFF000ULL;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PageAlignDown                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address down to page boundary                             *
     ********************************************************************************/
    NO_DISCARD static inline QWORD PageAlignDown(QWORD addr) noexcept
    {
        return addr & PAGE_MASK;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PageAlignUp                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address up to page boundary                               *
     ********************************************************************************/
    NO_DISCARD static inline QWORD PageAlignUp(QWORD addr) noexcept
    {
        return (addr + PAGE_SIZE - 1) & PAGE_MASK;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPageAligned                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if addr is 4KB aligned                                *
     ********************************************************************************/
    NO_DISCARD static inline BOOL IsPageAligned(QWORD addr) noexcept
    {
        return (addr & (PAGE_SIZE - 1)) == 0;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PaddrToKvaddr                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a physical address to a virtual address                    *
     ********************************************************************************/
    NO_DISCARD static inline PVOID PaddrToKvaddr(QWORD paddr) noexcept
    {
        return reinterpret_cast<PVOID>(paddr + 0xFFFFFFFF80000000ULL);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : KvaddrToPaddr                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a virtual address to a physical adddress                   *
     ********************************************************************************/
    NO_DISCARD static inline PVOID KvaddrToPaddr(QWORD kvaddr) noexcept
    {
        return reinterpret_cast<PVOID>(kvaddr - 0xFFFFFFFF80000000ULL);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TlbFlushPage                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the TLB entry for a single virtual address on this CPU       *
     ********************************************************************************/
    static inline VOID TlbFlushPage(QWORD va) noexcept
    {
        hal::InvLpg(va);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TlbFlushAll                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the entire TLB by reloading CR3                              *
     ********************************************************************************/
    static inline VOID TlbFlushAll() noexcept
    {
        QWORD cr3 = hal::ReadCr3();
        hal::WriteCr3(cr3);
    }
} // namespace trunk::mem