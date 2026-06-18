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
    inline constexpr SIZE_T MAX_MEMBLOCK_REGIONS = 128;

    struct GNU_PACKED MemoryRegion
    {
        QWORD base;
        QWORD size;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockInit                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialization function for memblock                               *
     ********************************************************************************/
    VOID MemblockInit(const boot::BootInfo &boot_info) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockAlloc                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate a new chunk inside the memblock region                    *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockAlloc(QWORD size, QWORD alignment) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockReserve                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reserve a region inside the memblock                               *
     ********************************************************************************/
    VOID MemblockReserve(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockIsReserved                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if any byte in [base, base + size) is reserved.       *
     ********************************************************************************/
    NO_DISCARD BOOL MemblockIsReserved(QWORD base, QWORD size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockTotalFree                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total free bytes remaining in the memory pool.             *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockTotalFree() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockTotalReserved                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns total reserved bytes across all reserved regions.          *
     ********************************************************************************/
    NO_DISCARD QWORD MemblockTotalReserved() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockGetRegionCount                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current region count.                                      *
     ********************************************************************************/
    NO_DISCARD SIZE_T MemblockGetRegionCount() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemblockGetRegion                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get a region at the passed in index.                               *
     ********************************************************************************/
    NO_DISCARD MemoryRegion MemblockGetRegion(SIZE_T index) noexcept;

} // namespace trunk::mem