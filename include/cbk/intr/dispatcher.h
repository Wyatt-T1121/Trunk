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
 *  MODULE  : Interrupt subsystem                                                *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Interrupt dispatcher                                               *
 ********************************************************************************/
#pragma once

#include <assert.h>
#include <attributes.h>
#include <types.h>

namespace cbk::interrupts
{
    struct GNU_PACKED InterruptFrame
    {
        QWORD r15, r14, r13, r12, r11, r10, r9, r8;
        QWORD rbp, rdi, rsi, rdx, rcx, rbx, rax;

        QWORD vector_number;
        QWORD error_code;

        QWORD rip;
        QWORD cs;
        QWORD rflags;
        QWORD rsp;
        QWORD ss;
    };

    STATIC_ASSERT(sizeof(InterruptFrame) == 176,
                  "InterruptFrame structural footprint must be exactly 176 bytes!");

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : KeInterruptDispatchHandler                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Takes the interrupt from trap and dispatches It                    *
     ********************************************************************************/
    EXTERN_C VOID
    KeInterruptDispatchHandler(InterruptFrame *frame) noexcept;

} // namespace cbk::interrupts