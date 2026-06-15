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
 *  PURPOSE : Populates the 256 gates and executes physical lidt instruction     *
 ********************************************************************************/
#include <trunk/tros/interrupts/idt/idt.h>

extern "C" const u64 g_InterruptVectorTable[256];

namespace trunk::interrupts
{
    static IdtDescriptor g_IdtEntries[256] ALIGNED(16);

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : set_gate                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Sets a new IDT gate with parameters                                *
     ********************************************************************************/
    void set_gate(u8 vector, u64 handler_address, u16 selector, u8 privilege, u8 ist) noexcept
    {
        g_IdtEntries[vector].offset_low = static_cast<u16>(handler_address & 0xFFFF);
        g_IdtEntries[vector].offset_mid = static_cast<u16>((handler_address >> 16) & 0xFFFF);
        g_IdtEntries[vector].offset_high = static_cast<u32>((handler_address >> 32) & 0xFFFFFFFF);

        g_IdtEntries[vector].segment_selector = selector;
        g_IdtEntries[vector].ist_index = ist;
        g_IdtEntries[vector].gate_type = 0xE;
        g_IdtEntries[vector].privilege = privilege;
        g_IdtEntries[vector].present = 1;
        g_IdtEntries[vector].reserved_0 = 0;
        g_IdtEntries[vector].reserved_1 = 0;
        g_IdtEntries[vector].reserved_2 = 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : idt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the interrupt descriptor table                         *
     ********************************************************************************/
    void idt_init() noexcept
    {
        const u16 kernel_code_selector = 0x08;

        for (int i = 0; i < 256; ++i)
            set_gate(static_cast<u8>(i), g_InterruptVectorTable[i], kernel_code_selector, 0, 0);

        IdtrPointer idtr;
        idtr.limit = (sizeof(IdtDescriptor) * 256) - 1;
        idtr.base_address = reinterpret_cast<u64>(&g_IdtEntries);

        asm volatile("lidt %0" : : "m"(idtr));
    }

} // namespace trunk::interrupts