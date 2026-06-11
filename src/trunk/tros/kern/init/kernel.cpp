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
 *  MODULE  : Core kernel                                                        *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Kernel entry point (TrkStartup). Reads like a table of contents —  *
 *            calls subsystem init functions in order, never returns.            *
 *            No logic lives here. If it does, it belongs in a subsystem file.   *
 *                                                                               *
 ********************************************************************************/

#include <trunk/kernel/kernel.h>
#include <trunk/gdt/gdt.h>

#include <trunk/drivers/serial/serial.h>

namespace serial = trunk::drivers::serial;

namespace trunk::kernel
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TrkStartup                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Top-level kernel entry.                                            *
     ********************************************************************************/
    extern "C" [[noreturn]]
    void TrkStartup(const boot::BootInfo &info) noexcept
    {
        serial::serial_init();
        serial::serial_puts("ALERT: TrkStartup() reached");

        (void)info;

        gdt::gdt_init();

        for (;;)
        {
            asm volatile("hlt");
        }
    }
} // namespace trunk::kernel