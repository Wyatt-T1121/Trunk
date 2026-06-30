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

// MmuInitPerCpu...

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
     * FUNC    : MmuIterateRange                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Range loop helper                                                   *
     ********************************************************************************/
    template <SIZE_T STRIDE = PAGE_SIZE, typename F>
    NO_DISCARD CBKSTATUS MmuIterateRange(QWORD start, SIZE_T size, F action) noexcept
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
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuExecuteOnPte                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Walks down any tiers to leaf                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuExecuteOnPte(QWORD virt, PAGING_LEVEL target_level,
                                         BOOL alloc_if_missing, MmuPteAction action,
                                         PTE_CONTEXT &ctx) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuWriteCr3                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new root page table address into the CPU                    *
     ********************************************************************************/
    VOID MmuWriteCr3(QWORD pml4_phys) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInitializePerCpu                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the MMU driver on every CPU core                        *
     ********************************************************************************/
    VOID MmuInitPerCpu() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInitialize                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Runs once on Core 0, wraps ArchAspace                              *
     ********************************************************************************/
    VOID MmuInitialize(ArchAspace *krnl_space) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapPage4K                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the 4 levels, allocates missing sub-tables              *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuMapPage4K(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuUnmapPage4K                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the final PTE, clears the entry, executes TLB flush     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuUnmapPage4K(QWORD virt) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuMapRange4K                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous virtual range to a continuous physical range      *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuMapRange4K(QWORD vstart, QWORD pstart, SIZE_T size,
                                       QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuUnmapRange4K                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 4KB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuUnmapRange4K(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuProtectPage4K                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modify an existing PTE's attribute bits                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuProtectPage4K(QWORD virt, QWORD flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsRangeFree                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a range of virtual addresses has any existing mappings   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuIsRangeFree(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsPagePresent                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a page is present                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MmuIsPagePresent(QWORD virt) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuTranslateVirtualToPhysical                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds what physical frame address is mapped to a virtual pointer   *
     ********************************************************************************/
    NO_DISCARD QWORD MmuTranslateVirtualToPhysical(QWORD virt) noexcept;

} // namespace cbk::mem