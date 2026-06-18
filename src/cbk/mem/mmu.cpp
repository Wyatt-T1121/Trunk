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

#include <cbk/aspace.h>
#include <cbk/hal/io.h>

#include <macros.h>

namespace trunk::mem
{
    static u8 s_vaddr_width = 48;
    static u8 s_paddr_width = 52;

    static bool s_early_mmu         = true;
    static bool s_nx_supported      = false;
    static bool s_huge_supported    = false;
    static bool s_pcid_supported    = false;
    static bool s_invpcid_supported = false;

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : is_valid_paddr                                                     *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Returns true if pa is within the physical address width            *
         ********************************************************************************/
        NO_DISCARD bool is_valid_paddr(u64 pa) noexcept
        {
            const u64 max_paddr = (u64{1} << s_paddr_width) - 1;
            return pa <= max_paddr;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : is_canonical                                                       *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Check if va is an x86_64 virtual address                           *
         ********************************************************************************/
        NO_DISCARD bool is_canonical(u64 va) noexcept
        {
            const u8 shift      = s_vaddr_width - 1;
            const i64 signed_va = static_cast<i64>(va);
            return (signed_va >> shift) == 0 || (signed_va >> shift) == -1;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : alloc_page_table                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Allocate and zero a single 4KB page table                          *
         ********************************************************************************/
        NO_DISCARD u64 alloc_page_table() noexcept
        {
            u64 phys = 0;

            if (s_early_mmu) {
                phys = memblock_alloc(PAGE_SIZE, PAGE_SIZE);
            } else {
                Page *page = pfn_alloc_pages(0);
                if (!page)
                    return 0;
                phys = static_cast<u64>(page - g_PfnAllocator.mm_pfn_database) * PAGE_SIZE;
            }

            if (!phys)
                return 0;

            ASSERT(is_valid_paddr(phys), "alloc_page_table: allocated address exceeds paddr_width");

            auto *virt = reinterpret_cast<u64 *>(paddr_to_kvaddr(phys));
            for (usize i = 0; i < NO_OF_PT_ENTRIES; ++i)
                virt[i] = 0;
            return phys;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : get_or_alloc_table                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Walk one level of the page table                                   *
         ********************************************************************************/
        NO_DISCARD u64 *get_or_alloc_table(u64 *entry, bool alloc) noexcept
        {
            if (!(*entry & PAGE_PRESENT)) {
                if (!alloc)
                    return nullptr;

                u64 new_phys = alloc_page_table();
                if (!new_phys)
                    return nullptr;

                *entry = new_phys | PAGE_PRESENT | PAGE_WRITABLE;
            }

            return reinterpret_cast<u64 *>(paddr_to_kvaddr(PTE_ADDR(*entry)));
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : mmu_get_pte                                                        *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Walk the page table for va                                         *
         ********************************************************************************/
        NO_DISCARD u64 *mmu_get_pte(ArchAspace *space, u64 va, bool alloc) noexcept
        {
            ASSERT(space != nullptr, "mmu_get_pte: space is null");
            ASSERT(space->pml4_virt != nullptr, "mmu_get_pte: pml4_virt is null");
            ASSERT(is_canonical(va), "mmu_get_pte: non virtual address");

            u64 *pdpt = get_or_alloc_table(&space->pml4_virt[PML4X(va)], alloc);
            if (!pdpt)
                return nullptr;

            u64 *pd = get_or_alloc_table(&pdpt[PDPTX(va)], alloc);
            if (!pd)
                return nullptr;

            if (pd[PDX(va)] & PAGE_PRESENT && pd[PDX(va)] & (u64{1} << 7))
                return nullptr;

            u64 *pt = get_or_alloc_table(&pd[PDX(va)], alloc);
            if (!pt)
                return nullptr;

            return &pt[PTX(va)];
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : mmu_get_pde                                                        *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Walk to the PDE level for va                                       *
         ********************************************************************************/
        NO_DISCARD u64 *mmu_get_pde(ArchAspace *space, u64 va, bool alloc) noexcept
        {
            ASSERT(space != nullptr, "mmu_get_pde: space is null");
            ASSERT(space->pml4_virt != nullptr, "mmu_get_pde: pml4_virt is null");
            ASSERT(is_canonical(va), "mmu_get_pde: non-canonical virtual address");

            u64 *pdpt = get_or_alloc_table(&space->pml4_virt[PML4X(va)], alloc);
            if (!pdpt)
                return nullptr;

            u64 *pd = get_or_alloc_table(&pdpt[PDPTX(va)], alloc);
            if (!pd)
                return nullptr;

            return &pd[PDX(va)];
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : query_cpu_features                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Query Cpuid for paging CPU features and address widths             *
         ********************************************************************************/
        void query_cpu_features() noexcept
        {
            u32 eax, ebx, ecx, edx;

            hal::Cpuid(0x1, eax, ebx, ecx, edx);
            s_huge_supported    = (edx >> 3) & 1;
            s_pcid_supported    = (ecx >> 17) & 1;
            s_invpcid_supported = (ebx >> 10) & 1;

            hal::Cpuid(0x80000001, eax, ebx, ecx, edx);
            s_nx_supported = (edx >> 20) & 1;

            hal::Cpuid(0x80000008, eax, ebx, ecx, edx);
            s_paddr_width = static_cast<u8>(eax & 0xFF);
            s_vaddr_width = static_cast<u8>((eax >> 8) & 0xFF);
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_early_init                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init to setup In boot stage(uses memblock)               *
     ********************************************************************************/
    void mmu_early_init() noexcept
    {
        s_early_mmu = true;
        query_cpu_features();
        mmu_early_init_percpu();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : The true Initialization function for the memory management unit    *
     ********************************************************************************/
    void mmu_init() noexcept
    {
        s_early_mmu = false;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_early_init_percpu                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init for each cpu core                                   *
     ********************************************************************************/
    void mmu_early_init_percpu() noexcept
    {
        u64 cr0  = hal::ReadCr0();
        cr0     |= trunk::cpu::CR0_WP;
        hal::WriteCr0(cr0);

        u64 cr4  = hal::ReadCr4();
        cr4     |= cpu::CR4_PGE;
        hal::WriteCr4(cr4);

        if (s_nx_supported) {
            u32 efer_lo, efer_hi;
            asm volatile("rdmsr" : "=a"(efer_lo), "=d"(efer_hi) : "c"(0xC0000080));
            efer_lo |= (1u << trunk::cpu::EFER_NXE);
            asm volatile("wrmsr" ::"a"(efer_lo), "d"(efer_hi), "c"(0xC0000080));
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_page                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a page In the MMU                                             *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_page(ArchAspace *space, u64 va, u64 pa, u64 flags) noexcept
    {
        ASSERT(is_page_aligned(va), "mmu_map_page: va must be 4KB aligned");
        ASSERT(is_page_aligned(pa), "mmu_map_page: pa must be 4KB aligned");
        ASSERT(is_canonical(va), "mmu_map_page: non-canonical virtual address");
        ASSERT(is_valid_paddr(pa), "mmu_map_page: pa exceeds physical address width");

        if (!s_nx_supported)
            flags &= ~PAGE_NX;

        u64 *pte = mmu_get_pte(space, va, true);
        if (!pte)
            return false;

        ASSERT(!(*pte & PAGE_PRESENT), "mmu_map_page: remapping already-present page");

        *pte = PTE_ADDR(pa) | flags | PAGE_PRESENT;
        tlb_flush_page(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_page_huge                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a 2MB huge page in the MMU                                    *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_page_huge(ArchAspace *space, u64 va, u64 pa, u64 flags) noexcept
    {
        ASSERT(s_huge_supported, "mmu_map_page_huge: CPU does not support PSE");
        ASSERT((va & HUGE_MASK) == 0, "mmu_map_page_huge: va must be 2MB aligned");
        ASSERT((pa & HUGE_MASK) == 0, "mmu_map_page_huge: pa must be 2MB aligned");
        ASSERT(is_canonical(va), "mmu_map_page_huge: non-canonical virtual address");
        ASSERT(is_valid_paddr(pa), "mmu_map_page_huge: pa exceeds physical address width");

        if (!s_nx_supported)
            flags &= ~PAGE_NX;

        u64 *pde = mmu_get_pde(space, va, true);
        if (!pde)
            return false;

        ASSERT(!(*pde & PAGE_PRESENT), "mmu_map_page_huge: remapping already-present PDE");

        *pde = PTE_ADDR(pa) | flags | PAGE_PRESENT | PAGE_HUGE;
        tlb_flush_page(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_mmio                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a memory-mapped I/O region with cache-disable flags           *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_mmio(ArchAspace *space, u64 va, u64 pa, usize size) noexcept
    {
        ASSERT(is_page_aligned(va), "mmu_map_mmio: va must be 4KB aligned");
        ASSERT(is_page_aligned(pa), "mmu_map_mmio: pa must be 4KB aligned");
        ASSERT(is_page_aligned(size), "mmu_map_mmio: size must be a multiple of 4KB");

        const u64 flags = PAGE_WRITABLE | PAGE_PCD | PAGE_PWT | PAGE_NX;
        const MapRange range{va, pa, size};
        return mmu_map_range(space, range, flags);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_range                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a range In the MMU                                            *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_range(ArchAspace *space, MapRange range, u64 flags) noexcept
    {
        ASSERT(is_page_aligned(range.start_vaddr), "mmu_map_range: vaddr must be 4KB aligned");
        ASSERT(is_page_aligned(range.start_paddr), "mmu_map_range: paddr must be 4KB aligned");
        ASSERT(is_page_aligned(range.size), "mmu_map_range: size must be multiple of 4KB");
        ASSERT(range.size > 0, "mmu_map_range: size must be non-zero");

        u64 curr_va = range.start_vaddr;
        u64 curr_pa = range.start_paddr;

        for (usize progress = 0; progress < range.size; progress += PAGE_SIZE) {
            if (!mmu_map_page(space, curr_va, curr_pa, flags)) {
                u64 rollback_va = range.start_vaddr;
                for (usize clean = 0; clean < progress; clean += PAGE_SIZE) {
                    bool ok = mmu_unmap_page(space, rollback_va);
                    ASSERT(ok, "mmu_map_range: unmap failed during rollback");
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
     *  FUNC    : mmu_unmap_page                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmaps a page In the MMU                                           *
     ********************************************************************************/
    NO_DISCARD bool mmu_unmap_page(ArchAspace *space, u64 va) noexcept
    {
        ASSERT(is_page_aligned(va), "mmu_unmap_page: va must be 4KB aligned");
        ASSERT(is_canonical(va), "mmu_unmap_page: non-canonical virtual address");

        u64 *pte = mmu_get_pte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return false;

        *pte = 0;
        tlb_flush_page(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_translate                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a page in the mmu                                       *
     ********************************************************************************/
    NO_DISCARD u64 mmu_translate(ArchAspace *space, u64 va) noexcept
    {
        ASSERT(is_canonical(va), "mmu_translate: non-canonical virtual address");

        u64 *pte = mmu_get_pte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return 0;

        return PTE_ADDR(*pte) | PGOFF(va);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_is_mapped                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if va is mapped and present in the given space.       *
     ********************************************************************************/
    NO_DISCARD bool mmu_is_mapped(ArchAspace *space, u64 va) noexcept
    {
        if (!is_canonical(va))
            return false;

        u64 *pte = mmu_get_pte(space, va, false);
        return pte != nullptr && (*pte & PAGE_PRESENT);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_protect                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Update the flags on an existing page mapping                       *
     ********************************************************************************/
    NO_DISCARD bool mmu_protect(ArchAspace *space, u64 va, u64 new_flags) noexcept
    {
        ASSERT(is_page_aligned(va), "mmu_protect: va must be 4KB aligned");
        ASSERT(is_canonical(va), "mmu_protect: non-canonical virtual address");

        u64 *pte = mmu_get_pte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return false;

        if (!s_nx_supported)
            new_flags &= ~PAGE_NX;

        *pte = PTE_ADDR(*pte) | new_flags | PAGE_PRESENT;
        tlb_flush_page(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_query                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return the raw PTE value for va                                    *
     ********************************************************************************/
    NO_DISCARD u64 mmu_query(ArchAspace *space, u64 va) noexcept
    {
        if (!is_canonical(va))
            return 0;

        u64 *pte = mmu_get_pte(space, va, false);
        if (!pte)
            return 0;

        return *pte;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_clear_accessed                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clear the accessed bit on a mapped page                            *
     ********************************************************************************/
    bool mmu_clear_accessed(ArchAspace *space, u64 va) noexcept
    {
        ASSERT(is_canonical(va), "mmu_clear_accessed: non-canonical virtual address");

        u64 *pte = mmu_get_pte(space, va, false);
        if (!pte || !(*pte & PAGE_PRESENT))
            return false;

        *pte &= ~PAGE_ACCESSED;
        tlb_flush_page(va);
        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_load_cr3                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new address space into CR3                                  *
     ********************************************************************************/
    void mmu_load_cr3(const ArchAspace *space) noexcept
    {
        ASSERT(space != nullptr, "mmu_load_cr3: space is null");
        ASSERT(space->pml4_phys != 0, "mmu_load_cr3: pml4_phys is zero");
        ASSERT(is_page_aligned(space->pml4_phys), "mmu_load_cr3: pml4_phys not page aligned");

        hal::WriteCr3(space->pml4_phys);
    }

} // namespace trunk::mem