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

namespace cbk::mem
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : NcacheAllocateBuffer                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocates a new non-cached continuous buffer                       *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS NcacheAllocateBuffer(SIZE_T size, QWORD tag,
                                              PNCACHE_DESCRIPTOR out_desc) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : NcacheFreeBuffer                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Restores cache state and releases frames back to PMM               *
     ********************************************************************************/
    VOID NcacheFreeBuffer(PNCACHE_DESCRIPTOR descriptor) noexcept;
} // namespace cbk::mem