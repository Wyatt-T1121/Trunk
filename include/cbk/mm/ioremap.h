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
 *  MODULE  : INPUT/OUTPUT REMAPPING                                             *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Memory-mapped INPUT/OUTPUT configurations                          *
 ********************************************************************************/
#pragma once

#include <attributes.h>
#include <status.h>
#include <types.h>

#include <cbk/mm/mappag.h>
#include <cbk/mm/mmdefs.h>
#include <cbk/mm/vad.h>

namespace cbk::mem
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIoRemap                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps physical hardware registers into virtual space                *
     ********************************************************************************/
    NO_DISCARD PVOID
    MmIoRemap(PMM_ADDRESS_SPACE address_space,
              QWORD phys_addr,
              SIZE_T size,
              QWORD flags,
              PMMVAD blank_node) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIoUnRemap                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmaps previously allocated hardware address range                 *
     ********************************************************************************/
    VOID
    MmIoUnRemap(PMM_ADDRESS_SPACE address_space, PVOID virt_addr, SIZE_T size) noexcept;
} // namespace cbk::mem