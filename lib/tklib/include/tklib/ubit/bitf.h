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
 *  MODULE  : Standard library utilites                                          *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Bit-based helper functions (bitwise helpers inside bit.h)          *
 ********************************************************************************/

#pragma once

#include <types.h>

namespace trunk::bitf
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : popcount                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the number of set bits in value.                           *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 popcount(u32 value) noexcept
    {
        return static_cast<u32>(__builtin_popcount(value));
    }

    [[nodiscard]] constexpr u32 popcount(u64 value) noexcept
    {
        return static_cast<u32>(__builtin_popcountll(value));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : clz                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Count leading zero bits.                                           *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 clz(u32 value) noexcept
    {
        return static_cast<u32>(__builtin_clz(value));
    }

    [[nodiscard]] constexpr u32 clz(u64 value) noexcept
    {
        return static_cast<u32>(__builtin_clzll(value));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ctz                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Count trailing zero bits.                                          *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 ctz(u32 value) noexcept
    {
        return static_cast<u32>(__builtin_ctz(value));
    }

    [[nodiscard]] constexpr u32 ctz(u64 value) noexcept
    {
        return static_cast<u32>(__builtin_ctzll(value));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : bit_width                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the number of bits needed to represent value.              *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 bit_width(u32 value) noexcept
    {
        return value == 0 ? 0u : 32u - clz(value);
    }

    [[nodiscard]] constexpr u32 bit_width(u64 value) noexcept
    {
        return value == 0 ? 0u : 64u - clz(value);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : bit_floor                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the largest power of two not greater than value.           *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 bit_floor(u32 value) noexcept
    {
        return value == 0 ? 0u : 1u << (bit_width(value) - 1u);
    }

    [[nodiscard]] constexpr u64 bit_floor(u64 value) noexcept
    {
        return value == 0 ? 0ull : 1ull << (bit_width(value) - 1u);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : bit_ceil                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the smallest power of two not less than value.             *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 bit_ceil(u32 value) noexcept
    {
        if (value <= 1)
            return 1u;
        return 1u << bit_width(value - 1u);
    }

    [[nodiscard]] constexpr u64 bit_ceil(u64 value) noexcept
    {
        if (value <= 1)
            return 1ull;
        return 1ull << bit_width(value - 1ull);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : has_single_bit                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if value is a non-zero power of two.                  *
     ********************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr bool has_single_bit(T value) noexcept
    {
        return value != 0 && (value & (value - 1)) == 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : test_bit                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if bit at position pos is set in value.               *
     ********************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr bool test_bit(T value, u32 pos) noexcept
    {
        return (value >> pos) & T{1};
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : set_bit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns value with bit at position pos set.                        *
     ********************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr T set_bit(T value, u32 pos) noexcept
    {
        return value | (T{1} << pos);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : clear_bit                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns value with bit at position pos cleared.                    *
     ********************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr T clear_bit(T value, u32 pos) noexcept
    {
        return value & ~(T{1} << pos);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : toggle_bit                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns value with bit at position pos toggled.                    *
     ********************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr T toggle_bit(T value, u32 pos) noexcept
    {
        return value ^ (T{1} << pos);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : rotl                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Rotate value left by shift bits.                                   *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 rotl(u32 value, u32 shift) noexcept
    {
        shift &= 31u;
        return (value << shift) | (value >> (32u - shift));
    }

    [[nodiscard]] constexpr u64 rotl(u64 value, u32 shift) noexcept
    {
        shift &= 63u;
        return (value << shift) | (value >> (64u - shift));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : rotr                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Rotate value right by shift bits.                                  *
     ********************************************************************************/
    [[nodiscard]] constexpr u32 rotr(u32 value, u32 shift) noexcept
    {
        shift &= 31u;
        return (value >> shift) | (value << (32u - shift));
    }

    [[nodiscard]] constexpr u64 rotr(u64 value, u32 shift) noexcept
    {
        shift &= 63u;
        return (value >> shift) | (value << (64u - shift));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : byteswap                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reverse the byte order of value.                                   *
     ********************************************************************************/
    [[nodiscard]] constexpr u16 byteswap(u16 value) noexcept
    {
        return static_cast<u16>(__builtin_bswap16(value));
    }

    [[nodiscard]] constexpr u32 byteswap(u32 value) noexcept
    {
        return __builtin_bswap32(value);
    }

    [[nodiscard]] constexpr u64 byteswap(u64 value) noexcept
    {
        return __builtin_bswap64(value);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mask                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns a u64 mask with the lowest n bits set.                     *
     ********************************************************************************/
    [[nodiscard]] constexpr u64 mask(u32 n) noexcept
    {
        return n >= 64 ? ~u64{0} : (u64{1} << n) - 1;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : extract_bits                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Extract a field of width bits starting at bit position pos.        *
     ********************************************************************************/
    [[nodiscard]] constexpr u64 extract_bits(u64 value, u32 pos, u32 width) noexcept
    {
        return (value >> pos) & mask(width);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : insert_bits                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Insert field into value at bit position pos with given width.      *
     ********************************************************************************/
    [[nodiscard]] constexpr u64 insert_bits(u64 value, u64 field, u32 pos, u32 width) noexcept
    {
        const u64 m = mask(width);
        return (value & ~(m << pos)) | ((field & m) << pos);
    }

} // namespace trunk::bitf