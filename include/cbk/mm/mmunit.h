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
#pragma once

#include <lddef.h>
#include <types.h>

#include <assert.h>
#include <attributes.h>
#include <status.h>

#include <cbk/hal/io.h>

#include <cbk/mm/mmdefs.h>
#include <cbk/mm/physical.h>

#include <tklib/string.h>

// Memory management unit...
// This file is apart of the virtual memory system...

// DUTIES:
// Write CR3 register
// Invalidate pages in the TLB cache
// Walk and modify Page Table Entries(PTE)

// Because we are only supporting x86_64,
// We can have one mmu file...

// Usually, each CPU core get's it's own MMU,
// Because we don't have multi-core yet,
// We will leave the function blank.

// MmInitializePageTablesPerCpu...

// I also split logic into 3 seperate files:
//          mmu
//          ncache
//          mappag

// So this file will be a lot more clean...

namespace cbk::mem
{
    using MmuPteAction = VOID (*)(PPAGE_TABLE_ENTRY target_pte, PTE_CONTEXT &ctx) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmIterateRange                                                      *
     * DATE    : 2026                                                                *
     * PURPOSE : Range loop helper                                                   *
     ********************************************************************************/
    template <SIZE_T STRIDE = PAGE_SIZE, typename F>
    NO_DISCARD CBKSTATUS
    MmIterateRange(QWORD start, SIZE_T size, F action) noexcept
    {
        if ((start & (STRIDE - 1)) != 0)
            return STATUS_DATATYPE_MISALIGNMENT;

        SIZE_T step_count = (size + STRIDE - 1) / STRIDE;

        for (SIZE_T i = 0; i < step_count; ++i) {
            CBKSTATUS status = action(start + (i * STRIDE), i);
            if (status != STATUS_SUCCESS)
                return status;
        }

        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetTableIndex                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Extract 9-bit table index for a given level                        *
     ********************************************************************************/
    NO_DISCARD ULONG
    MmGetTableIndex(QWORD virt, PAGING_LEVEL lvl) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetTablePointer                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Convert a physical frame back into a virtual page table pointer    *
     ********************************************************************************/
    NO_DISCARD PPAGE_TABLE
    MmGetTablePointer(PAGE_TABLE_ENTRY entry) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmExecuteOnPte                                                      *
     * DATE    : 2026                                                                *
     * PURPOSE : Walks down any tiers to leaf                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmExecuteOnPte(QWORD virt,
                   PAGING_LEVEL target_level,
                   BOOL alloc_if_missing,
                   MmuPteAction action,
                   PTE_CONTEXT &ctx) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmWalkToTable                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the page tables to a specific level                     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmWalkToTable(QWORD pml4_phys,
                  QWORD virt,
                  PAGING_LEVEL target_level,
                  BOOL alloc_if_missing,
                  PPAGE_TABLE_ENTRY &out_entry,
                  PAGING_LEVEL &out_resolved_level) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmProtectRange                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Update protection flags across a range                             *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmProtectRange(QWORD start_addr, SIZE_T size, ULONG new_protection) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmWriteCr3                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new root page table address into the CPU                    *
     ********************************************************************************/
    VOID
    MmWriteCr3(QWORD pml4_phys) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInitializePerCpu                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the MMU driver on every CPU core                        *
     ********************************************************************************/
    VOID
    MmInitializePageTablesPerCpu() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HalInitializeMmu                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Runs once on Core 0, wraps ArchAspace                              *
     ********************************************************************************/
    VOID
    HalInitializeMmu(ArchAspace *krnl_space) noexcept;

} // namespace cbk::mem