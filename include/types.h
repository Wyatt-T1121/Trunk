/* ******************************************************************************
 *                                                                              *
 *  Copyright 2026 Trollycat                                                    *
 *                                                                              *
 *  Licensed under the Apache License, Version 2.0 (the "License");             *
 *  you may not use this file except in compliance with the License.            *
 *  You may obtain a copy of the License at                                     *
 *                                                                              *
 *      http://www.apache.org/licenses/LICENSE-2.0                              *
 *                                                                              *
 *  Unless required by applicable law or agreed to in writing, software         *
 *  distributed under the License is distributed on an "AS IS" BASIS,           *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
 *  See the License for the specific language governing permissions and         *
 *  limitations under the License.                                              *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  MODULE  : Global definitions                                                *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Freestanding primitive type aliases for the entire kernel.        *
 *            No standard library headers are included anywhere in Trunk.       *
 *            Every file that needs a sized integer includes this.              *
 *                                                                              *
 * *****************************************************************************/

#pragma once

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Unsigned integer aliases                                          *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Fixed-width unsigned types.                                       *
 *                                                                              *
 * *****************************************************************************/
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using u128 = unsigned __int128;

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Signed integer aliases                                            *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Fixed-width signed types.                                         *
 *                                                                              *
 * *****************************************************************************/
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;
using i128 = __int128;

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Size and pointer-sized types                                      *
 *  DATE    : 2026                                                              *
 *  PURPOSE : usize/isize: counts and offsets (size_t/ptrdiff_t equivalent).    *
 *            uptr: raw physical or virtual addresses.                          *
 *                                                                              *
 * *****************************************************************************/
using usize = unsigned long;
using isize = long;
using uptr = unsigned long;

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Floating point aliases                                            *
 *  DATE    : 2026                                                              *
 *  PURPOSE : f32 / f64 for cases where floating point is needed.               *
 *            Avoid in hot paths - kernel code prefers fixed point.             *
 *                                                                              *
 * *****************************************************************************/
using f32 = float;
using f64 = double;

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Boolean alias                                                     *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Explicit bool alias. Keeps parity with the other type aliases.    *
 *                                                                              *
 * *****************************************************************************/
using b8 = bool;

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Null pointer type                                                 *
 *  DATE    : 2026                                                              *
 *  PURPOSE : nullptr_t lets functions explicitly accept or return nullptr.     *
 *            Mirrors std::nullptr_t without any standard header.               *
 *                                                                              *
 * *****************************************************************************/
using nullptr_t = decltype(nullptr);

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Byte type                                                         *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Raw memory byte. Distinct from u8 to prevent arithmetic.          *
 *            Use when the value is raw memory, not a number.                   *
 *                                                                              *
 * *****************************************************************************/
enum class byte : unsigned char
{
};

/* ******************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  SECTION : Integer limits                                                    *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Compile-time min/max constants for every integer type.            *
 *            Avoids pulling in <climits> or <limits.h>.                        *
 *                                                                              *
 * *****************************************************************************/
namespace limits
{
    inline constexpr u8 u8_min = 0;
    inline constexpr u8 u8_max = 0xFF;
    inline constexpr u16 u16_min = 0;
    inline constexpr u16 u16_max = 0xFFFF;
    inline constexpr u32 u32_min = 0;
    inline constexpr u32 u32_max = 0xFFFF'FFFFu;
    inline constexpr u64 u64_min = 0;
    inline constexpr u64 u64_max = 0xFFFF'FFFF'FFFF'FFFFull;

    inline constexpr i8 i8_min = -128;
    inline constexpr i8 i8_max = 127;
    inline constexpr i16 i16_min = -32768;
    inline constexpr i16 i16_max = 32767;
    inline constexpr i32 i32_min = -2147483648;
    inline constexpr i32 i32_max = 2147483647;
    inline constexpr i64 i64_min = -9223372036854775807LL - 1;
    inline constexpr i64 i64_max = 9223372036854775807LL;

    inline constexpr usize usize_max = ~usize{0};
    inline constexpr uptr uptr_max = ~uptr{0};
} // namespace limits

#pragma GCC diagnostic pop