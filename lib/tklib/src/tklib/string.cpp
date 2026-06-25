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
 *  AUTHOR  : Trollycat                                                         *
 *  MODULE  : Standard library utility                                          *
 *  DATE    : 2026                                                              *
 *  PURPOSE : String and memory manipulation utilities                          *
 * *****************************************************************************/
#include <tklib/string.h>

namespace tklib
{

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions must not overlap.           *
     * *****************************************************************************/
    PVOID memcpy(PVOID dst, LPCVOID src, SIZE_T n) noexcept
    {
        auto *d       = static_cast<BYTE *>(dst);
        CONST auto *s = static_cast<CONST BYTE *>(src);
        for (SIZE_T i = 0; i < n; ++i)
            d[i] = s[i];
        return dst;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memset                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Fill n bytes of dst with value.                                   *
     * *****************************************************************************/
    PVOID memset(PVOID dst, BYTE value, SIZE_T n) noexcept
    {
        auto *d = static_cast<BYTE *>(dst);
        for (SIZE_T i = 0; i < n; ++i)
            d[i] = value;
        return dst;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memmove                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions may overlap.                *
     * *****************************************************************************/
    PVOID memmove(PVOID dst, LPCVOID src, SIZE_T n) noexcept
    {
        auto *d       = static_cast<BYTE *>(dst);
        CONST auto *s = static_cast<CONST BYTE *>(src);

        if (d < s) {
            for (SIZE_T i = 0; i < n; ++i)
                d[i] = s[i];
        } else if (d > s) {
            for (SIZE_T i = n; i > 0; --i)
                d[i - 1] = s[i - 1];
        }
        return dst;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcmp                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare n bytes of a and b.                                       *
     * *****************************************************************************/
    LONG memcmp(LPCVOID a, LPCVOID b, SIZE_T n) noexcept
    {
        CONST auto *pa = static_cast<CONST BYTE *>(a);
        CONST auto *pb = static_cast<CONST BYTE *>(b);
        for (SIZE_T i = 0; i < n; ++i) {
            if (pa[i] != pb[i])
                return static_cast<LONG>(pa[i]) - static_cast<LONG>(pb[i]);
        }
        return 0;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memchr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of value in first n bytes of ptr.           *
     * *****************************************************************************/
    LPCVOID memchr(LPCVOID ptr, BYTE value, SIZE_T n) noexcept
    {
        CONST auto *p = static_cast<CONST BYTE *>(ptr);
        for (SIZE_T i = 0; i < n; ++i)
            if (p[i] == value)
                return &p[i];
        return nullptr;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlen                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return the length of a null-terminated string, not including      *
     *            the null terminator.                                              *
     * *****************************************************************************/
    SIZE_T strlen(PCSTR s) noexcept
    {
        PCSTR p = s;
        while (*p)
            ++p;
        return static_cast<SIZE_T>(p - s);
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strnlen                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return the length of s, stopping at max.                          *
     * *****************************************************************************/
    SIZE_T strnlen(PCSTR s, SIZE_T max) noexcept
    {
        SIZE_T n = 0;
        while (n < max && s[n])
            ++n;
        return n;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcmp                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare two null-terminated strings.                              *
     * *****************************************************************************/
    LONG strcmp(PCSTR a, PCSTR b) noexcept
    {
        while (*a && *a == *b) {
            ++a;
            ++b;
        }
        return static_cast<LONG>(static_cast<BYTE>(*a)) - static_cast<LONG>(static_cast<BYTE>(*b));
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncmp                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare up to n characters of two strings.                        *
     * *****************************************************************************/
    LONG strncmp(PCSTR a, PCSTR b, SIZE_T n) noexcept
    {
        for (SIZE_T i = 0; i < n; ++i) {
            if (a[i] != b[i])
                return static_cast<LONG>(static_cast<BYTE>(a[i])) -
                       static_cast<LONG>(static_cast<BYTE>(b[i]));
            if (!a[i])
                return 0;
        }
        return 0;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcasecmp                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Case-insensitive comparison of two null-terminated strings.       *
     * *****************************************************************************/
    LONG strcasecmp(PCSTR a, PCSTR b) noexcept
    {
        while (*a && to_lower(*a) == to_lower(*b)) {
            ++a;
            ++b;
        }
        return static_cast<LONG>(static_cast<BYTE>(to_lower(*a))) -
               static_cast<LONG>(static_cast<BYTE>(to_lower(*b)));
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncasecmp                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Case-insensitive comparison of up to n characters.                *
     * *****************************************************************************/
    LONG strncasecmp(PCSTR a, PCSTR b, SIZE_T n) noexcept
    {
        for (SIZE_T i = 0; i < n; ++i) {
            CHAR ca = to_lower(a[i]);
            CHAR cb = to_lower(b[i]);
            if (ca != cb)
                return static_cast<LONG>(static_cast<BYTE>(ca)) -
                       static_cast<LONG>(static_cast<BYTE>(cb));
            if (!a[i])
                return 0;
        }
        return 0;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy src into dst including null terminator. Prefer strlcpy.      *
     * *****************************************************************************/
    PCHAR strcpy(PCHAR dst, PCSTR src) noexcept
    {
        PCHAR d = dst;
        while ((*d++ = *src++)) {
        }
        return dst;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncpy                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy at most n bytes of src into dst. Pads with null bytes.       *
     * *****************************************************************************/
    PCHAR strncpy(PCHAR dst, PCSTR src, SIZE_T n) noexcept
    {
        SIZE_T i = 0;
        for (; i < n && src[i]; ++i)
            dst[i] = src[i];
        for (; i < n; ++i)
            dst[i] = '\0';
        return dst;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlcpy                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy src into dst writing at most (size - 1) chars.               *
     * *****************************************************************************/
    SIZE_T strlcpy(PCHAR dst, PCSTR src, SIZE_T size) noexcept
    {
        SIZE_T src_len = strlen(src);
        if (size == 0)
            return src_len;

        SIZE_T copy = src_len < (size - 1) ? src_len : (size - 1);
        memcpy(dst, src, copy);
        dst[copy] = '\0';
        return src_len;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcat                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append src to dst. dst must have enough space. Prefer strlcat.    *
     * *****************************************************************************/
    PCHAR strcat(PCHAR dst, PCSTR src) noexcept
    {
        PCHAR d = dst + strlen(dst);
        while ((*d++ = *src++)) {
        }
        return dst;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncat                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append at most n characters of src to dst.                        *
     * *****************************************************************************/
    PCHAR strncat(PCHAR dst, PCSTR src, SIZE_T n) noexcept
    {
        PCHAR d  = dst + strlen(dst);
        SIZE_T i = 0;
        for (; i < n && src[i]; ++i)
            d[i] = src[i];
        d[i] = '\0';
        return dst;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlcat                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append src to dst writing at most (size - strlen(dst) - 1) chars. *
     * *****************************************************************************/
    SIZE_T strlcat(PCHAR dst, PCSTR src, SIZE_T size) noexcept
    {
        SIZE_T dst_len = strnlen(dst, size);
        SIZE_T src_len = strlen(src);
        if (dst_len >= size)
            return dst_len + src_len;

        SIZE_T space = size - dst_len - 1;
        SIZE_T copy  = src_len < space ? src_len : space;
        memcpy(dst + dst_len, src, copy);
        dst[dst_len + copy] = '\0';
        return dst_len + src_len;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strchr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of c in s.                                  *
     * *****************************************************************************/
    PCSTR strchr(PCSTR s, CHAR c) noexcept
    {
        for (; *s; ++s)
            if (*s == c)
                return s;
        return (c == '\0') ? s : nullptr;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strrchr                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find last occurrence of c in s.                                   *
     * *****************************************************************************/
    PCSTR strrchr(PCSTR s, CHAR c) noexcept
    {
        PCSTR last = nullptr;
        for (; *s; ++s)
            if (*s == c)
                last = s;
        return (c == '\0') ? s : last;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strstr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of needle in haystack.                      *
     * *****************************************************************************/
    PCSTR strstr(PCSTR haystack, PCSTR needle) noexcept
    {
        if (!*needle)
            return haystack;
        for (; *haystack; ++haystack) {
            PCSTR h = haystack;
            PCSTR n = needle;
            while (*h && *n && *h == *n) {
                ++h;
                ++n;
            }
            if (!*n)
                return haystack;
        }
        return nullptr;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strnstr                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of needle in at most n bytes of haystack.   *
     * *****************************************************************************/
    PCSTR strnstr(PCSTR haystack, PCSTR needle, SIZE_T n) noexcept
    {
        SIZE_T needle_len = strlen(needle);
        if (needle_len == 0)
            return haystack;
        if (n < needle_len)
            return nullptr;

        for (SIZE_T i = 0; i <= n - needle_len; ++i) {
            if (strncmp(haystack + i, needle, needle_len) == 0)
                return haystack + i;
        }
        return nullptr;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strpbrk                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first character in s that appears in accept.                 *
     * *****************************************************************************/
    PCSTR strpbrk(PCSTR s, PCSTR accept) noexcept
    {
        for (; *s; ++s)
            for (PCSTR a = accept; *a; ++a)
                if (*s == *a)
                    return s;
        return nullptr;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strspn                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return length of leading segment of s made entirely of chars      *
     *            found in accept.                                                  *
     * *****************************************************************************/
    SIZE_T strspn(PCSTR s, PCSTR accept) noexcept
    {
        SIZE_T n = 0;
        for (; s[n]; ++n) {
            BOOL found = FALSE;
            for (PCSTR a = accept; *a; ++a)
                if (s[n] == *a) {
                    found = TRUE;
                    break;
                }
            if (!found)
                break;
        }
        return n;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcspn                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return length of leading segment of s made entirely of chars      *
     *            NOT found in reject.                                              *
     * *****************************************************************************/
    SIZE_T strcspn(PCSTR s, PCSTR reject) noexcept
    {
        SIZE_T n = 0;
        for (; s[n]; ++n)
            for (PCSTR r = reject; *r; ++r)
                if (s[n] == *r)
                    return n;
        return n;
    }

} // namespace tklib