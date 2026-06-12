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
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Interrupt subsystem                                                *
 *  DATE    : 2026                                                               *
 *  PURPOSE :  Populates the 256 gates and executes physical lidt instruction    *
 ********************************************************************************/
#pragma once

#include <types.h>
#include <macros.h>
#include <assert.h>

namespace trunk::interrupts
{
    struct GNU_PACKED IdtDescriptor
    {
        u16 offset_low;
        u16 segment_selector;

        // clang-format off
        u16 ist_index   : 3;
        u16 reserved_0  : 5;
        u16 gate_type   : 4;
        u16 reserved_1  : 1;
        u16 privilege   : 2;
        u16 present     : 1;
        // clang-format on

        u16 offset_mid;
        u32 offset_high;

        u32 reserved_2;
    };

    struct GNU_PACKED IdtrPointer
    {
        u16 limit;
        u64 base_address;
    };

    STATIC_ASSERT(sizeof(IdtDescriptor) == 16, "IdtDescriptor must be exactly 16 bytes!");
    STATIC_ASSERT(sizeof(IdtrPointer) == 10, "IdtrPointer must be exactly 10 bytes!");

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the interrupt descriptor table                          *
     ********************************************************************************/
    void idt_init() noexcept;

} // namespace trunk::interrupts
