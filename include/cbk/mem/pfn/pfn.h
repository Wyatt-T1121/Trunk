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

#include <assert.h>
#include <macros.h>
#include <types.h>

#include <cbk/kern/kabort.h>

#include <cbk/mem/arch/mmarch.h>
#include <cbk/mem/types/mmtypes.h>
#include <cbk/mem/util/list.h>

#define ASSERT_IS_CBK_PFN(pfn_num)                                                                 \
    ASSERT((pfn_num) != 0 && (pfn_num) <= mm_highest_physical_page, "Invalid PFN provided")

namespace trunk::mem
{
    inline constexpr SIZE_T BUDDY_MAX_ORDER = 11;

    extern PMMPFN g_MmPfnDatabase;

    struct MmPfn
    {
        BYTE order;
        MM_PFN_STATE page_location;
        ListEntry list_entry;
        PMM_RMAP_ENTRY rmap_list_head;
        ULONG reference_count;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : AddrToPfn                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Shift address to Page frame number                                 *
     ********************************************************************************/
    NO_DISCARD inline constexpr QWORD AddrToPfn(QWORD addr) noexcept
    {
        return addr >> PAGE_SHIFT;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PfnToAddr                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Shift Page frame number to address                                 *
     ********************************************************************************/
    NO_DISCARD inline constexpr QWORD PfnToAddr(QWORD pfn) noexcept
    {
        return pfn << PAGE_SHIFT;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : GetPfnEntry                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a raw PFN index into its metadata structure pointer     *
     ********************************************************************************/
    NO_DISCARD inline PMMPFN GetPfnEntry(PFN_NUM pfn_num) noexcept
    {
        return &g_MmPfnDatabase[pfn_num];
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : GetPfnEntryIndex                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a raw PFN index into its metadata structure pointer     *
     ********************************************************************************/
    NO_DISCARD inline PFN_NUM GetPfnEntryIndex(PMMPFN pfn_entry) noexcept
    {
        return (PFN_NUM)(pfn_entry - g_MmPfnDatabase);
    }

} // namespace trunk::mem