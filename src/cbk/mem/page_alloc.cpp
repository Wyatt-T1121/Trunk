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
#include <cbk/mem/page_alloc.h>

#include <assert.h>
#include <macros.h>

namespace trunk::mem
{
    PfnAllocatorState g_PfnAllocator{};

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pfn_allocator_init                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the PFN allocator(buddy)                                *
     ********************************************************************************/
    void pfn_allocator_init(Page *dbMemory, usize max) noexcept
    {
        ASSERT(dbMemory != nullptr, "pfn_allocator_init: NULL DATABASE");
        ASSERT(max > 0, "pfn_allocator_init: ZERO MAX_FRAMES");

        g_PfnAllocator.mm_pfn_database = dbMemory;
        g_PfnAllocator.max_frames      = max;

        for (u8 i = 0; i < BUDDY_MAX_ORDER; ++i)
            g_PfnAllocator.free_lists[i] = nullptr;

        for (usize i = 0; i < max; ++i) {
            g_PfnAllocator.mm_pfn_database[i].order     = 0;
            g_PfnAllocator.mm_pfn_database[i].is_free   = false;
            g_PfnAllocator.mm_pfn_database[i].node.next = nullptr;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pfn_alloc_pages                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate pages                                                     *
     ********************************************************************************/
    NO_DISCARD Page *pfn_alloc_pages(u8 order) noexcept
    {
        ASSERT(order < BUDDY_MAX_ORDER, "pfn_alloc_pages: ORDER EXCEEDS BUDDY_MAX_ORDER");

        for (usize i = order; i < BUDDY_MAX_ORDER; ++i)
            if (g_PfnAllocator.free_lists[i] != nullptr) {
                FreeAreaNode *allocated_node = g_PfnAllocator.free_lists[i];
                g_PfnAllocator.free_lists[i] = allocated_node->next;

                Page *page = reinterpret_cast<Page *>(reinterpret_cast<uptr>(allocated_node) -
                                                      OFFSET_OF(Page, node));

                usize current_order = i;

                while (current_order > order) {
                    --current_order;

                    usize buddy_pfn =
                        (page - g_PfnAllocator.mm_pfn_database) + (usize{1} << current_order);

                    if (buddy_pfn >= g_PfnAllocator.max_frames)
                        continue;

                    Page *buddy_page = &g_PfnAllocator.mm_pfn_database[buddy_pfn];

                    buddy_page->order   = current_order;
                    buddy_page->is_free = true;

                    buddy_page->node.next = g_PfnAllocator.free_lists[current_order];
                    g_PfnAllocator.free_lists[current_order] = &buddy_page->node;
                }

                page->order     = order;
                page->is_free   = false;
                page->node.next = nullptr;

                return page;
            }

        return nullptr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pfn_free_pages                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Free pages                                                         *
     ********************************************************************************/
    void pfn_free_pages(Page *page, u8 order) noexcept
    {
        ASSERT(page != nullptr, "pfn_free_pages: NULL PAGE");
        ASSERT(order < BUDDY_MAX_ORDER, "pfn_free_pages: ORDER EXCEEDS BUDDY_MAX_ORDER");

        usize pfn = page - g_PfnAllocator.mm_pfn_database;

        ASSERT(pfn < g_PfnAllocator.max_frames, "pfn_free_pages: PFN OUT OF BOUNDS");
        ASSERT(!page->is_free, "pfn_free_pages: DOUBLE FREE DETECTED");

        while (order < BUDDY_MAX_ORDER - 1) {
            usize buddy_pfn = pfn ^ (usize{1} << order);

            if (buddy_pfn >= g_PfnAllocator.max_frames)
                break;

            Page *buddy_page = &g_PfnAllocator.mm_pfn_database[buddy_pfn];

            if (!buddy_page->is_free || buddy_page->order != order)
                break;

            FreeAreaNode **prev = &g_PfnAllocator.free_lists[order];

            while (*prev != nullptr && *prev != &buddy_page->node)
                prev = &((*prev)->next);
            if (*prev == &buddy_page->node)
                *prev = buddy_page->node.next;

            if (buddy_pfn < pfn) {
                pfn  = buddy_pfn;
                page = buddy_page;
            }

            order++;
        }

        page->order   = order;
        page->is_free = true;

        page->node.next                  = g_PfnAllocator.free_lists[order];
        g_PfnAllocator.free_lists[order] = &page->node;
    }
} // namespace trunk::mem