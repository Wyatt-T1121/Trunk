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
 *  MODULE  : Task State Segment                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initializes the Task State Segment                     *
 ********************************************************************************/
#include <cbk/gdt/tss.h>

#include <assert.h>
#include <kmlayout.h>

#include <tklib/math.h>

namespace trunk::gdt
{
    static Tss s_tss{};

    static BYTE s_ist1_stack[IST_STACK_SIZE];
    static BYTE s_ist2_stack[IST_STACK_SIZE];
    static BYTE s_ist3_stack[IST_STACK_SIZE];
    static BYTE s_ist4_stack[IST_STACK_SIZE];
    static BYTE s_ist5_stack[IST_STACK_SIZE];
    static BYTE s_ist6_stack[IST_STACK_SIZE];
    static BYTE s_ist7_stack[IST_STACK_SIZE];

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TssInit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the Task State Segment                                 *
     ********************************************************************************/
    VOID TssInit() noexcept
    {
        s_tss.iopb_offset = sizeof(Tss);
        s_tss.ist[0]      = reinterpret_cast<QWORD>(s_ist1_stack + IST_STACK_SIZE);
        s_tss.ist[1]      = reinterpret_cast<QWORD>(s_ist2_stack + IST_STACK_SIZE);
        s_tss.ist[2]      = reinterpret_cast<QWORD>(s_ist3_stack + IST_STACK_SIZE);
        s_tss.ist[3]      = reinterpret_cast<QWORD>(s_ist4_stack + IST_STACK_SIZE);
        s_tss.ist[4]      = reinterpret_cast<QWORD>(s_ist5_stack + IST_STACK_SIZE);
        s_tss.ist[5]      = reinterpret_cast<QWORD>(s_ist6_stack + IST_STACK_SIZE);
        s_tss.ist[6]      = reinterpret_cast<QWORD>(s_ist7_stack + IST_STACK_SIZE);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TssSetRsp0                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Set the RSP0 field for ring mode                                   *
     ********************************************************************************/
    VOID TssSetRsp0(QWORD rsp) noexcept
    {
        ASSERT(rsp == reinterpret_cast<QWORD>(__stack_top), "RSP0 DOES NOT MATCH KERNEL STACK TOP");
        ASSERT(tklib::math::is_aligned(rsp, 16), "RSP0 IS NOT 16-BYTE ALIGNED");
        s_tss.rsp0 = rsp;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TssGet                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current tss by reference                                   *
     ********************************************************************************/
    NO_DISCARD const Tss &TssGet() noexcept
    {
        return s_tss;
    }
} // namespace trunk::gdt