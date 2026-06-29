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

#include <cbk/hal/io.h>

#include <cbk/mm/mmdefs.h>
#include <cbk/mm/physical.h>

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
//          largepag

// So this file will be a lot more clean...

// Although, 4KB mappings are still apart of this file...
// We don't need to create a seperate 'smallpag'...

namespace cbk::mem
{
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
    NO_DISCARD BOOL MmuMapPage4K(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuUnmapPage4K                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the final PTE, clears the entry, executes TLB flush     *
     ********************************************************************************/
    NO_DISCARD BOOL MmuUnmapPage4K(QWORD virt) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuMapRange4K                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous virtual range to a continuous physical range      *
     ********************************************************************************/
    NO_DISCARD BOOL MmuMapRange4K(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmuUnmapRange4K                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 4KB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD BOOL MmuUnmapRange4K(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuProtectPage4K                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modify an existing PTE's attribute bits                            *
     ********************************************************************************/
    NO_DISCARD BOOL MMuProtectPage4K(QWORD virt, QWORD flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsRangeFree                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a range of virtual addresses has any existing mappings   *
     ********************************************************************************/
    NO_DISCARD BOOL MmuIsRangeFree(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsPagePresent                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a page is present                                        *
     ********************************************************************************/
    NO_DISCARD BOOL MmuIsPagePresent(QWORD virt) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuTranslateVirtualToPhysical                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds what physical frame address is mapped to a virtual pointer   *
     ********************************************************************************/
    NO_DISCARD QWORD MmuTranslateVirtualToPhysical(QWORD virt) noexcept;

} // namespace cbk::mem