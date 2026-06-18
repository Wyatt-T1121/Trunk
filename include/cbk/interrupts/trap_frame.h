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
 *                                                                               *
 *  AUTHOR  :  Trollycat                                                         *
 *  MODULE  :  Interrupt subsystem                                               *
 *  DATE    :  2026                                                              *
 *  PURPOSE :  Maps the hardware and software register state on an interrupt     *
 *                                                                  trap         *
 ********************************************************************************/
#pragma once

#include <types.h>

namespace trunk::interrupts
{
    // clang-format off
    #pragma pack(push, 1)

    struct TrapFrame
    {
        u64 r15;
        u64 r14;
        u64 r13;
        u64 r12;
        u64 r11;
        u64 r10;
        u64 r9;
        u64 r8;
        u64 rbp;
        u64 rdi;
        u64 rsi;
        u64 rdx;
        u64 rcx;
        u64 rbx;
        u64 rax;

        u64 vector_number;
        u64 error_code;

        u64 rip;
        u64 cs;
        u64 rflags;
        u64 rsp;
        u64 ss;
    };

    #pragma pack(pop)

    // clang-format on
} // namespace trunk::interrupts