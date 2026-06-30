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
 *  MODULE  : RTL Bitmap                                                         *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Bitmap helpers for the virtual memory manager                      *
 ********************************************************************************/
#pragma once

#include <attributes.h>
#include <types.h>

#include <tklib/ubuiltin.h>

// BITMAP HELPERS
// 0 / 1
// 0 = FREE
// 1 = RESERVED

namespace tklib
{
    constexpr ULONG RTL_BITMAP_FAILED = 0xFFFFFFFFUL;

    struct RtlBitmap
    {
        ULONG size_of_bitmap;
        PULONG buffer;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlInitializeBitMap                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the bitmap struct with an existin block of memory       *
     ********************************************************************************/
    VOID RtlInitializeBitMap(PRTL_BITMAP bitmap_header, PULONG bitmap_buffer,
                             ULONG size_of_bitmap) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlFindClearBitsAndSet                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Scans the bitmap for clear bits and marks them as 1s               *
     ********************************************************************************/
    NO_DISCARD ULONG RtlFindClearBitsAndSet(PRTL_BITMAP bitmap_header, ULONG number_to_find,
                                            ULONG hint_index) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlClearBits                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Changes a range of bits back to 0                                  *
     ********************************************************************************/
    VOID RtlClearBits(PRTL_BITMAP bitmap_header, ULONG starting_index,
                      ULONG number_to_clear) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlSetBits                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Changes a range of bits to 1                                       *
     ********************************************************************************/
    VOID RtlSetBits(PRTL_BITMAP bitmap_header, ULONG starting_index, ULONG number_to_set) noexcept;

} // namespace tklib