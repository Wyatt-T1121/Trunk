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
 *  MODULE  : Non-cached manager                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : High-level Non-Cached(NCACHE) manager                              *
 ********************************************************************************/
#pragma once

#include <assert.h>
#include <attributes.h>
#include <status.h>
#include <types.h>

#include <cbk/mm/freelist.h>
#include <cbk/mm/mappag.h>
#include <cbk/mm/mmdefs.h>
#include <cbk/mm/virtual.h>

namespace cbk::mem
{
    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmNcacheAllocateBuffer                                              *
     * DATE    : 2026                                                                *
     * PURPOSE : Reserves virtual space via VMM and wires uncached physical entries  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmNcacheAllocateBuffer(PMM_ADDRESS_SPACE address_space,
                           SIZE_T size,
                           QWORD tag,
                           PNCACHE_DESCRIPTOR out_desc) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmNcacheFreeBuffer                                                  *
     * DATE    : 2026                                                                *
     * PURPOSE : Tears down translations, releases frames, and frees VMM region      *
     ********************************************************************************/
    VOID
    MmNcacheFreeBuffer(PMM_ADDRESS_SPACE address_space, PNCACHE_DESCRIPTOR descriptor) noexcept;
} // namespace cbk::mem