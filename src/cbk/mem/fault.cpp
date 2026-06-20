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
#include <cbk/mem/fault.h>

#include <cbk/hal/io.h>
#include <cbk/kern/kabort.h>

namespace trunk::mem
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : HandlePageFault                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Raw ISR entry hook for Vector 14 (#PF)                             *
     ********************************************************************************/
    VOID HandlePageFault(interrupts::TrapFrame *frame) noexcept
    {
        ULONG_PTR faulting_address = hal::ReadCr2();

        LONG status = MmAccessFault(faulting_address, frame);

        if (status == STATUS_SUCCESS)
            return;

        kernel::KAbort("FATAL_PAGE_FAULT");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmAccessFault                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Evaluates why the CPU faulted                                      *
     ********************************************************************************/
    NO_DISCARD LONG MmAccessFault(ULONG_PTR faulting_address, interrupts::TrapFrame *frame) noexcept
    {
        BOOL is_present = (frame->error_code & 1) != 0;
        BOOL is_write   = (frame->error_code & 2) != 0;
        BOOL is_user    = (frame->error_code & 4) != 0;

        if (!is_user && faulting_address < 0x0000800000000000ULL)
            return STATUS_ACCESS_VIOLATION;

        return STATUS_ACCESS_VIOLATION;
    }

} // namespace trunk::mem