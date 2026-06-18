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
 *  MODULE  : Global Descriptor Table                                            *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initalizes the Global Descriptor Table.                *
 ********************************************************************************/
#pragma once

#include <cbk/desc/descriptor.h>

#include <macros.h>
#include <types.h>

namespace trunk::gdt
{
    inline constexpr BYTE GDT_PRESENT    = 0x80;
    inline constexpr BYTE GDT_RING0      = 0x00;
    inline constexpr BYTE GDT_RING3      = 0x60;
    inline constexpr BYTE GDT_SYSTEM     = 0x10;
    inline constexpr BYTE GDT_EXECUTABLE = 0x08;
    inline constexpr BYTE GDT_READ_WRITE = 0x02;
    inline constexpr BYTE GDT_LONG_MODE  = 0x20;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the global descriptor table                            *
     ********************************************************************************/
    VOID GdtInit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : GdtFlush                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flushes/Reloads the global descriptor table (external assembly)    *
     ********************************************************************************/
    extern "C" VOID GdtFlush(ULONG_PTR gdt_ptr_addr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : GdtInstallTss                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Installs the TSS                                                   *
     ********************************************************************************/
    NO_DISCARD WORD GdtInstallTss(const Tss *tss_ptr) noexcept;

} // namespace trunk::gdt
