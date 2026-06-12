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
 *  MODULE  : Architecture Initialization                                        *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initalizes the permanent 64-bit                        *
 *            Global Descriptor Table.                                           *
 *                                                                               *
 ********************************************************************************/
#include <trunk/tros/kern/gdt/gdt.h>

namespace trunk::gdt
{
    static GdtEntry gdt[5];
    static GdtPointer gdt_pointer;

    constexpr u8 GDT_PRESENT = 0x80;
    constexpr u8 GDT_RING0 = 0x00;
    constexpr u8 GDT_RING3 = 0x60;
    constexpr u8 GDT_SYSTEM = 0x10;
    constexpr u8 GDT_EXECUTABLE = 0x08;
    constexpr u8 GDT_READ_WRITE = 0x02;
    constexpr u8 GDT_LONG_MODE = 0x20;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_create_entries                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Creates standard GDT entries                                       *
     ********************************************************************************/
    void gdt_create_entries() noexcept
    {
        gdt[0] = GdtEntry{0, 0, 0, 0, 0, 0};

        // Kernel Code
        gdt[1] = GdtEntry::create(GDT_PRESENT | GDT_RING0 | GDT_SYSTEM | GDT_EXECUTABLE | GDT_READ_WRITE, GDT_LONG_MODE);

        // Kernel Data
        gdt[2] = GdtEntry::create(GDT_PRESENT | GDT_RING0 | GDT_SYSTEM | GDT_READ_WRITE, 0);

        // User Code
        gdt[3] = GdtEntry::create(GDT_PRESENT | GDT_RING3 | GDT_SYSTEM | GDT_EXECUTABLE | GDT_READ_WRITE, GDT_LONG_MODE);

        // User Data
        gdt[4] = GdtEntry::create(GDT_PRESENT | GDT_RING3 | GDT_SYSTEM | GDT_READ_WRITE, 0);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the global descriptor table subsystem                  *
     ********************************************************************************/
    void gdt_init() noexcept
    {
        gdt_create_entries();

        gdt_pointer.limit = sizeof(gdt) - 1;
        gdt_pointer.base = reinterpret_cast<uptr>(&gdt);

        gdt_flush(reinterpret_cast<uptr>(&gdt_pointer));
    }
} // namespace trunk::gdt