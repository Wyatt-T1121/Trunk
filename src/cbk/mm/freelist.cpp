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
 *  PURPOSE : Alloc layer between MMU and Physical allocators                    *
 ********************************************************************************/
#include <cbk/mm/freelist.h>

#include <cbk/mm/pfn.h>

#include <assert.h>

namespace cbk::mem
{
    MMPFN *g_MmPfnDatabase = nullptr;

    PMMPFN mm_pfn_database = nullptr;

    PFN_NUM mm_available_pages            = 0;
    PFN_NUM mm_resident_available_pages   = 0;
    PFN_NUM mm_resident_available_at_init = 0;

    PFN_NUM mm_highest_physical_page = 0;

    SIZE_T mm_total_committed_pages      = 0;
    SIZE_T mm_shared_commit              = 0;
    SIZE_T mm_driver_commit              = 0;
    SIZE_T mm_process_commit             = 0;
    SIZE_T mm_paged_pool                 = 0;
    SIZE_T mm_peak_commitment            = 0;
    SIZE_T mm_total_commit_limit_maximum = 0;

    ListEntry mm_active_user_list;
    ListEntry mm_page_location_list_head[MEM_PFN_STATE_COUNT];

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetLRUFirstUserPage                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Find the least recently used physical memory page                  *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmGetLRUFirstUserPage() noexcept
    {
        if (IsListEmpty(&mm_active_user_list))
            return 0;

        PLIST_ENTRY first_node = mm_active_user_list.flink;
        PMMPFN pfn             = CONTAINING_RECORD(first_node, MMPFN, list_entry);
        PFN_NUM page           = GetPfnEntryIndex(pfn);

        MmReferencePage(page);

        return page;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmInsertLRULastUserPage                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Place a physical memory page at the back of the LRU chain          *
     ********************************************************************************/
    VOID MmInsertLRULastUserPage(PFN_NUM page) noexcept
    {
        PMMPFN pfn = GetPfnEntry(page);

        if (pfn->list_entry.flink != nullptr && pfn->list_entry.blink != nullptr)
            RemoveEntryList(&pfn->list_entry);

        InsertTailList(&mm_active_user_list, &pfn->list_entry);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmRemoveLRUUserPage                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Completely pull a page out of the LRU chain                        *
     ********************************************************************************/
    VOID MmRemoveLRUUserPage(PFN_NUM page) noexcept
    {
        ASSERT(page != 0, "MmRemoveLRUUserPage: page = 0");
        ASSERT_IS_CBK_PFN(page);

        PMMPFN pfn = GetPfnEntry(page);

        if (pfn->list_entry.flink != nullptr && pfn->list_entry.blink != nullptr) {
            RemoveEntryList(&pfn->list_entry);
            pfn->list_entry.flink = nullptr;
            pfn->list_entry.blink = nullptr;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetLRUNextUserPage                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Iterate through pages in order starting at the given page          *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmGetLRUNextUserPage(PFN_NUM prev_page, BOOL move_to_last) noexcept
    {
        PFN_NUM page = 0;

        PMMPFN previous_pfn = GetPfnEntry(prev_page);

        PLIST_ENTRY next_entry = previous_pfn->list_entry.flink;
        PMMPFN next_pfn        = CONTAINING_RECORD(next_entry, MMPFN, list_entry);

        if (move_to_last && MmGetReferenceCountPage(prev_page) > 1) {
            MmRemoveLRUUserPage(prev_page);
            MmInsertLRULastUserPage(prev_page);
        }

        if (next_entry != &mm_active_user_list) {
            page = GetPfnEntryIndex(next_pfn);
            MmReferencePage(page);
        } else
            page = 0;

        MmDereferencePage(prev_page);

        return page;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPfnFree                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check to see if a specific physical page frame is free             *
     ********************************************************************************/
    NO_DISCARD BOOL IsPfnFree(PMMPFN pfn1) noexcept
    {
        return (pfn1->reference_count == 0) &&
               (pfn1->page_location == MM_PFN_STATE::FREE_PAGE_LIST ||
                pfn1->page_location == MM_PFN_STATE::ZEROED_PAGE_LIST);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPfnInUse                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a physical page frame is actively holding data            *
     ********************************************************************************/
    NO_DISCARD BOOL IsPfnInUse(PMMPFN pfn1) noexcept
    {
        return !IsPfnFree(pfn1);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmSetRmapListHeadPage                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Attach or update the tracking list of va that point                *
     *                                                      to a physical page frame *
     ********************************************************************************/
    VOID MmSetRmapListHeadPage(PFN_NUM pfn, PMM_RMAP_ENTRY list_head) noexcept
    {
        ASSERT_IS_CBK_PFN(pfn);

        PMMPFN pfn1 = GetPfnEntry(pfn);
        ASSERT(pfn1 != nullptr, "MmSetRmapListHeadPage: PFN entry pointer is null");

        if (list_head != nullptr)
            ASSERT(IsPfnInUse(pfn1), "MmSetRmapListHeadPage: PFN must be active to assign RMAP");

        pfn1->rmap_list_head = list_head;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetRmapListHeadPage                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Retrieve the head of the linked-list for a specific                *
     *                                                          physical page frame  *
     ********************************************************************************/
    NO_DISCARD PMM_RMAP_ENTRY MmGetRmapListHeadPage(PFN_NUM pfn) noexcept
    {
        ASSERT_IS_CBK_PFN(pfn);
        PMMPFN pfn1 = GetPfnEntry(pfn);
        ASSERT(pfn1 != nullptr, "MmGetRmapListHeadPage: PFN entry pointer is null");
        ASSERT(IsPfnInUse(pfn1), "MmGetRmapListHeadPage: PFN must be active to have RMAP");
        return pfn1->rmap_list_head;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmReferencePage                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Increment reference counter of physical page frame(+1)             *
     ********************************************************************************/
    VOID MmReferencePage(PFN_NUM pfn) noexcept
    {
        ASSERT_IS_CBK_PFN(pfn);
        PMMPFN pfn1 = GetPfnEntry(pfn);
        ASSERT(pfn1 != nullptr, "MmReferencePage: PFN entry pointer is null");
        ASSERT(!IsPfnFree(pfn1), "MmReferencePage: Cannot reference a completely freed page");
        pfn1->reference_count++;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetReferenceCountPage                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read and return the current number of active owners or users a     *
     *                                                       physical page frame has *
     ********************************************************************************/
    NO_DISCARD ULONG MmGetReferenceCountPage(PFN_NUM pfn) noexcept
    {
        ASSERT_IS_CBK_PFN(pfn);
        PMMPFN pfn1 = GetPfnEntry(pfn);
        ASSERT(pfn1 != nullptr, "MmGetReferenceCountPage: PFN entry pointer is null");
        return pfn1->reference_count;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIsPageInUse                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Check if a specific physical page frame number is currently        *
     *                                                  allocated or holding data    *
     ********************************************************************************/
    NO_DISCARD BOOL MmIsPageInUse(PFN_NUM pfn) noexcept
    {
        ASSERT_IS_CBK_PFN(pfn);
        return IsPfnInUse(GetPfnEntry(pfn));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmDereferencePage                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Decrement the reference counter of a physical page frame(-1)       *
     ********************************************************************************/
    VOID MmDereferencePage(PFN_NUM pfn) noexcept
    {
        ASSERT_IS_CBK_PFN(pfn);

        PMMPFN pfn1 = GetPfnEntry(pfn);
        ASSERT(pfn1 != nullptr, "MmDereferencePage: PFN entry pointer is null");

        ASSERT(pfn1->reference_count != 0,
               "MmDereferencePage: Cannot dereference a page with 0 references");

        pfn1->reference_count--;

        if (pfn1->reference_count == 0) {
            MmRemoveLRUUserPage(pfn);
            pfn1->page_location = MM_PFN_STATE::FREE_PAGE_LIST;
            SIZE_T list_index   = static_cast<SIZE_T>(MM_PFN_STATE::FREE_PAGE_LIST);
            InsertTailList(&mm_page_location_list_head[list_index], &pfn1->list_entry);
            mm_available_pages++;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmAllocPage                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate a single physical page frame from memory                  *
     ********************************************************************************/
    NO_DISCARD PFN_NUM MmAllocPage(ULONG type) noexcept
    {
        SIZE_T free_index   = static_cast<SIZE_T>(MM_PFN_STATE::FREE_PAGE_LIST);
        SIZE_T zeroed_index = static_cast<SIZE_T>(MM_PFN_STATE::ZEROED_PAGE_LIST);

        PLIST_ENTRY list_head = &mm_page_location_list_head[free_index];

        if (IsListEmpty(list_head)) {
            list_head = &mm_page_location_list_head[zeroed_index];
            if (IsListEmpty(list_head))
                return 0;
        }

        PLIST_ENTRY node = list_head->flink;
        RemoveEntryList(node);

        PMMPFN pfn1        = CONTAINING_RECORD(node, MmPfn, list_entry);
        PFN_NUM pfn_offset = GetPfnEntryIndex(pfn1);

        pfn1->reference_count = 1;
        pfn1->page_location   = MM_PFN_STATE::ACTIVE_AND_VALID;
        pfn1->rmap_list_head  = nullptr;

        pfn1->list_entry.flink = nullptr;
        pfn1->list_entry.blink = nullptr;

        if (type == static_cast<ULONG>(MC_TYPE::USER))
            MmInsertLRULastUserPage(pfn_offset);

        if (mm_available_pages > 0)
            mm_available_pages--;
        return pfn_offset;
    }

} // namespace cbk::mem