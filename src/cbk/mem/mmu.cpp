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
 *  MODULE  : Memory management unit                                             *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Paging, segments, and everything else related to mmu               *
 ********************************************************************************/
#include <cbk/mem/mmu.h>

#include <cbk/mem/memblock.h>
#include <cbk/mem/page_alloc.h>
#include <cbk/mem/pfn.h>

#include <cbk/hal/io.h>
#include <cbk/mem/aspace.h>

#include <macros.h>

namespace trunk::mem
{
    static BYTE s_vaddr_width = 48;
    static BYTE s_paddr_width = 52;

    static BOOL s_early_mmu         = true;
    static BOOL s_nx_supported      = false;
    static BOOL s_huge_supported    = false;
    static BOOL s_pcid_supported    = false;
    static BOOL s_invpcid_supported = false;

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : IsValidPaddr                                                       *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Returns true if pa is within the physical address width            *
         ********************************************************************************/
        NO_DISCARD BOOL IsValidPaddr(QWORD pa) noexcept
        {
            const QWORD max_paddr = (QWORD{1} << s_paddr_width) - 1;
            return pa <= max_paddr;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : IsCanonical                                                        *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Check if va is an x86_64 virtual address                           *
         ********************************************************************************/
        NO_DISCARD BOOL IsCanonical(QWORD va) noexcept
        {
            const BYTE shift         = s_vaddr_width - 1;
            const LONGLONG signed_va = static_cast<LONGLONG>(va);
            return (signed_va >> shift) == 0 || (signed_va >> shift) == -1;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : AllocPageTable                                                     *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Allocate and zero a single 4KB page table                          *
         ********************************************************************************/
        NO_DISCARD QWORD AllocPageTable() noexcept
        {
            QWORD phys = 0;

            if (s_early_mmu) {
                phys = MemblockAlloc(PAGE_SIZE, PAGE_SIZE);
            } else {
                Page *page = PfnAllocPages(0);
                if (!page)
                    return 0;
                phys = static_cast<QWORD>(page - g_PfnAllocator.mm_pfn_database) * PAGE_SIZE;
            }

            if (!phys)
                return 0;

            ASSERT(IsValidPaddr(phys), "AllocPageTable: allocated address exceeds paddr_width");

            auto *virt = reinterpret_cast<QWORD *>(PaddrToKvaddr(phys));
            for (SIZE_T i = 0; i < NO_OF_PT_ENTRIES; ++i)
                virt[i] = 0;
            return phys;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : GetOrAllocTable                                                    *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Walk one level of the page table                                   *
         ********************************************************************************/
        NO_DISCARD QWORD *GetOrAllocTable(QWORD *entry, BOOL alloc) noexcept
        {
            if (!(*entry & PAGE_PRESENT)) {
                if (!alloc)
                    return nullptr;

                QWORD new_phys = AllocPageTable();
                if (!new_phys)
                    return nullptr;

                *entry = new_phys | PAGE_PRESENT | PAGE_WRITABLE;
            }

            return reinterpret_cast<QWORD *>(PaddrToKvaddr(PTE_ADDR(*entry)));
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuGetPte                                                          *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Walk the page table for va                                         *
         ********************************************************************************/
        NO_DISCARD QWORD *MmuGetPte(ArchAspace *space, QWORD va, BOOL alloc) noexcept
        {
            ASSERT(space != nullptr, "MmuGetPte: space is null");
            ASSERT(space->pml4_virt != nullptr, "MmuGetPte: pml4_virt is null");
            ASSERT(IsCanonical(va), "MmuGetPte: non virtual address");

            QWORD *pdpt = GetOrAllocTable(&space->pml4_virt[PML4X(va)], alloc);
            if (!pdpt)
                return nullptr;

            QWORD *pd = GetOrAllocTable(&pdpt[PDPTX(va)], alloc);
            if (!pd)
                return nullptr;

            if (pd[PDX(va)] & PAGE_PRESENT && pd[PDX(va)] & (QWORD{1} << 7))
                return nullptr;

            QWORD *pt = GetOrAllocTable(&pd[PDX(va)], alloc);
            if (!pt)
                return nullptr;

            return &pt[PTX(va)];
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuGetPde                                                          *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Walk to the PDE level for va                                       *
         ********************************************************************************/
        NO_DISCARD QWORD *MmuGetPde(ArchAspace *space, QWORD va, BOOL alloc) noexcept
        {
            ASSERT(space != nullptr, "MmuGetPde: space is null");
            ASSERT(space->pml4_virt != nullptr, "MmuGetPde: pml4_virt is null");
            ASSERT(IsCanonical(va), "MmuGetPde: non-canonical virtual address");

            QWORD *pdpt = GetOrAllocTable(&space->pml4_virt[PML4X(va)], alloc);
            if (!pdpt)
                return nullptr;

            QWORD *pd = GetOrAllocTable(&pdpt[PDPTX(va)], alloc);
            if (!pd)
                return nullptr;

            return &pd[PDX(va)];
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : QueryCpuFeatures                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Query Cpuid for paging CPU features and address widths             *
         ********************************************************************************/
        VOID QueryCpuFeatures() noexcept
        {
            DWORD eax, ebx, ecx, edx;

            hal::Cpuid(0x1, eax, ebx, ecx, edx);
            s_huge_supported    = (edx >> 3) & 1;
            s_pcid_supported    = (ecx >> 17) & 1;
            s_invpcid_supported = (ebx >> 10) & 1;

            hal::Cpuid(0x80000001, eax, ebx, ecx, edx);
            s_nx_supported = (edx >> 20) & 1;

            hal::Cpuid(0x80000008, eax, ebx, ecx, edx);
            s_paddr_width = static_cast<BYTE>(eax & 0xFF);
            s_vaddr_width = static_cast<BYTE>((eax >> 8) & 0xFF);
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuEarlyInit                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init to setup In boot stage(uses memblock)               *
     ********************************************************************************/
    VOID MmuEarlyInit() noexcept
    {
        s_early_mmu = true;
        QueryCpuFeatures();
        MmuEarlyInitPerCpu();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : The true Initialization function for the memory management unit    *
     ********************************************************************************/
    VOID MmuInit() noexcept
    {
        s_early_mmu = false;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuEarlyInitPerCpu                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init for each cpu core                                   *
     ********************************************************************************/
    VOID MmuEarlyInitPerCpu() noexcept
    {
        QWORD cr0  = hal::ReadCr0();
        cr0       |= trunk::cpu::CR0_WP;
        hal::WriteCr0(cr0);

        QWORD cr4  = hal::ReadCr4();
        cr4       |= cpu::CR4_PGE;
        hal::WriteCr4(cr4);

        if (s_nx_supported) {
            DWORD efer_lo, efer_hi;
            asm volatile("rdmsr" : "=a"(efer_lo), "=d"(efer_hi) : "c"(0xC0000080));
            efer_lo |= (1u << trunk::cpu::EFER_NXE);
            asm volatile("wrmsr" ::"a"(efer_lo), "d"(efer_hi), "c"(0xC0000080));
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapPage                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a page In the MMU                                             *
     ********************************************************************************/
    NO_DISCARD BOOL MmuMapPage(ArchAspace *space, QWORD va, QWORD pa, QWORD flags) noexcept
    {
        ASSERT(IsPageAligned(va), "MmuMapPage: va must be 4KB aligned");
        ASSERT(IsPageAligned(pa), "MmuMapPage: pa must be 4KB aligned");
        ASSERT(IsCanonical(va), "MmuMapPage: non-canonical virtual address");
        ASSERT(IsValidPaddr(pa), "MmuMapPage: pa exceeds physical address width");

        if (!s_nx_supported)
            flags &= ~PAGE_NX;

        QWORD *pte = MmuGetPte(space, va, true);
        if (!pte)
            return false;

        ASSERT(!(*pte & PAGE_PRESENT), "MmuMapPage: remapping already-present page");

        *pte = PTE_ADDR(pa) | flags | PAGE_PRESENT;
        TlbFlushPage(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapPageHuge                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a 2MB huge page in the MMU                                    *
     ********************************************************************************/
    NO_DISCARD BOOL MmuMapPageHuge(ArchAspace *space, QWORD va, QWORD pa, QWORD flags) noexcept
    {
        ASSERT(s_huge_supported, "MmuMapPageHuge: CPU does not support PSE");
        ASSERT((va & HUGE_MASK) == 0, "MmuMapPageHuge: va must be 2MB aligned");
        ASSERT((pa & HUGE_MASK) == 0, "MmuMapPageHuge: pa must be 2MB aligned");
        ASSERT(IsCanonical(va), "MmuMapPageHuge: non-canonical virtual address");
        ASSERT(IsValidPaddr(pa), "MmuMapPageHuge: pa exceeds physical address width");

        if (!s_nx_supported)
            flags &= ~PAGE_NX;

        QWORD *pde = MmuGetPde(space, va, true);
        if (!pde)
            return false;

        ASSERT(!(*pde & PAGE_PRESENT), "MmuMapPageHuge: remapping already-present PDE");

        *pde = PTE_ADDR(pa) | flags | PAGE_PRESENT | PAGE_HUGE;
        TlbFlushPage(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapMmio                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a memory-mapped I/O region with cache-disable flags           *
     ********************************************************************************/
    NO_DISCARD BOOL MmuMapMmio(ArchAspace *space, QWORD va, QWORD pa, SIZE_T size) noexcept
    {
        ASSERT(IsPageAligned(va), "MmuMapMmio: va must be 4KB aligned");
        ASSERT(IsPageAligned(pa), "MmuMapMmio: pa must be 4KB aligned");
        ASSERT(IsPageAligned(size), "MmuMapMmio: size must be a multiple of 4KB");

        const QWORD flags = PAGE_WRITABLE | PAGE_PCD | PAGE_PWT | PAGE_NX;
        const MapRange range{va, pa, size};
        return MmuMapRange(space, range, flags);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapRange                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a range In the MMU                                            *
     ********************************************************************************/
    NO_DISCARD BOOL MmuMapRange(ArchAspace *space, MapRange range, QWORD flags) noexcept
    {
        ASSERT(IsPageAligned(range.start_vaddr), "MmuMapRange: vaddr must be 4KB aligned");
        ASSERT(IsPageAligned(range.start_paddr), "MmuMapRange: paddr must be 4KB aligned");
        ASSERT(IsPageAligned(range.size), "MmuMapRange: size must be multiple of 4KB");
        ASSERT(range.size > 0, "MmuMapRange: size must be non-zero");

        QWORD curr_va = range.start_vaddr;
        QWORD curr_pa = range.start_paddr;

        for (SIZE_T progress = 0; progress < range.size; progress += PAGE_SIZE) {
            if (!MmuMapPage(space, curr_va, curr_pa, flags)) {
                QWORD rollback_va = range.start_vaddr;
                for (SIZE_T clean = 0; clean < progress; clean += PAGE_SIZE) {
                    BOOL ok = MmuUnmapPage(space, rollback_va);
                    ASSERT(ok, "MmuMapRange: unmap failed during rollback");
                    rollback_va += PAGE_SIZE;
                }
                return false;
            }

            curr_va += PAGE_SIZE;
            curr_pa += PAGE_SIZE;
        }

        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuUnmapPage                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmaps a page In the MMU                                           *
     ********************************************************************************/
    NO_DISCARD BOOL MmuUnmapPage(ArchAspace *space, QWORD va) noexcept
    {
        ASSERT(IsPageAligned(va), "MmuUnmapPage: va must be 4KB aligned");
        ASSERT(IsCanonical(va), "MmuUnmapPage: non-canonical virtual address");

        QWORD *pte = MmuGetPte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return false;

        *pte = 0;
        TlbFlushPage(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuTranslate                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a page in the mmu                                       *
     ********************************************************************************/
    NO_DISCARD QWORD MmuTranslate(ArchAspace *space, QWORD va) noexcept
    {
        ASSERT(IsCanonical(va), "MmuTranslate: non-canonical virtual address");

        QWORD *pte = MmuGetPte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return 0;

        return PTE_ADDR(*pte) | PGOFF(va);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsMapped                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if va is mapped and present in the given space.       *
     ********************************************************************************/
    NO_DISCARD BOOL MmuIsMapped(ArchAspace *space, QWORD va) noexcept
    {
        if (!IsCanonical(va))
            return false;

        QWORD *pte = MmuGetPte(space, va, false);
        return pte != nullptr && (*pte & PAGE_PRESENT);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuProtect                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Update the flags on an existing page mapping                       *
     ********************************************************************************/
    NO_DISCARD BOOL MmuProtect(ArchAspace *space, QWORD va, QWORD new_flags) noexcept
    {
        ASSERT(IsPageAligned(va), "MmuProtect: va must be 4KB aligned");
        ASSERT(IsCanonical(va), "MmuProtect: non-canonical virtual address");

        QWORD *pte = MmuGetPte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return false;

        if (!s_nx_supported)
            new_flags &= ~PAGE_NX;

        *pte = PTE_ADDR(*pte) | new_flags | PAGE_PRESENT;
        TlbFlushPage(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuQuery                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return the raw PTE value for va                                    *
     ********************************************************************************/
    NO_DISCARD QWORD MmuQuery(ArchAspace *space, QWORD va) noexcept
    {
        if (!IsCanonical(va))
            return 0;

        QWORD *pte = MmuGetPte(space, va, false);
        if (!pte)
            return 0;

        return *pte;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuClearAccessed                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clear the accessed bit on a mapped page                            *
     ********************************************************************************/
    BOOL MmuClearAccessed(ArchAspace *space, QWORD va) noexcept
    {
        ASSERT(IsCanonical(va), "MmuClearAccessed: non-canonical virtual address");

        QWORD *pte = MmuGetPte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return false;

        *pte &= ~PAGE_ACCESSED;
        TlbFlushPage(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuLoadCr3                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new address space into CR3                                  *
     ********************************************************************************/
    VOID MmuLoadCr3(const ArchAspace *space) noexcept
    {
        ASSERT(space != nullptr, "MmuLoadCr3: space is null");
        ASSERT(space->pml4_phys != 0, "MmuLoadCr3: pml4_phys is zero");
        ASSERT(IsPageAligned(space->pml4_phys), "MmuLoadCr3: pml4_phys not page aligned");

        hal::WriteCr3(space->pml4_phys);
    }

} // namespace trunk::mem