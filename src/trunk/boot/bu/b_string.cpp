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
 *  MODULE  : Bootstrapping                                                     *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Boot-stage memory utility implementations.                        *
 *            Freestanding — no standard library, no tklib dependency.          *
 *            These are the only memory functions available during boot.        *
 *                                                                              *
 * *****************************************************************************/

#include <trunk/boot/bu/b_string.h>

namespace trunk::boot
{

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions must not overlap.           *
     *            Forward copy — safe when dst < src.                               *
     *                                                                              *
     * *****************************************************************************/
    void *memcpy(void *dst, const void *src, usize n) noexcept
    {
        auto *d = static_cast<u8 *>(dst);
        const auto *s = static_cast<const u8 *>(src);
        for (usize i = 0; i < n; ++i)
            d[i] = s[i];
        return dst;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memset                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Fill n bytes of dst with val.                                     *
     *                                                                              *
     * *****************************************************************************/
    void *memset(void *dst, u8 val, usize n) noexcept
    {
        auto *d = static_cast<u8 *>(dst);
        for (usize i = 0; i < n; ++i)
            d[i] = val;
        return dst;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcmp                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare n bytes of a and b.                                       *
     *            Returns 0 if equal, <0 if a < b, >0 if a > b.                     *
     *                                                                              *
     * *****************************************************************************/
    int memcmp(const void *a, const void *b, usize n) noexcept
    {
        const auto *pa = static_cast<const u8 *>(a);
        const auto *pb = static_cast<const u8 *>(b);
        for (usize i = 0; i < n; ++i)
            if (pa[i] != pb[i])
                return static_cast<int>(pa[i]) - static_cast<int>(pb[i]);
        return 0;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : strlen                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Freestanding strlen. Returns length of null-terminated string,     *
     *            capped at max to prevent overrun into unmapped memory.             *
     ********************************************************************************/
    usize strlen(const char *s, usize max) noexcept
    {
        usize n = 0;
        while (n < max && s[n] != '\0')
            ++n;
        return n;
    }

} // namespace trunk::boot