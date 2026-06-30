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
 *  PURPOSE : CPU MMU driver                                                     *
 ********************************************************************************/
#include <cbk/mm/mmunit.h>

#include <cbk/mm/mappag.h>

namespace cbk::mem
{
    ArchAspace *krnl_space = nullptr;

    namespace
    {

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapIdentityPhysmap                                              *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Internal helper, map direct identity/physmap window                *
         ********************************************************************************/
        VOID MmuMapIdentityPhysmap() noexcept
        {
            PFN_NUM pg_hi        = MmGetHighestPhysicalPage();
            SIZE_T phys_map_size = pg_hi * PAGE_SIZE;

            CBKSTATUS status = MapRange4K(PHYSMAP_BASE, 0x0, phys_map_size,
                                          PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL);
            ASSERT(status == STATUS_SUCCESS,
                   "MmuMapIdentityPhysmap: Failed to map direct phys mem window");
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : IMmuMapSection                                                     *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Map a section                                                      *
         ********************************************************************************/
        VOID IMmuMapSection(PVOID section_start, PVOID section_end, QWORD hw_flags) noexcept
        {
            QWORD vstart = reinterpret_cast<QWORD>(section_start);
            QWORD pstart = vstart - KERNEL_VMA;

            SIZE_T size = reinterpret_cast<QWORD>(section_end) - vstart;

            CBKSTATUS status = MapRange4K(vstart, pstart, size, hw_flags);
            ASSERT(status == STATUS_SUCCESS, "IMmuMapSection: Failed to map kernel section...?");
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapTextSection                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .text section (EXECUTABLE CODE)                            *
         ********************************************************************************/
        VOID MmuMapTextSection() noexcept
        {
            IMmuMapSection(__text_start, __text_end, TEXT_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapRoDataSection                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .rodata section (CONSTANT DATA)                            *
         ********************************************************************************/
        VOID MmuMapRoDataSection() noexcept
        {
            IMmuMapSection(__rodata_start, __rodata_end, RODATA_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuMapDataBssSection                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Protect .bss section (UNINITIALIZED DATA)                          *
         ********************************************************************************/
        VOID MmuMapDataBssSection() noexcept
        {
            IMmuMapSection(__bss_start, __stack_top, BSS_SECTION_HW_FLAGS);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuGetTableIndex                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Extract 9-bit table index for a given level                        *
         ********************************************************************************/
        NO_DISCARD ULONG MmuGetTableIndex(QWORD virt, PAGING_LEVEL lvl) noexcept
        {
            return (virt >> static_cast<ULONG>(lvl)) & IDX_BITSHIFT;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MmuGetTablePointer                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Convert a physical frame back into a virtual page table pointer    *
         ********************************************************************************/
        NO_DISCARD PPAGE_TABLE MmuGetTablePointer(PAGE_TABLE_ENTRY entry) noexcept
        {
            QWORD phys_addr = static_cast<QWORD>(entry.Bits.page_frame) << PAGE_SHIFT;
            return reinterpret_cast<PPAGE_TABLE>(PaddrToKvaddr(phys_addr));
        }
    } // namespace

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuExecuteOnPte                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Walks down any tiers to leaf                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuExecuteOnPte(QWORD virt, PAGING_LEVEL target_level,
                                         BOOL alloc_if_missing, MmuPteAction action,
                                         PTE_CONTEXT &ctx) noexcept
    {
        QWORD alignment_mask =
            (target_level == PAGING_LEVEL::PD) ? (HUGE_PAGE_SIZE - 1) : (PAGE_SIZE - 1);
        if ((virt & alignment_mask) != 0)
            return STATUS_DATATYPE_MISALIGNMENT;

        PPAGE_TABLE_ENTRY working_table =
            reinterpret_cast<PPAGE_TABLE_ENTRY>(PaddrToKvaddr(krnl_space->pml4_phys));

        constexpr PAGING_LEVEL steps[] = {PAGING_LEVEL::PML4, PAGING_LEVEL::PDPT, PAGING_LEVEL::PD};

        for (PAGING_LEVEL level : steps) {
            ULONG idx = MmuGetTableIndex(virt, level);

            if (level == target_level) {
                PPAGE_TABLE_ENTRY target_pte = &working_table[idx];
                QWORD old_val                = target_pte->val;

                action(target_pte, ctx);

                if (target_pte->val != old_val)
                    hal::InvLpg(virt);

                return STATUS_SUCCESS;
            }

            PAGE_TABLE_ENTRY entry = working_table[idx];

            if (!entry.Bits.present) {
                if (!alloc_if_missing)
                    return STATUS_NOT_FOUND;

                PFN_NUM new_pfn = MmAllocPage(static_cast<ULONG>(MC_TYPE::SYSTEM));
                if (new_pfn == 0)
                    return STATUS_NO_MEMORY;

                QWORD new_paddr = PfnToAddr(new_pfn);

                PPAGE_TABLE_ENTRY new_tbl_virt =
                    reinterpret_cast<PPAGE_TABLE_ENTRY>(PaddrToKvaddr(new_paddr));

                tklib::memset(new_tbl_virt, 0, PAGE_SIZE);

                working_table[idx].val = new_paddr | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
                entry                  = working_table[idx];
            }

            if (entry.Bits.large_page) {
                if (!alloc_if_missing && ctx.extra == PHYS_ADDR_MAX)
                    action(&working_table[idx], ctx);
                return STATUS_LARGE_PAGE;
            }

            working_table = reinterpret_cast<PPAGE_TABLE_ENTRY>(MmuGetTablePointer(entry));
        }

        if (target_level == PAGING_LEVEL::PT) {
            ULONG pt_idx                 = MmuGetTableIndex(virt, PAGING_LEVEL::PT);
            PPAGE_TABLE_ENTRY target_pte = &working_table[pt_idx];
            QWORD old_val                = target_pte->val;

            action(target_pte, ctx);

            if (target_pte->val != old_val)
                hal::InvLpg(virt);

            return STATUS_SUCCESS;
        }

        return STATUS_NOT_FOUND;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuWriteCr3                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new root page table address into the CPU                    *
     ********************************************************************************/
    VOID MmuWriteCr3(QWORD pml4_phys) noexcept
    {
        ASSERT((pml4_phys & 0xFFF) == 0, "PML4 physical address must be page-aligned!");
        hal::WriteCr3(pml4_phys);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInitializePerCpu                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the MMU driver on every CPU core                        *
     ********************************************************************************/
    VOID MmuInitPerCpu() noexcept
    {
        // MULTI-CORE NOT ADDED
        // TODO: INIT PER CPU
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInitialize                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Runs once on Core 0, wraps ArchAspace                              *
     ********************************************************************************/
    VOID MmuInitialize(ArchAspace *space) noexcept
    {
        ASSERT(space != nullptr, "MmuInitialize: Kernel address space cannot be nullptr");
        krnl_space = space;

        // Map the massive direct physmap (window)
        MmuMapIdentityPhysmap();

        // PROTECTION: SECTIONS
        // THIS APPLIES SPECIFIC HW_FLAGS TO EACH SECTION
        // THIS PROTECTS THEM WHEN IT COMES TO WRITING AND READING
        MmuMapTextSection();
        MmuMapRoDataSection();
        MmuMapDataBssSection();

        // Throw hw switch to verified tables
        MmuWriteCr3(krnl_space->pml4_phys);

        // DOES NOTHING
        MmuInitPerCpu();
    }
} // namespace cbk::mem