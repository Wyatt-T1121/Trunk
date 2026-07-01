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
 *  AUTHOR  :  Trollycat                                                         *
 *  MODULE  :  Interrupt subsystem                                               *
 *  DATE    :  2026                                                              *
 *  PURPOSE :  Hardware and software register state during an interrupt (tf)     *
 ********************************************************************************/
#pragma once

#include <types.h>

namespace cbk::interrupts
{
    // clang-format off
    #pragma pack(push, 1)

    struct TrapFrame
    {
        QWORD r15;
        QWORD r14;
        QWORD r13;
        QWORD r12;
        QWORD r11;
        QWORD r10;
        QWORD r9;
        QWORD r8;
        QWORD rbp;
        QWORD rdi;
        QWORD rsi;
        QWORD rdx;
        QWORD rcx;
        QWORD rbx;
        QWORD rax;

        QWORD vector_number;
        QWORD error_code;

        QWORD rip;
        QWORD cs;
        QWORD rflags;
        QWORD rsp;
        QWORD ss;
    };

    #pragma pack(pop)

    // clang-format on
} // namespace cbk::interrupts