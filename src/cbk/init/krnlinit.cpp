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
#include <cbk/init/krnlinit.h>

#include <cbk/gdt/gdt.h>
#include <cbk/intr/idt.h>

#include <cbk/mm/fault.h>

#include <drivers/hal/pic.h>

#include <cbk/hal/io.h>

namespace cbk::kernel
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CbkSetupSubsystems                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Setup all subsystems of the Trunk kernel                           *
     ********************************************************************************/
    VOID
    CbkSetupSubsystems(const boot::BootInfo &info) noexcept
    {
        gdt::GdtInit();
        interrupts::IdtInit();

        CBKSTATUS status = drivers::pic::PicInit();
        ASSERT(status == STATUS_SUCCESS, "CbkSetupSubsystems: Failed to init PIC");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CbkStartup                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Top-level kernel entry.                                            *
     ********************************************************************************/

    // EXTERN_C - Called as a C function to avoid garabge name
    // NO_RETURN - Does not return, infinite halt
    // TEXT_SECTION - Force this code into the .text section to assure it works
    EXTERN_C
    NO_RETURN
    TEXT_SECTION
    VOID
    CbkStartup(const boot::BootInfo &info) noexcept
    {
        CbkSetupSubsystems(info);

        hal::Sti();
        interrupts::RegisterInterruptHandler(14, mem::KiPageFault);

        (VOID) info;
        for (;;)
            asm volatile("sti; hlt");
    }
} // namespace cbk::kernel