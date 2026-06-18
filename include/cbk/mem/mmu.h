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
#pragma once

#include <macros.h>
#include <types.h>

#include <cbk/cpu/cpu_flags.h>

#include <cbk/mem/page.h>

#include <cbk/mem/aspace.h>

namespace trunk::mem
{
    struct MapRange
    {
        u64 start_vaddr;
        u64 start_paddr;
        u64 size;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuEarlyInit                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init to setup In boot stage(uses memblock)               *
     ********************************************************************************/
    void MmuEarlyInit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuInit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : The true Initialization function for the memory management unit    *
     ********************************************************************************/
    void MmuInit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuEarlyInitPerCpu                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init for each cpu core                                   *
     ********************************************************************************/
    void MmuEarlyInitPerCpu() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuLoadCr3                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new address space into CR3                                  *
     ********************************************************************************/
    void MmuLoadCr3(const ArchAspace *space) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapPage                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a page In the MMU                                             *
     ********************************************************************************/
    NO_DISCARD bool MmuMapPage(ArchAspace *space, u64 va, u64 pa, u64 flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapPageHuge                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a 2MB huge page in the MMU                                    *
     ********************************************************************************/
    NO_DISCARD bool MmuMapPageHuge(ArchAspace *space, u64 va, u64 pa, u64 flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapMmio                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a memory-mapped I/O region with cache-disable flags           *
     ********************************************************************************/
    NO_DISCARD bool MmuMapMmio(ArchAspace *space, u64 va, u64 pa, usize size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuMapRange                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a range In the MMU                                            *
     ********************************************************************************/
    NO_DISCARD bool MmuMapRange(ArchAspace *space, MapRange range, u64 flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuUnmapPage                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmaps a page In the MMU                                           *
     ********************************************************************************/
    NO_DISCARD bool MmuUnmapPage(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuIsMapped                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if va is mapped and present in the given space.       *
     ********************************************************************************/
    NO_DISCARD bool MmuIsMapped(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuProtect                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Update the flags on an existing page mapping                       *
     ********************************************************************************/
    NO_DISCARD bool MmuProtect(ArchAspace *space, u64 va, u64 new_flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuClearAccessed                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clear the accessed bit on a mapped page                            *
     ********************************************************************************/
    NO_DISCARD bool MmuClearAccessed(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuTranslate                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a page in the mmu                                       *
     ********************************************************************************/
    NO_DISCARD u64 MmuTranslate(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmuQuery                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return the raw PTE value for va                                    *
     ********************************************************************************/
    NO_DISCARD u64 MmuQuery(ArchAspace *space, u64 va) noexcept;

} // namespace trunk::mem