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
 *  MODULE  : Kernel abortion                                                    *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Halts the kernel on a fatal state, equ to panic().                 *
 ********************************************************************************/
#include <trunk/drivers/serial/serial.h>
#include <trunk/tros/kern/kabort.h>

#include <macros.h>

namespace trunk::kernel
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : kabort                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Halts the kernel forever and prints the message                    *
     ********************************************************************************/
    NO_RETURN void kabort(const char *message) noexcept
    {
        asm volatile("cli");

        drivers::serial::serial_puts(
            "                        KERNEL ABORTED                     \n");
        drivers::serial::serial_puts("STOP_REASON: ");
        drivers::serial::serial_puts(message);

        asm volatile(".lockdown_loop:\n\t"
                     "hlt\n\t"
                     "jmp .lockdown_loop");
        __builtin_unreachable();
    }
} // namespace trunk::kernel