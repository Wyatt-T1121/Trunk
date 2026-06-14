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
 *  MODULE  : Task State Segment                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initializes the Task State Segment                     *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

namespace trunk::gdt
{
    inline constexpr u32 IST_STACK_SIZE = 4096;

    struct GNU_PACKED Tss
    {
        u32 reserved0;
        u64 rsp0;
        u64 rsp1;
        u64 rsp2;
        u64 reserved1;
        u64 ist[7];
        u64 reserved2;
        u16 reserved3;
        u16 iopb_offset;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tss_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the Task State Segment                                 *
     ********************************************************************************/
    void tss_init() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tss_set_rsp0                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Set the RSP0 field for ring mode                                   *
     ********************************************************************************/
    void tss_set_rsp0(u64 rsp) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : tss_get                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current tss by reference                                   *
     ********************************************************************************/
    [[nodiscard]] const Tss &tss_get() noexcept;
} // namespace trunk::gdt