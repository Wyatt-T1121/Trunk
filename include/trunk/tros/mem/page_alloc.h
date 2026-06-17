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
 *  MODULE  : Page alloc                                                         *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Buddy allocator                                                    *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

#include <trunk/tros/mem/pfn.h>

namespace trunk::mem
{
    struct PfnAllocatorState
    {
        Page *mm_pfn_database;
        usize max_frames;
        FreeAreaNode *free_lists[BUDDY_MAX_ORDER];
    };

    extern PfnAllocatorState g_PfnAllocator;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pfn_allocator_init                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the PFN allocator(buddy)                                *
     ********************************************************************************/
    void pfn_allocator_init(Page *dbMemory, usize max) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pfn_alloc_pages                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocate pages                                                     *
     ********************************************************************************/
    NO_DISCARD Page *pfn_alloc_pages(u8 order) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pfn_free_pages                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Free pages                                                         *
     ********************************************************************************/
    void pfn_free_pages(Page *page, u8 order) noexcept;
} // namespace trunk::mem