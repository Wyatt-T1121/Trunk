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
 *  MODULE  : Global Descriptor Table                                            *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initalizes the permanent 64-bit                        *
 *            Global Descriptor Table.                                           *
 *                                                                               *
 ********************************************************************************/

#pragma once

#include <trunk/tros/gdt/tss.h>
#include <trunk/ead/eadescriptor_table.h>

#include <types.h>
#include <macros.h>

namespace trunk::gdt
{
    inline constexpr u8 GDT_PRESENT = 0x80;
    inline constexpr u8 GDT_RING0 = 0x00;
    inline constexpr u8 GDT_RING3 = 0x60;
    inline constexpr u8 GDT_SYSTEM = 0x10;
    inline constexpr u8 GDT_EXECUTABLE = 0x08;
    inline constexpr u8 GDT_READ_WRITE = 0x02;
    inline constexpr u8 GDT_LONG_MODE = 0x20;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the global descriptor table                            *
     ********************************************************************************/
    void gdt_init() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_flush                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flushes/Reloads the global descriptor table (external assembly)    *
     ********************************************************************************/
    extern "C" void gdt_flush(uptr gdt_ptr_addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_install_tss                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Installs the TSS                                                   *
     ********************************************************************************/
    [[nodiscard]] u16 gdt_install_tss(const Tss *tss_ptr) noexcept;

} // namespace trunk::gdt
