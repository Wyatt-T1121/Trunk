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

#include <types.h>
#include <macros.h>
#include <assert.h>

namespace trunk::interrupts
{
    struct GNU_PACKED InterruptFrame
    {
        u64 r15, r14, r13, r12, r11, r10, r9, r8;
        u64 rbp, rdi, rsi, rdx, rcx, rbx, rax;

        u64 vector_number;
        u64 error_code;

        u64 rip;
        u64 cs;
        u64 rflags;
        u64 rsp;
        u64 ss;
    };

    STATIC_ASSERT(sizeof(InterruptFrame) == 176, "InterruptFrame structural footprint must be exactly 176 bytes!");

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : kinterrupt_dispatcher                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Takes the interrupt from trap and dispatches It                    *
     ********************************************************************************/
    extern "C" void kinterrupt_dispatcher(InterruptFrame *frame) noexcept;

} // namespace trunk::interrupts