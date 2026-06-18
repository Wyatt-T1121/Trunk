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
 *  PURPOSE : Type aliases                                                       *
 ********************************************************************************/
#pragma once

#include <stddef.h>
#include <stdint.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

using BYTE  = uint8_t;
using WORD  = uint16_t;
using DWORD = uint32_t;
using QWORD = uint64_t;

using CHAR     = int8_t;
using SHORT    = int16_t;
using LONG     = int32_t;
using LONGLONG = int64_t;

using UCHAR     = uint8_t;
using USHORT    = uint16_t;
using ULONG     = uint32_t;
using ULONGLONG = uint64_t;

using SIZE_T    = size_t;
using ULONG_PTR = uintptr_t;
using LONG_PTR  = intptr_t;

using BOOL = bool;

using VOID    = void;
using PVOID   = void *;
using LPCVOID = const void *;

using PBYTE = BYTE *;

using PWORD  = WORD *;
using PDWORD = DWORD *;
using PQWORD = QWORD *;
using PCHAR  = CHAR *;
using PLONG  = LONG *;
using PULONG = ULONG *;
using PCSTR  = const char *;
using PWSTR  = wchar_t *;
using PCWSTR = const wchar_t *;

using HANDLE  = void *;
using PHANDLE = HANDLE *;

inline constexpr ULONG_PTR INVALID_HANDLE_VALUE = ~ULONG_PTR{0};

namespace limits
{
    inline constexpr BYTE BYTE_min   = 0;
    inline constexpr BYTE BYTE_max   = UINT8_MAX;
    inline constexpr WORD WORD_min   = 0;
    inline constexpr WORD WORD_max   = UINT16_MAX;
    inline constexpr DWORD DWORD_min = 0;
    inline constexpr DWORD DWORD_max = UINT32_MAX;
    inline constexpr QWORD QWORD_min = 0;
    inline constexpr QWORD QWORD_max = UINT64_MAX;

    inline constexpr CHAR CHAR_min         = INT8_MIN;
    inline constexpr CHAR CHAR_max         = INT8_MAX;
    inline constexpr SHORT SHORT_min       = INT16_MIN;
    inline constexpr SHORT SHORT_max       = INT16_MAX;
    inline constexpr LONG LONG_min         = INT32_MIN;
    inline constexpr LONG LONG_max         = INT32_MAX;
    inline constexpr LONGLONG LONGLONG_min = INT64_MIN;
    inline constexpr LONGLONG LONGLONG_max = INT64_MAX;

    inline constexpr SIZE_T SIZE_T_max       = SIZE_MAX;
    inline constexpr ULONG_PTR ULONG_PTR_max = UINTPTR_MAX;
    inline constexpr LONG_PTR LONG_PTR_min   = INTPTR_MIN;
    inline constexpr LONG_PTR LONG_PTR_max   = INTPTR_MAX;
} // namespace limits

#pragma GCC diagnostic pop