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
 *  MODULE  : Page alloc                                                         *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Buddy allocator                                                    *
 ********************************************************************************/
#include <cbk/mem/alloc/page_alloc.h>

#include <assert.h>
#include <macros.h>

namespace trunk::mem
{
    PfnAllocatorState g_PfnAllocator{};

    PMMPFN g_MmPfnDatabase = nullptr;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PfnAllocatorInit                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the PFN allocator(buddy)                                *
     ********************************************************************************/
    VOID PfnAllocatorInit(MMPFN *dbMemory, SIZE_T max) noexcept
    {
        ASSERT(dbMemory != nullptr, "PfnAllocatorInit: NULL DATABASE");
        ASSERT(max > 0, "PfnAllocatorInit: ZERO MAX_FRAMES");

        g_PfnAllocator.mm_pfn_database = dbMemory;
        g_MmPfnDatabase                = dbMemory;
        g_PfnAllocator.max_frames      = max;

        for (BYTE i = 0; i < BUDDY_MAX_ORDER; ++i)
            InitializeListHead(&g_PfnAllocator.free_lists[i]);

        for (SIZE_T i = 0; i < max; ++i) {
            g_PfnAllocator.mm_pfn_database[i].order         = 0;
            g_PfnAllocator.mm_pfn_database[i].page_location = MM_PFN_STATE::FREE_PAGE_LIST;

            g_PfnAllocator.mm_pfn_database[i].list_entry.flink = nullptr;
            g_PfnAllocator.mm_pfn_database[i].list_entry.blink = nullptr;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PfnAllocPages                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate pages                                                     *
     ********************************************************************************/
    NO_DISCARD MMPFN *PfnAllocPages(BYTE order) noexcept
    {
        ASSERT(order < BUDDY_MAX_ORDER, "PfnAllocPages: ORDER EXCEEDS BUDDY_MAX_ORDER");

        for (SIZE_T i = order; i < BUDDY_MAX_ORDER; ++i) {
            PLIST_ENTRY list_head = &g_PfnAllocator.free_lists[i];

            if (!(IsListEmpty(list_head))) {
                PLIST_ENTRY allocated_node = list_head->flink;
                RemoveEntryList(allocated_node);

                MMPFN *page          = CONTAINING_RECORD(allocated_node, MMPFN, list_entry);
                SIZE_T current_order = i;

                while (current_order > order) {
                    --current_order;

                    SIZE_T buddy_pfn =
                        (page - g_PfnAllocator.mm_pfn_database) + (SIZE_T{1} << current_order);

                    if (buddy_pfn >= g_PfnAllocator.max_frames)
                        continue;

                    MMPFN *buddy_page = &g_PfnAllocator.mm_pfn_database[buddy_pfn];

                    buddy_page->order         = current_order;
                    buddy_page->page_location = MM_PFN_STATE::FREE_PAGE_LIST;

                    InsertHeadList(&g_PfnAllocator.free_lists[current_order],
                                   &buddy_page->list_entry);
                }

                page->order            = order;
                page->page_location    = MM_PFN_STATE::ACTIVE_AND_VALID;
                page->list_entry.flink = nullptr;
                page->list_entry.blink = nullptr;

                return page;
            }
        }

        return nullptr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PfnFreePages                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Free pages                                                         *
     ********************************************************************************/
    VOID PfnFreePages(MMPFN *page, BYTE order) noexcept
    {
        ASSERT(page != nullptr, "PfnFreePages: NULL PAGE");
        ASSERT(order < BUDDY_MAX_ORDER, "PfnFreePages: ORDER EXCEEDS BUDDY_MAX_ORDER");

        SIZE_T pfn = page - g_PfnAllocator.mm_pfn_database;

        ASSERT(pfn < g_PfnAllocator.max_frames, "PfnFreePages: PFN OUT OF BOUNDS");
        ASSERT(page->page_location == MM_PFN_STATE::ACTIVE_AND_VALID,
               "PfnFreePages: DOUBLE FREE DETECTED");

        while (order < BUDDY_MAX_ORDER - 1) {
            SIZE_T buddy_pfn = pfn ^ (SIZE_T{1} << order);

            if (buddy_pfn >= g_PfnAllocator.max_frames)
                break;

            MMPFN *buddy_page = &g_PfnAllocator.mm_pfn_database[buddy_pfn];

            if (buddy_page->page_location != MM_PFN_STATE::FREE_PAGE_LIST ||
                buddy_page->order != order)
                break;

            RemoveEntryList(&buddy_page->list_entry);

            if (buddy_pfn < pfn) {
                pfn  = buddy_pfn;
                page = buddy_page;
            }

            order++;
        }

        page->order         = order;
        page->page_location = MM_PFN_STATE::FREE_PAGE_LIST;

        InsertHeadList(&g_PfnAllocator.free_lists[order], &page->list_entry);
    }
} // namespace trunk::mem