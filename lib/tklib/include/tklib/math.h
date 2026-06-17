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
 *  MODULE  : Standard library utility                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Math utilites.                                                     *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

namespace tklib::math
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : is_power_of_two                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if value is a non-zero power of two.                  *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr bool is_power_of_two(T value) noexcept
    {
        return value != 0 && (value & (value - 1)) == 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : align_up                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Round value up to the nearest multiple of alignment.               *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T align_up(T value, T alignment) noexcept
    {
        T rem = value % alignment;
        return (rem == 0) ? value : value + (alignment - rem);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : align_down                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Round value down to the nearest multiple of alignment.             *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T align_down(T value, T alignment) noexcept
    {
        return value & ~(alignment - 1);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : align_up_pow2                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Round value up to the next power of two greater than or equal      *
     *            to value.                                                          *
     ********************************************************************************/
    NO_DISCARD constexpr u64 align_up_pow2(u64 value) noexcept
    {
        if (value == 0)
            return 1;
        --value;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value |= value >> 32;
        return value + 1;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : is_aligned                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if value is aligned to alignment.                     *
     ********************************************************************************/
    template <typename T, typename U>
    NO_DISCARD constexpr bool is_aligned(T value, U alignment) noexcept
    {
        return (value & (static_cast<T>(alignment) - 1)) == 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : add_would_overflow                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if a + b would overflow T.                            *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr bool add_would_overflow(T a, T b) noexcept
    {
        return b > 0 && a > limits::u64_max - b;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : mul_would_overflow                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns true if a * b would overflow u64.                          *
     ********************************************************************************/
    NO_DISCARD constexpr bool mul_would_overflow(u64 a, u64 b) noexcept
    {
        if (a == 0 || b == 0)
            return false;
        return a > limits::u64_max / b;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : min                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the smaller of a and b.                                    *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T min(T a, T b) noexcept
    {
        return a < b ? a : b;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : max                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the larger of a and b.                                     *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T max(T a, T b) noexcept
    {
        return a > b ? a : b;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : clamp                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Clamp value to [lo, hi].                                           *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T clamp(T value, T lo, T hi) noexcept
    {
        return value < lo ? lo : (value > hi ? hi : value);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : abs                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns the absolute value of a signed integer.                    *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T abs(T value) noexcept
    {
        return value < 0 ? -value : value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : sign                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns -1, 0, or 1 depending on the sign of value.                *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T sign(T value) noexcept
    {
        return (value > T{0}) - (value < T{0});
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : div_round_up                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Integer division rounding up.                                      *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T div_round_up(T a, T b) noexcept
    {
        return (a + b - 1) / b;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : div_round_down                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Integer division rounding down.                                    *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T div_round_down(T a, T b) noexcept
    {
        return a / b;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : div_round_nearest                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Integer division rounding to the nearest integer.                  *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T div_round_nearest(T a, T b) noexcept
    {
        return (a + b / 2) / b;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pow                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Computes base raised to the power of exp. Integer only.            *
     ********************************************************************************/
    template <typename T> NO_DISCARD constexpr T pow(T base, u32 exp) noexcept
    {
        T result = 1;
        while (exp > 0) {
            if (exp & 1)
                result *= base;
            base  *= base;
            exp  >>= 1;
        }
        return result;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : log2_floor                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns floor(log2(value)).                                        *
     ********************************************************************************/
    NO_DISCARD constexpr u32 log2_floor(u64 value) noexcept
    {
        u32 result = 0;
        while (value >>= 1)
            ++result;
        return result;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : log2_ceil                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns ceil(log2(value)).                                         *
     ********************************************************************************/
    NO_DISCARD constexpr u32 log2_ceil(u64 value) noexcept
    {
        if (value <= 1)
            return 0;
        return log2_floor(value - 1) + 1;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : log10_floor                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns floor(log10(value)).                                       *
     ********************************************************************************/
    NO_DISCARD constexpr u32 log10_floor(u64 value) noexcept
    {
        u32 result = 0;
        while (value >= 10) {
            value /= 10;
            ++result;
        }
        return result;
    }

} // namespace tklib::math