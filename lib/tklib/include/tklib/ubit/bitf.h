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
 *  MODULE  : Standard library utilites                                          *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Bit-based helper functions                                         *
 ********************************************************************************/
#pragma once

#include <attributes.h>
#include <types.h>

namespace tklib
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : popcount                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the number of set bits in value.                           *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr DWORD popcount(T value) noexcept
    {
        static_assert(is_integral_v<T>, "popcount requires an integral type");
        if constexpr (sizeof(T) <= 4) {
            return static_cast<DWORD>(__builtin_popcount(static_cast<unsigned int>(value)));
        } else
            return static_cast<DWORD>(__builtin_popcountll(static_cast<unsigned long long>(value)));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : clz                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Count leading zero bits.                                           *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr DWORD clz(T value) noexcept
    {
        static_assert(is_integral_v<T>, "clz requires an integral type");
        if constexpr (sizeof(T) <= 4) {
            return static_cast<DWORD>(__builtin_clz(static_cast<unsigned int>(value)));
        } else {
            constexpr DWORD delta = 64u - (sizeof(T) * 8u);
            return static_cast<DWORD>(__builtin_clzll(static_cast<unsigned long long>(value))) -
                   delta;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ctz                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Count trailing zero bits.                                          *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr DWORD ctz(T value) noexcept
    {
        static_assert(is_integral_v<T>, "ctz requires an integral type");
        if constexpr (sizeof(T) <= 4) {
            return static_cast<DWORD>(__builtin_ctz(static_cast<unsigned int>(value)));
        } else {
            return static_cast<DWORD>(__builtin_ctzll(static_cast<unsigned long long>(value)));
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : bit_width                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the number of bits needed to represent value.              *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr DWORD bit_width(T value) noexcept
    {
        static_assert(is_integral_v<T>, "bit_width requires an integral type");
        return value == 0 ? 0u : (static_cast<DWORD>(sizeof(T) * 8u)) - clz(value);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : bit_floor                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the largest power of two not greater than value.           *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T bit_floor(T value) noexcept
    {
        static_assert(is_integral_v<T>, "bit_floor requires an integral type");
        return value == 0 ? T{0} : T{1} << (bit_width(value) - 1u);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : bit_ceil                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the smallest power of two not less than value.             *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T bit_ceil(T value) noexcept
    {
        static_assert(is_integral_v<T>, "bit_ceil requires an integral type");
        if (value <= 1)
            return T{1};
        return T{1} << bit_width(static_cast<T>(value - 1u));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : has_single_bit                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns TRUE if value is a non-zero power of two.                  *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr BOOL has_single_bit(T value) noexcept
    {
        return value != 0 && (value & (value - 1)) == 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : test_bit                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns TRUE if bit at position pos is set in value.               *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr BOOL test_bit(T value, DWORD pos) noexcept
    {
        return (value >> pos) & T{1};
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : set_bit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns value with bit at position pos set.                        *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T set_bit(T value, DWORD pos) noexcept
    {
        return value | (T{1} << pos);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : clear_bit                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns value with bit at position pos cleared.                    *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T clear_bit(T value, DWORD pos) noexcept
    {
        return value & ~(T{1} << pos);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : toggle_bit                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns value with bit at position pos toggled.                    *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T toggle_bit(T value, DWORD pos) noexcept
    {
        return value ^ (T{1} << pos);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : rotl                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Rotate value left by shift bits.                                   *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T rotl(T value, DWORD shift) noexcept
    {
        static_assert(is_integral_v<T>, "rotl requires an integral type");
        constexpr DWORD width  = sizeof(T) * 8u;
        shift                 &= (width - 1u);
        if (shift == 0)
            return value;
        return (value << shift) | (value >> (width - shift));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : rotr                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Rotate value right by shift bits.                                  *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T rotr(T value, DWORD shift) noexcept
    {
        static_assert(is_integral_v<T>, "rotr requires an integral type");
        constexpr DWORD width  = sizeof(T) * 8u;
        shift                 &= (width - 1u);
        if (shift == 0)
            return value;
        return (value >> shift) | (value << (width - shift));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : byteswap                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reverse the byte order of value.                                   *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T byteswap(T value) noexcept
    {
        static_assert(is_integral_v<T>, "byteswap requires an integral type");
        if constexpr (sizeof(T) == 2) {
            return static_cast<T>(__builtin_bswap16(static_cast<WORD>(value)));
        } else if constexpr (sizeof(T) == 4) {
            return static_cast<T>(__builtin_bswap32(static_cast<DWORD>(value)));
        } else if constexpr (sizeof(T) == 8) {
            return static_cast<T>(__builtin_bswap64(static_cast<QWORD>(value)));
        } else {
            return value;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mask                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns a QWORD mask with the lowest n bits set.                     *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD mask(DWORD n) noexcept
    {
        return n >= 64 ? ~QWORD{0} : (QWORD{1} << n) - 1;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : extract_bits                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Extract a field of width bits starting at bit position pos.        *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD extract_bits(QWORD value, DWORD pos, DWORD width) noexcept
    {
        return (value >> pos) & mask(width);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : insert_bits                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Insert field into value at bit position pos with given width.      *
     ********************************************************************************/
    NO_DISCARD constexpr QWORD insert_bits(QWORD value, QWORD field, DWORD pos,
                                           DWORD width) noexcept
    {
        const QWORD m = mask(width);
        return (value & ~(m << pos)) | ((field & m) << pos);
    }

} // namespace tklib