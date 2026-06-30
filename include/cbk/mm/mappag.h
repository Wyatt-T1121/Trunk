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
 *  MODULE  : Page mapper                                                        *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Handles all mapping requests of all sizes                          *
 ********************************************************************************/
#pragma once

#include <cbk/mm/mmunit.h>

namespace cbk::mem
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MapPage4K                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the 4 levels, allocates missing sub-tables              *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapPage4K(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : UnmapPage4K                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clears level 1 PTE, TLB cache flush                                *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapPage4K(QWORD virt) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ProtectPage4K                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modify an existing PTE's attribute bits                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS ProtectPage4K(QWORD virt, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapPage2M                                                           *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates at Level 2 PD with PAGE_HUGE set                         *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapPage2M(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapPage2M                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 2 PDE block, executes TLB cache flush                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapPage2M(QWORD virt) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapPage1G                                                           *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates at Level 3 PDPT with PAGE_HUGE set                       *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapPage1G(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapPage1G                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 3 PDPTE block, executes TLB cache flush                *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapPage1G(QWORD virt) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapRange4K                                                          *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous virtual range to a continuous physical range      *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapRange4K(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapRange4K                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 4KB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapRange4K(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapRange2M                                                          *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 2MB pages in one call                    *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapRange2M(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapRange2M                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 2MB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapRange2M(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsRangeFree                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a range of virtual addresses has any existing mappings   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS IsRangeFree(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPagePresent                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a page is present                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS IsPagePresent(QWORD virt) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TranslateVirtualToPhysical                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds what physical frame address is mapped to a virtual pointer   *
     ********************************************************************************/
    NO_DISCARD QWORD TranslateVirtualToPhysical(QWORD virt) noexcept;
} // namespace cbk::mem