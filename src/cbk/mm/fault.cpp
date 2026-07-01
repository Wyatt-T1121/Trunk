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
#include <cbk/mm/fault.h>

#include <cbk/bgchk/bug.h>
#include <cbk/hal/io.h>

// Instead of a page fault being thrown, it calls this
// Because we don't have any checks, it currently will just fail
// And since there's no way to pass variables into uart logs, this technically doesn't work
// It will just print 'PAGE_FAULT', nothing more.

// Once we get UART formatting working, this will be a lot better.
// It will make it easier to track memory bugs.

// Also instead of panicing, it will try to recover the system.

// MISSING FEATURES:
//      INFORMATION ABOUT PAGE FAULT
//      ATTEMPTING TO RECOVER INSTEAD OF INSTANT PANIC

namespace cbk::mem
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : KiPageFault                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Raw ISR entry hook for Vector 14 (#PF)                             *
     ********************************************************************************/
    VOID
    KiPageFault(interrupts::InterruptFrame *frame, MAYBE_UNUSED PVOID context) noexcept
    {
        ULONG_PTR faulting_address = hal::ReadCr2();
        CBKSTATUS status           = MmAccessFault(faulting_address, frame);
        ASSERT(status == STATUS_SUCCESS, "FATAL_PAGE_FAULT");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmAccessFault                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Evaluates why the CPU faulted                                      *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmAccessFault(ULONG_PTR faulting_address, interrupts::InterruptFrame *frame) noexcept
    {
        MAYBE_UNUSED BOOL is_present = (frame->error_code & 1) != 0;
        MAYBE_UNUSED BOOL is_write   = (frame->error_code & 2) != 0;
        MAYBE_UNUSED BOOL is_user    = (frame->error_code & 4) != 0;

        if (!is_user && faulting_address < 0x0000800000000000ULL)
            return STATUS_ACCESS_VIOLATION;

        return STATUS_ACCESS_VIOLATION;
    }

} // namespace cbk::mem