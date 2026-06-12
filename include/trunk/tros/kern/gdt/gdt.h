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

#pragma once

#include <types.h>
#include <macros.h>

namespace trunk::gdt
{
    struct GNU_PACKED GdtEntry
    {
        u16 limit_low;
        u16 base_low;
        u8 base_middle;
        u8 access;
        u8 flags_limit_high;
        u8 base_high;

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : create                                                             *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Creates a new GdtEntry with passed in paramaters                   *
         ********************************************************************************/

        static constexpr GdtEntry create(u8 access, u8 flags) noexcept
        {
            return GdtEntry{
                .limit_low = 0,
                .base_low = 0,
                .base_middle = 0,
                .access = access,
                .flags_limit_high = static_cast<u8>((flags & 0xF0)),
                .base_high = 0};
        }
    };

    struct GNU_PACKED GdtPointer
    {
        u16 limit;
        uptr base;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the global descriptor table subsystem                  *
     ********************************************************************************/
    void gdt_init() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_flush                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Assembly function that flushes the global descriptor table         *
     ********************************************************************************/
    extern "C" void gdt_flush(uptr gdt_ptr_addr) noexcept;

} // namespace trunk::gdt
