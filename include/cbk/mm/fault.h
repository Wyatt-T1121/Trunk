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
 *  MODULE  : Page fault handler                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Page fault handler, registered to interrupt #14                    *
 ********************************************************************************/
#pragma once

#include <cbk/intr/interrupts.h>
#include <cbk/intr/trap_frame.h>

#include <attributes.h>
#include <status.h>
#include <types.h>

namespace cbk::mem
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : KiPageFault                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Raw ISR entry hook for Vector 14 (#PF)                             *
     ********************************************************************************/
    VOID
    KiPageFault(interrupts::InterruptFrame *frame, MAYBE_UNUSED PVOID context) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmAccessFault                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Evaluates why the CPU faulted                                      *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmAccessFault(ULONG_PTR faulting_address, interrupts::InterruptFrame *frame) noexcept;

} // namespace cbk::mem