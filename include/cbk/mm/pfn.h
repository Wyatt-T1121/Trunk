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
#include <attributes.h>
#include <types.h>

#include <cbk/bgchk/bug.h>
#include <cbk/mm/mmdefs.h>

namespace cbk::mem
{
    extern PMMPFN g_MmPfnDatabase;

    struct MmPfn
    {
        MM_PFN_STATE page_location;
        ListEntry list_entry;
        PMM_RMAP_ENTRY rmap_list_head;
        ULONG reference_count;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetPfnFromAddress                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Shift address to Page frame number                                 *
     ********************************************************************************/
    NO_DISCARD INLINE_CONST QWORD
    MmGetPfnFromAddress(QWORD addr) noexcept
    {
        return addr >> PAGE_SHIFT;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetVirtualAddressFromPfn                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Shift Page frame number to address                                 *
     ********************************************************************************/
    NO_DISCARD INLINE_CONST QWORD
    MmGetVirtualAddressFromPfn(QWORD pfn) noexcept
    {
        return pfn << PAGE_SHIFT;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetPfnEntry                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a raw PFN index into its metadata structure pointer     *
     ********************************************************************************/
    NO_DISCARD INLINE PMMPFN
    MmGetPfnEntry(PFN_NUM pfn_num) noexcept
    {
        return &g_MmPfnDatabase[pfn_num];
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetPfnEntryIndex                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Translates a raw PFN index into its metadata structure pointer     *
     ********************************************************************************/
    NO_DISCARD INLINE PFN_NUM
    MmGetPfnEntryIndex(PMMPFN pfn_entry) noexcept
    {
        return (PFN_NUM)(pfn_entry - g_MmPfnDatabase);
    }

} // namespace cbk::mem