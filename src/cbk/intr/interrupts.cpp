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
 *  PURPOSE : Implements registration array logic and exception translation      *
 ********************************************************************************/
#include <cbk/intr/interrupts.h>

#include <drivers/serial/serial.h>

#include <cbk/bgchk/bug.h>

namespace cbk::interrupts
{
    static RegisteredHandler g_InterruptHandlers[256] = {};

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : KeRegisterInterruptHandler                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Assigns a custom C++ driver function to an IDT slot                *
     ********************************************************************************/
    VOID
    KeRegisterInterruptHandler(BYTE vector, InterruptHandler handler, PVOID context) noexcept
    {
        ASSERT(vector < 256, "VECTOR OUT OF BOUNDS IN REGISTER_INTERRUPT_HANDLER");

        if (g_InterruptHandlers[vector].handler != nullptr && vector >= 32)
            drivers::serial::SerialPuts("WARNING: OVERRIDING INTERRUPT VECTOR\n");

        g_InterruptHandlers[vector].handler = handler;
        g_InterruptHandlers[vector].context = context;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : KeExecuteInterruptHandler                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : called to route interrupts                                         *
     ********************************************************************************/
    VOID
    KeExecuteInterruptHandler(BYTE vector, InterruptFrame *frame) noexcept
    {
        RegisteredHandler target = g_InterruptHandlers[vector];

        if (target.handler != nullptr)
            target.handler(frame, target.context);
        else {
            if (vector < 32)
                kernel::KAbort("KERNEL PANIC!!! UNHANDLED EXCEPTION.");
        }
    }

} // namespace cbk::interrupts