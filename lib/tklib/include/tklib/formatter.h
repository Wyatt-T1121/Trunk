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
 *  PURPOSE : Number to string utilites                                          *
 ********************************************************************************/

#pragma once

#include <types.h>

namespace tklib
{
    inline constexpr const char HEX_CHARS[] = "0123456789abcdef";

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_hex                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a zero-padded 16-digit hex string into buf.          *
     ********************************************************************************/
    VOID fmt_hex(char *buf, SIZE_T size, QWORD val) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_hex32                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a zero-padded 8-digit hex string into buf.           *
     ********************************************************************************/
    VOID fmt_hex32(char *buf, SIZE_T size, DWORD val) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_dec                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a decimal string into buf.                           *
     ********************************************************************************/
    VOID fmt_dec(char *buf, SIZE_T size, QWORD val) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_dec_signed                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a signed decimal string into buf.                    *
     ********************************************************************************/
    VOID fmt_dec_signed(char *buf, SIZE_T size, LONGLONG val) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_bin                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a zero-padded 64-bit binary string into buf.         *
     ********************************************************************************/
    VOID fmt_bin(char *buf, SIZE_T size, QWORD val) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_size                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a human-readable size string (KB, MB, GB).           *
     ********************************************************************************/
    VOID fmt_size(char *buf, SIZE_T size, QWORD val) noexcept;

} // namespace tklib