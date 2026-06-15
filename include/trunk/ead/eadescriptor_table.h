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
 *  MODULE  : Struct definitions                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores the GDT, IDT, and TSS structures                            *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

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

struct GNU_PACKED TssDescriptor
{
    u16 limit_low;
    u16 base_low;
    u8 base_middle;

    u8 type : 4;
    u8 zero : 1;
    u8 dpl : 2;
    u8 p : 1;

    u8 limit_high : 4;
    u8 avl : 1;
    u8 l : 1;
    u8 db : 1;
    u8 g : 1;

    u8 base_high;
    u32 base_upper;
    u32 reserved;
};

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

struct GNU_PACKED GdtLayout
{
    GdtEntry null_desc;
    GdtEntry kernel_code;
    GdtEntry kernel_data;
    GdtEntry user_code;
    GdtEntry user_data;
    TssDescriptor tss_desc;
};

struct GNU_PACKED GdtPointer
{
    u16 limit;
    uptr base;
};

struct GNU_PACKED IdtDescriptor
{
    u16 offset_low;
    u16 segment_selector;

    // clang-format off
        u16 ist_index   : 3;
        u16 reserved_0  : 5;
        u16 gate_type   : 4;
        u16 reserved_1  : 1;
        u16 privilege   : 2;
        u16 present     : 1;
    // clang-format on

    u16 offset_mid;
    u32 offset_high;

    u32 reserved_2;
};

struct GNU_PACKED IdtrPointer
{
    u16 limit;
    u64 base_address;
};
