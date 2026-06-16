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

#include <tklib/formatter.h>
#include <tklib/string.h>

namespace tklib
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_hex                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a zero-padded 16-digit hex string into buf.          *
     ********************************************************************************/
    void fmt_hex(char *buf, usize size, u64 val) noexcept
    {
        if (!buf || size < 19)
            return;

        buf[0] = '0';
        buf[1] = 'x';
        buf[18] = '\0';

        for (i32 i = 17; i >= 2; --i)
        {
            buf[i] = HEX_CHARS[val & 0xF];
            val >>= 4;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_hex32                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a zero-padded 8-digit hex string into buf.           *
     ********************************************************************************/
    void fmt_hex32(char *buf, usize size, u32 val) noexcept
    {
        if (!buf || size < 11)
            return;

        buf[0] = '0';
        buf[1] = 'x';
        buf[10] = '\0';

        for (i32 i = 9; i >= 2; --i)
        {
            buf[i] = HEX_CHARS[val & 0xF];
            val >>= 4;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_dec                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a decimal string into buf.                           *
     ********************************************************************************/
    void fmt_dec(char *buf, usize size, u64 val) noexcept
    {
        if (!buf || size < 2)
            return;

        if (val == 0)
        {
            buf[0] = '0';
            buf[1] = '\0';
            return;
        }

        char tmp[21];
        i32 i = 20;
        tmp[20] = '\0';

        while (val > 0 && i > 0)
        {
            tmp[--i] = '0' + static_cast<char>(val % 10);
            val /= 10;
        }

        usize len = static_cast<usize>(20 - i);
        if (len >= size)
            len = size - 1;

        memcpy(buf, &tmp[i], len);
        buf[len] = '\0';
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_dec_signed                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a signed decimal string into buf.                    *
     ********************************************************************************/
    void fmt_dec_signed(char *buf, usize size, i64 val) noexcept
    {
        if (!buf || size < 2)
            return;

        if (val >= 0)
        {
            fmt_dec(buf, size, static_cast<u64>(val));
            return;
        }

        buf[0] = '-';
        fmt_dec(buf + 1, size - 1, static_cast<u64>(-val));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_bin                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a zero-padded 64-bit binary string into buf.         *
     ********************************************************************************/
    void fmt_bin(char *buf, usize size, u64 val) noexcept
    {
        if (!buf || size < 67)
            return;

        buf[0] = '0';
        buf[1] = 'b';
        buf[66] = '\0';

        for (i32 i = 65; i >= 2; --i)
        {
            buf[i] = '0' + static_cast<char>(val & 1);
            val >>= 1;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : fmt_size                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Format val as a human-readable size string (KB, MB, GB).           *
     ********************************************************************************/
    void fmt_size(char *buf, usize size, u64 val) noexcept
    {
        if (!buf || size < 2)
            return;

        const char *unit;
        u64 display;

        if (val >= 1024ull * 1024ull * 1024ull)
        {
            display = val / (1024ull * 1024ull * 1024ull);
            unit = " GB";
        }
        else if (val >= 1024ull * 1024ull)
        {
            display = val / (1024ull * 1024ull);
            unit = " MB";
        }
        else if (val >= 1024ull)
        {
            display = val / 1024ull;
            unit = " KB";
        }
        else
        {
            display = val;
            unit = " B";
        }

        char tmp[21];
        fmt_dec(tmp, sizeof(tmp), display);
        strlcpy(buf, tmp, size);
        strlcat(buf, unit, size);
    }

} // namespace tklib