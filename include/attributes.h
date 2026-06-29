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
 *  MODULE  : Global definitions                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Global-level macros for everything                                 *
 ********************************************************************************/
#pragma once

// clang-format off
#if defined(__GNUC__) || defined(__clang__)
    #define GNU_PACKED   [[gnu::packed]]
    #define NO_DISCARD   [[nodiscard]]
    #define NO_RETURN    [[noreturn]]
    #define UNLIKELY     [[unlikely]]
    #define MAYBE_UNUSED [[maybe_unused]]

    #define INLINE       inline
    #define INLINE_CONST inline constexpr

    #define ALWAYS_INLINE __attribute__((always_inline))
    #define ALIGNED(x)    __attribute__((aligned(x)))
    #define TEXT_SECTION  __attribute__((section(".text")))

    #define IS_ALIGNED(addr, alignment) (((addr) & ((alignment) - 1)) == 0)
    #define OFFSET_OF(type, member)     reinterpret_cast<SIZE_T>(&(reinterpret_cast<type*>(0)->member))

    #define EXTERN_C extern "C"
#else
    #define GNU_PACKED
    #define NO_DISCARD
    #define NO_RETURN
    #define UNLIKELY
    #define MAYBE_UNUSED
    #define INLINE
    #define INLINE_CONST
    #define ALWAYS_INLINE
    #define ALIGNED
    #define IS_ALIGNED
    #define OFFSET_OF
    #define TEXT_SECTION
    #define EXTERN_C
#endif

// clang-format on