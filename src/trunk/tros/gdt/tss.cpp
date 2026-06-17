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
#include <trunk/tros/gdt/tss.h>

#include <assert.h>
#include <kmlayout.h>

#include <tklib/math.h>

namespace trunk::gdt
{
    static Tss s_tss{};

    static u8 s_ist1_stack[IST_STACK_SIZE];
    static u8 s_ist2_stack[IST_STACK_SIZE];
    static u8 s_ist3_stack[IST_STACK_SIZE];
    static u8 s_ist4_stack[IST_STACK_SIZE];
    static u8 s_ist5_stack[IST_STACK_SIZE];
    static u8 s_ist6_stack[IST_STACK_SIZE];
    static u8 s_ist7_stack[IST_STACK_SIZE];

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tss_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the Task State Segment                                 *
     ********************************************************************************/
    void tss_init() noexcept
    {
        s_tss.iopb_offset = sizeof(Tss);
        s_tss.ist[0]      = reinterpret_cast<u64>(s_ist1_stack + IST_STACK_SIZE);
        s_tss.ist[1]      = reinterpret_cast<u64>(s_ist2_stack + IST_STACK_SIZE);
        s_tss.ist[2]      = reinterpret_cast<u64>(s_ist3_stack + IST_STACK_SIZE);
        s_tss.ist[3]      = reinterpret_cast<u64>(s_ist4_stack + IST_STACK_SIZE);
        s_tss.ist[4]      = reinterpret_cast<u64>(s_ist5_stack + IST_STACK_SIZE);
        s_tss.ist[5]      = reinterpret_cast<u64>(s_ist6_stack + IST_STACK_SIZE);
        s_tss.ist[6]      = reinterpret_cast<u64>(s_ist7_stack + IST_STACK_SIZE);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tss_set_rsp0                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Set the RSP0 field for ring mode                                   *
     ********************************************************************************/
    void tss_set_rsp0(u64 rsp) noexcept
    {
        ASSERT(rsp == reinterpret_cast<u64>(__stack_top), "RSP0 DOES NOT MATCH KERNEL STACK TOP");
        ASSERT(tklib::math::is_aligned(rsp, 16), "RSP0 IS NOT 16-BYTE ALIGNED");
        s_tss.rsp0 = rsp;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tss_get                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current tss by reference                                   *
     ********************************************************************************/
    NO_DISCARD const Tss &tss_get() noexcept
    {
        return s_tss;
    }
} // namespace trunk::gdt