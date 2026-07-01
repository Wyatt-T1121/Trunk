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
     *  FUNC    : MmMapPage4K                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the 4 levels, allocates missing sub-tables              *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapPage4K(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmUnmapPage4K                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clears level 1 PTE, TLB cache flush                                *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapPage4K(QWORD virt) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmProtectPage4K                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modify an existing PTE's attribute bits                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmProtectPage4K(QWORD virt, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapPage2M                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates at Level 2 PD with PAGE_HUGE set                         *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapPage2M(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapPage2M                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 2 PDE block, executes TLB cache flush                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapPage2M(QWORD virt) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapPage1G                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates at Level 3 PDPT with PAGE_HUGE set                       *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapPage1G(QWORD virt, QWORD phys, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapPage1G                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 3 PDPTE block, executes TLB cache flush                *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapPage1G(QWORD virt) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapRange4K                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous virtual range to a continuous physical range      *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapRange4K(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapRange4K                                                      *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 4KB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapRange4K(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapRange2M                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 2MB pages in one call                    *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapRange2M(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapRange2M                                                      *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 2MB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapRange2M(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIsRangeFree                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a range of virtual addresses has any existing mappings   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmIsRangeFree(QWORD start, SIZE_T size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIsPagePresent                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a page is present                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmIsPagePresent(QWORD virt) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmTranslateVirtualToPhysical                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds what physical frame address is mapped to a virtual pointer   *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmTranslateVirtualToPhysical(QWORD virt) noexcept;
} // namespace cbk::mem