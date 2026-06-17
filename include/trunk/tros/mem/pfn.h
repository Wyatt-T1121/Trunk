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
 *  MODULE  : Page frame number                                                  *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Assigns each page(4kb) a unique number for easy tracking.          *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

#include <trunk/tros/mem/page.h>

namespace trunk::mem
{
    inline constexpr usize BUDDY_MAX_ORDER = 11;

    struct FreeAreaNode
    {
        FreeAreaNode *next;
    };

    struct Page
    {
        u8 order;
        bool is_free;
        FreeAreaNode node;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : addr_to_pfn                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Shift address to Page frame number                                 *
     ********************************************************************************/
    NO_DISCARD inline constexpr u64 addr_to_pfn(u64 addr) noexcept
    {
        return addr >> PAGE_SHIFT;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pfn_to_addr                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Shift Page frame number to address                                 *
     ********************************************************************************/
    NO_DISCARD inline constexpr u64 pfn_to_addr(u64 pfn) noexcept
    {
        return pfn << PAGE_SHIFT;
    }
} // namespace trunk::mem