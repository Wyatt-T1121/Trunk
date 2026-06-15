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

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;
using u128 = unsigned __int128;

using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;
using i128 = __int128;

using usize = unsigned long;
using isize = long;
using uptr = unsigned long;

using f32 = float;
using f64 = double;

using b8 = bool;

using nullptr_t = decltype(nullptr);

enum class byte : unsigned char
{
};

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