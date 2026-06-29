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
 *  MODULE  : Physical memory manager                                            *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Manages the physical memory layer                                  *
 ********************************************************************************/
#pragma once

#include <assert.h>
#include <attributes.h>
#include <types.h>

#include <cbk/mm/mmdefs.h>

#include <cbk/mm/freelist.h>
#include <cbk/mm/memblock.h>

// Physical memory management,
// Split into 3 files..

// freelist
// memblock
// physical

// THIS FILE: physical

// ORDER:
// 1. Set the highest physical page
// 2. Allocate a memory area for the pfn database
// 3. assign both pfn databases to that area
// 4. Loop through pfn database and set state
// 5. Discard of memblock allocator

// So the grand job is to initialize the PFN database

namespace cbk::mem
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmInitializePfnDatabase                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the page frame number database                          *
     ********************************************************************************/
    VOID MmInitializePfnDatabase() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmInitializePhysicalManager                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the physical memory manager                             *
     ********************************************************************************/
    VOID MmInitializePhysicalManager() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetHighestPhysicalPage                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the highest available phys page                                *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmGetHighestPhysicalPage() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetAvailablePageCount                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get available page count                                           *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmGetAvailablePageCount() noexcept;
} // namespace cbk::mem