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
 *                                                                               *
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Memory block                                                       *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Memory allocator for early boot stage.                             *
 ********************************************************************************/
#pragma once

#include <assert.h>
#include <macros.h>
#include <types.h>

#include <boot/trldr/mb2/boot.h>

namespace trunk::mem
{
    inline constexpr usize MAX_MEMBLOCK_REGIONS = 128;

    struct GNU_PACKED MemoryRegion
    {
        u64 base;
        u64 size;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_init                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialization function for memblock                               *
     ********************************************************************************/
    void memblock_init(const boot::BootInfo &boot_info) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_alloc                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate a new chunk inside the memblock region                    *
     ********************************************************************************/
    NO_DISCARD u64 memblock_alloc(u64 size, u64 alignment) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_reserve                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reserve a region inside the memblock                               *
     ********************************************************************************/
    void memblock_reserve(u64 base, u64 size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_is_reserved                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if any byte in [base, base + size) is reserved.       *
     ********************************************************************************/
    NO_DISCARD bool memblock_is_reserved(u64 base, u64 size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_total_free                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total free bytes remaining in the memory pool.             *
     ********************************************************************************/
    NO_DISCARD u64 memblock_total_free() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_total_reserved                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total reserved bytes across all reserved regions.          *
     ********************************************************************************/
    NO_DISCARD u64 memblock_total_reserved() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_get_region_count                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current region count.                                      *
     ********************************************************************************/
    NO_DISCARD usize memblock_get_region_count() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : memblock_get_region                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get a region at the passed in index.                               *
     ********************************************************************************/
    NO_DISCARD MemoryRegion memblock_get_region(usize index) noexcept;

} // namespace trunk::mem