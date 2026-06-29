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
 *  MODULE  : Freelist allocator                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : High-speed allocation engine                                       *
 ********************************************************************************/
#pragma once

#include <cbk/mm/mmdefs.h>

#include <attributes.h>
#include <types.h>

// The Freelist is a high-speed allocation engine,
// Instead of scanning a massive array or tracking allocations with a  bitmap,
// We use a page routing engine.

// Rather than using a flat global pool, page frames are segmented by lifecycle
// and multiplexed via 'mm_page_location_list_head[]'. Page frame allocation and
// deallocation resolve in O(1) time by pushing and popping
// records directly from their state lanes.

// This design mirrors NT and ReactOS, all credits go to them

namespace cbk::mem
{
    extern PMMPFN mm_pfn_database;

    extern PFN_NUM mm_available_pages;
    extern PFN_NUM mm_resident_available_pages;
    extern PFN_NUM mm_resident_available_at_init;

    extern PFN_NUM mm_highest_physical_page;

    extern SIZE_T mm_total_committed_pages;
    extern SIZE_T mm_shared_commit;
    extern SIZE_T mm_driver_commit;
    extern SIZE_T mm_process_commit;
    extern SIZE_T mm_paged_pool;
    extern SIZE_T mm_peak_commitment;
    extern SIZE_T mm_total_commit_limit_maximum;

    extern ListEntry mm_active_user_list;
    extern ListEntry mm_page_location_list_head[];

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetLRUFirstUserPage                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Find the least recently used physical memory page                  *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmGetLRUFirstUserPage() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmInsertLRULastUserPage                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Place a physical memory page at the back of the LRU chain          *
     ********************************************************************************/
    VOID MmInsertLRULastUserPage(PFN_NUM page) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmRemoveLRUUserPage                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Completely pull a page out of the LRU chain                        *
     ********************************************************************************/
    VOID MmRemoveLRUUserPage(PFN_NUM page) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetLRUNextUserPage                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Iterate through pages in order starting at upfront page            *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmGetLRUNextUserPage(PFN_NUM prev_page, BOOL move_to_last) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPfnFree                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check to see if a specific physical page frame is ready            *
     ********************************************************************************/
    NO_DISCARD BOOL IsPfnFree(PMMPFN pfn1) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPfnInUse                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a physical page frame is actively holding data            *
     ********************************************************************************/
    NO_DISCARD BOOL IsPfnInUse(PMMPFN pfn1) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmSetRmapListHeadPage                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Attach or update the tracking list of va that point                *
     *                                                      to a physical page frame *
     ********************************************************************************/
    VOID MmSetRmapListHeadPage(PFN_NUM pfn, PMM_RMAP_ENTRY list_head) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetRmapListHeadPage                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Retrieve the head of the linked-list for a specific                *
     *                                                          physical page frame  *
     ********************************************************************************/
    NO_DISCARD PMM_RMAP_ENTRY MmGetRmapListHeadPage(PFN_NUM pfn) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmReferencePage                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Increment reference counter of physical page frame(+1)             *
     ********************************************************************************/
    VOID MmReferencePage(PFN_NUM pfn) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetReferenceCountPage                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read and return the current number of active owners or users a     *
     *                                                       physical page frame has *
     ********************************************************************************/
    NO_DISCARD ULONG MmGetReferenceCountPage(PFN_NUM pfn) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIsPageInUse                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a specific physical page frame number is currently        *
     *                                                  allocated or holding data    *
     ********************************************************************************/
    NO_DISCARD BOOL MmIsPageInUse(PFN_NUM pfn) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmDereferencePage                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Decrement the reference counter of a physical page frame(-1)       *
     ********************************************************************************/
    VOID MmDereferencePage(PFN_NUM pfn) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmAllocPage                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate a single physical page frame from memory                  *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmAllocPage(ULONG type) noexcept;

} // namespace cbk::mem