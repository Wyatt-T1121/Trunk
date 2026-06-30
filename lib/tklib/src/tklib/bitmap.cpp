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
#include <tklib/bitmap.h>

#include <tklib/ubit/bitf.h>

// BITMAP HELPERS
// 0 / 1
// 0 = FREE
// 1 = RESERVED

namespace tklib
{
    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiModifyBitmapRange                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Modify a bitmap range (kinda keeps API clean)                      *
         ********************************************************************************/
        template <typename OpFunc>
        ALWAYS_INLINE VOID MiModifyBitmapRange(PRTL_BITMAP bitmap_header, ULONG starting_index,
                                               ULONG number_to_modify, OpFunc bit_op) noexcept
        {
            for (ULONG i = 0; i < number_to_modify; ++i) {
                ULONG bit_index   = starting_index + i;
                ULONG block_index = bit_index / 32;
                DWORD bit_pos     = static_cast<DWORD>(bit_index % 32);

                bitmap_header->buffer[block_index] =
                    bit_op(bitmap_header->buffer[block_index], bit_pos);
            }
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlInitializeBitMap                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the bitmap struct with an existin block of memory       *
     ********************************************************************************/
    VOID RtlInitializeBitMap(PRTL_BITMAP bitmap_header, PULONG bitmap_buffer,
                             ULONG size_of_bitmap) noexcept
    {
        bitmap_header->size_of_bitmap = size_of_bitmap;
        bitmap_header->buffer         = bitmap_buffer;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlFindClearBitsAndSet                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Scans the bitmap for clear bits and marks them as 1s               *
     ********************************************************************************/
    NO_DISCARD ULONG RtlFindClearBitsAndSet(PRTL_BITMAP bitmap_header, ULONG number_to_find,
                                            ULONG hint_index) noexcept
    {
        if (number_to_find == 0 || number_to_find > bitmap_header->size_of_bitmap)
            return RTL_BITMAP_FAILED;

        const ULONG total_bits  = bitmap_header->size_of_bitmap;
        ULONG current_run_start = RTL_BITMAP_FAILED;
        ULONG run_count         = 0;

        for (ULONG i = hint_index; i < total_bits; ++i) {
            ULONG block_index = i / 32;
            DWORD bit_pos     = static_cast<DWORD>(i % 32);

            ULONG block_value = bitmap_header->buffer[block_index];

            if (block_value == RTL_BITMAP_FAILED && bit_pos == 0) {
                run_count          = 0;
                current_run_start  = RTL_BITMAP_FAILED;
                i                 += 31;
                continue;
            }

            if (!tklib::test_bit(block_value, bit_pos)) {
                if (current_run_start == RTL_BITMAP_FAILED)
                    current_run_start = i;
                run_count++;

                if (run_count == number_to_find) {
                    RtlSetBits(bitmap_header, current_run_start, number_to_find);
                    return current_run_start;
                }
            } else {
                run_count         = 0;
                current_run_start = RTL_BITMAP_FAILED;
            }
        }

        return RTL_BITMAP_FAILED;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlClearBits                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Changes a range of bits back to 0                                  *
     ********************************************************************************/
    VOID RtlClearBits(PRTL_BITMAP bitmap_header, ULONG starting_index,
                      ULONG number_to_clear) noexcept
    {
        MiModifyBitmapRange(bitmap_header, starting_index, number_to_clear,
                            [](ULONG block, DWORD pos) {
                                return tklib::clear_bit(block, pos);
                            });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RtlSetBits                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Changes a range of bits to 1                                       *
     ********************************************************************************/
    VOID RtlSetBits(PRTL_BITMAP bitmap_header, ULONG starting_index, ULONG number_to_set) noexcept
    {
        MiModifyBitmapRange(bitmap_header, starting_index, number_to_set,
                            [](ULONG block, DWORD pos) {
                                return tklib::set_bit(block, pos);
                            });
    }

} // namespace tklib