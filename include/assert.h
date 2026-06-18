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
 *  PURPOSE : Global-level assert() macros                                       *
 ********************************************************************************/
#pragma once

#include <cbk/kern/kabort.h>

// clang-format off
#ifdef __cplusplus
    #define STATIC_ASSERT(expr, msg) static_assert(expr, msg)
#else
    #define STATIC_ASSERT(expr, msg) _Static_assert(expr, msg)
#endif

#if defined(TRUNK_DEBUG) || !defined(NDEBUG)
    #define ASSERT(condition, message)                                            \
        do {                                                                      \
            if (!(condition)) UNLIKELY {                                      \
                ::trunk::kernel::KAbort("ASSERTION FAILED: " message " (" #condition ")"); \
            }                                                                     \
        } while (false)
#else
    #define ASSERT(condition, message) do { (VOID )(condition); } while (false)
#endif

// clang-format on