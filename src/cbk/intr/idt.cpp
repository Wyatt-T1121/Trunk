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
 *  PURPOSE : Populates the 256 IDT gates.                                       *
 ********************************************************************************/
#include <cbk/intr/idt.h>

extern "C" CONST QWORD g_InterruptVectorTable[256];

namespace cbk::interrupts
{
    static IdtDescriptor g_IdtEntries[256] ALIGNED(16);

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : SetGate                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Sets a new IDT gate with parameters                                *
     ********************************************************************************/
    VOID SetGate(BYTE vector, QWORD handler_address, WORD selector, BYTE privilege,
                 BYTE ist) noexcept
    {
        g_IdtEntries[vector].offset_low  = static_cast<WORD>(handler_address & 0xFFFF);
        g_IdtEntries[vector].offset_mid  = static_cast<WORD>((handler_address >> 16) & 0xFFFF);
        g_IdtEntries[vector].offset_high = static_cast<DWORD>((handler_address >> 32) & 0xFFFFFFFF);

        g_IdtEntries[vector].segment_selector = selector;
        g_IdtEntries[vector].ist_index        = ist;
        g_IdtEntries[vector].gate_type        = 0xE;
        g_IdtEntries[vector].privilege        = privilege;
        g_IdtEntries[vector].present          = 1;
        g_IdtEntries[vector].reserved_0       = 0;
        g_IdtEntries[vector].reserved_1       = 0;
        g_IdtEntries[vector].reserved_2       = 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IdtInit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the interrupt descriptor table                         *
     ********************************************************************************/
    VOID IdtInit() noexcept
    {
        CONST WORD kernel_code_selector = 0x08;

        for (int i = 0; i < 256; ++i)
            SetGate(static_cast<BYTE>(i), g_InterruptVectorTable[i], kernel_code_selector, 0, 0);

        SetGate(8, g_InterruptVectorTable[8], kernel_code_selector, 0, 1);
        SetGate(2, g_InterruptVectorTable[2], kernel_code_selector, 0, 2);
        SetGate(1, g_InterruptVectorTable[1], kernel_code_selector, 0, 3);
        SetGate(18, g_InterruptVectorTable[18], kernel_code_selector, 0, 4);

        IdtrPointer idtr;
        idtr.limit        = (sizeof(IdtDescriptor) * 256) - 1;
        idtr.base_address = reinterpret_cast<QWORD>(&g_IdtEntries);

        asm volatile("lidt %0" : : "m"(idtr));
    }

} // namespace cbk::interrupts