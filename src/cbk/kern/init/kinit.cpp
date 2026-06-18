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
 *  MODULE  : Core kernel                                                        *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Kernel entry point (CbkStartup)                                    *
 ********************************************************************************/
#include <cbk/kern/init/kinit.h>
#include <cbk/kern/welcome.h>

#include <cbk/gdt/gdt.h>
#include <cbk/hal/io.h>
#include <cbk/interrupts/idt/idt.h>
#include <drivers/hal/pic.h>

#define STARTUP_FUNC_FLAGS extern "C" NO_RETURN __attribute__((section(".text")))

namespace trunk::kernel
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CbkSetupSubsystems                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Setup all subsystems of the Trunk kernel                           *
     ********************************************************************************/
    void CbkSetupSubsystems() noexcept
    {
        gdt::GdtInit();
        interrupts::IdtInit();
        drivers::pic::PicInit();
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CbkStartup                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Top-level kernel entry.                                            *
     ********************************************************************************/
    STARTUP_FUNC_FLAGS void CbkStartup(const boot::BootInfo &info) noexcept
    {
        CbkSetupSubsystems();
        hal::Sti();

        MUWelcome();

        (void)info;
        for (;;) {
            asm volatile("sti; hlt");
        }
    }
} // namespace trunk::kernel