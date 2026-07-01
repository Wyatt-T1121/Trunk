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
#include <cbk/mm/physical.h>

namespace cbk::mem
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmInitializePfnDatabase                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the page frame number database                          *
     ********************************************************************************/
    VOID
    MmInitializePfnDatabase() noexcept
    {
        // Highest physical ram address available
        QWORD ram_address_high = MmMemblockGetEndOfDRam();
        ASSERT(ram_address_high != PHYS_ADDR_MAX,
               "MMInitializePfnDatabase: RAM_ADDRESS_HIGH FAILURE");

        // Calculate the highest physical page
        // ram_address_high - converted to a page frame number
        mm_highest_physical_page = MmGetPfnFromAddress(ram_address_high);

        // Size of our database area
        SIZE_T db_size = mm_highest_physical_page * sizeof(MMPFN);

        // Allocate an area in memory for our PFN datbases
        // SIZE: db_size
        // ALIGN: PAGE_SIZE

        // Because we need this for our memory management system to work,
        // Panic if it fails to allocate an area
        QWORD alloc_paddr = MmMemblockAllocateOrCrashKernel(db_size, PAGE_SIZE);

        // Map our databases to the memory area we just allocated
        g_MmPfnDatabase = reinterpret_cast<PMMPFN>(PaddrToKvaddr(alloc_paddr));
        mm_pfn_database = g_MmPfnDatabase;

        MmMemblockFreeAll();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmInitializePhysicalManager                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the physical memory manager                             *
     ********************************************************************************/
    VOID
    MmInitializePhysicalManager() noexcept
    {
        MmInitializePfnDatabase();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetHighestPhysicalPage                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the highest available phys page                                *
     ********************************************************************************/
    NO_DISCARD PFN_NUM
    MmGetHighestPhysicalPage() noexcept
    {
        return mm_highest_physical_page;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetAvailablePageCount                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get available page count                                           *
     ********************************************************************************/
    NO_DISCARD PFN_NUM
    MmGetAvailablePageCount() noexcept
    {
        return mm_available_pages;
    }
} // namespace cbk::mem