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

#include <cbk/aspace.h>

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
     *  FUNC    : mmu_early_init                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init to setup In boot stage(uses memblock)               *
     ********************************************************************************/
    void mmu_early_init() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : The true Initialization function for the memory management unit    *
     ********************************************************************************/
    void mmu_init() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_early_init_percpu                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Early MMU init for each cpu core                                   *
     ********************************************************************************/
    void mmu_early_init_percpu() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_load_cr3                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Load a new address space into CR3                                  *
     ********************************************************************************/
    void mmu_load_cr3(const ArchAspace *space) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_page                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a page In the MMU                                             *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_page(ArchAspace *space, u64 va, u64 pa, u64 flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_page_huge                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a 2MB huge page in the MMU                                    *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_page_huge(ArchAspace *space, u64 va, u64 pa, u64 flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_mmio                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a memory-mapped I/O region with cache-disable flags           *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_mmio(ArchAspace *space, u64 va, u64 pa, usize size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_map_range                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps a range In the MMU                                            *
     ********************************************************************************/
    NO_DISCARD bool mmu_map_range(ArchAspace *space, MapRange range, u64 flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_unmap_page                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmaps a page In the MMU                                           *
     ********************************************************************************/
    NO_DISCARD bool mmu_unmap_page(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_is_mapped                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if va is mapped and present in the given space.       *
     ********************************************************************************/
    NO_DISCARD bool mmu_is_mapped(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_protect                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Update the flags on an existing page mapping                       *
     ********************************************************************************/
    NO_DISCARD bool mmu_protect(ArchAspace *space, u64 va, u64 new_flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_clear_accessed                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clear the accessed bit on a mapped page                            *
     ********************************************************************************/
    NO_DISCARD bool mmu_clear_accessed(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_translate                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a page in the mmu                                       *
     ********************************************************************************/
    NO_DISCARD u64 mmu_translate(ArchAspace *space, u64 va) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mmu_query                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return the raw PTE value for va                                    *
     ********************************************************************************/
    NO_DISCARD u64 mmu_query(ArchAspace *space, u64 va) noexcept;

} // namespace trunk::mem