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
 *  MODULE  : Memory types                                                       *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores common memory types and handlers                            *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

namespace trunk::mem
{
    inline constexpr SIZE_T MM_PFN_STATE_COUNT = 7;

    // TODO: THIS IS A FORWARD DECL, NOT DEFINED YET.
    // TO BE DEFINED DURING PS.
    struct EProcess;
    using PEPROCESS = EProcess *;

    struct ListEntry;
    using LIST_ENTRY  = ListEntry;
    using PLIST_ENTRY = ListEntry *;

    struct MmRmapEntry;
    using PMM_RMAP_ENTRY = MmRmapEntry *;

    struct MmPfn;
    using MMPFN   = MmPfn;
    using PMMPFN  = MmPfn *;
    using PPMMPFN = MmPfn **;

    using PFN_NUM = QWORD;

    enum class MM_PFN_STATE : BYTE
    {
        ZEROED_PAGE_LIST = 0,
        FREE_PAGE_LIST   = 1,
        ACTIVE_AND_VALID = 6
    };

    enum class MC_TYPE : ULONG
    {
        SYSTEM     = 1,
        USER       = 2,
        NPPOOL     = 3,
        PPOOL      = 4,
        CACHE      = 5,
        CONTIGUOUS = 6
    };
} // namespace trunk::mem