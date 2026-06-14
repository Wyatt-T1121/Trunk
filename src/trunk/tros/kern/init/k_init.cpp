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
 *  PURPOSE : Kernel entry point (TrkStartup)                                    *
 ********************************************************************************/
#include <trunk/tros/kern/init/k_init.h>

#include <trunk/asi/io.h>

#include <trunk/drivers/serial/serial.h>
#include <trunk/drivers/hal/pic.h>

#include <trunk/tros/gdt/gdt.h>
#include <trunk/tros/interrupts/idt/idt.h>

namespace serial = trunk::drivers::serial;

#define STARTUP_FUNC_FLAGS extern "C" [[noreturn]] __attribute__((section(".text")))

namespace trunk::kernel
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TrkSetupSubsystems                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Setup all subsystems of the Trunk kernel                           *
     ********************************************************************************/
    void TrkSetupSubsystems() noexcept
    {
        gdt::gdt_init();
        interrupts::idt_init();
        drivers::pic::pic_init();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TrkStartup                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Top-level kernel entry.                                            *
     ********************************************************************************/
    STARTUP_FUNC_FLAGS void
    TrkStartup(const boot::BootInfo &info) noexcept
    {
        TrkSetupSubsystems();
        asi::sti();

        (void)info;
        for (;;)
        {
            asm volatile("cli; hlt");
        }
    }
} // namespace trunk::kernel