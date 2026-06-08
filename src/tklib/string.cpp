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
 *  MODULE  : Standard library utility                                          *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Implementation of freestanding string and memory utilities.       *
 *            No heap allocation. No dependencies beyond String.h.              *
 *                                                                              *
 * *****************************************************************************/

#include <tklib/string.h>

namespace tklib
{

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions must not overlap.           *
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
     *  PURPOSE : Fill n bytes of dst with value.                                   *
     *                                                                              *
     * *****************************************************************************/
    void *memset(void *dst, u8 value, usize n) noexcept
    {
        auto *d = static_cast<u8 *>(dst);
        for (usize i = 0; i < n; ++i)
            d[i] = value;
        return dst;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memmove                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions may overlap.                *
     *                                                                              *
     * *****************************************************************************/
    void *memmove(void *dst, const void *src, usize n) noexcept
    {
        auto *d = static_cast<u8 *>(dst);
        const auto *s = static_cast<const u8 *>(src);

        if (d < s)
        {
            for (usize i = 0; i < n; ++i)
                d[i] = s[i];
        }
        else if (d > s)
        {
            for (usize i = n; i > 0; --i)
                d[i - 1] = s[i - 1];
        }
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
    i32 memcmp(const void *a, const void *b, usize n) noexcept
    {
        const auto *pa = static_cast<const u8 *>(a);
        const auto *pb = static_cast<const u8 *>(b);
        for (usize i = 0; i < n; ++i)
        {
            if (pa[i] != pb[i])
                return static_cast<i32>(pa[i]) - static_cast<i32>(pb[i]);
        }
        return 0;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memchr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of value in first n bytes of ptr.           *
     *            Returns pointer to found byte or nullptr.                         *
     *                                                                              *
     * *****************************************************************************/
    const void *memchr(const void *ptr, u8 value, usize n) noexcept
    {
        const auto *p = static_cast<const u8 *>(ptr);
        for (usize i = 0; i < n; ++i)
            if (p[i] == value)
                return &p[i];
        return nullptr;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlen                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return the length of a null-terminated string, not including      *
     *            the null terminator.                                              *
     *                                                                              *
     * *****************************************************************************/
    usize strlen(const char *s) noexcept
    {
        const char *p = s;
        while (*p)
            ++p;
        return static_cast<usize>(p - s);
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strnlen                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return the length of s, stopping at max.                          *
     *            Safe alternative to strlen for untrusted input.                   *
     *                                                                              *
     * *****************************************************************************/
    usize strnlen(const char *s, usize max) noexcept
    {
        usize n = 0;
        while (n < max && s[n])
            ++n;
        return n;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcmp                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare two null-terminated strings.                              *
     *            Returns 0 if equal, <0 if a < b, >0 if a > b.                     *
     *                                                                              *
     * *****************************************************************************/
    i32 strcmp(const char *a, const char *b) noexcept
    {
        while (*a && *a == *b)
        {
            ++a;
            ++b;
        }
        return static_cast<i32>(static_cast<u8>(*a)) - static_cast<i32>(static_cast<u8>(*b));
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncmp                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare up to n characters of two strings.                        *
     *                                                                              *
     * *****************************************************************************/
    i32 strncmp(const char *a, const char *b, usize n) noexcept
    {
        for (usize i = 0; i < n; ++i)
        {
            if (a[i] != b[i])
                return static_cast<i32>(static_cast<u8>(a[i])) - static_cast<i32>(static_cast<u8>(b[i]));
            if (!a[i])
                return 0;
        }
        return 0;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcasecmp                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Case-insensitive comparison of two null-terminated strings.       *
     *                                                                              *
     * *****************************************************************************/
    i32 strcasecmp(const char *a, const char *b) noexcept
    {
        while (*a && to_lower(*a) == to_lower(*b))
        {
            ++a;
            ++b;
        }
        return static_cast<i32>(static_cast<u8>(to_lower(*a))) - static_cast<i32>(static_cast<u8>(to_lower(*b)));
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncasecmp                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Case-insensitive comparison of up to n characters.                *
     *                                                                              *
     * *****************************************************************************/
    i32 strncasecmp(const char *a, const char *b, usize n) noexcept
    {
        for (usize i = 0; i < n; ++i)
        {
            char ca = to_lower(a[i]);
            char cb = to_lower(b[i]);
            if (ca != cb)
                return static_cast<i32>(static_cast<u8>(ca)) - static_cast<i32>(static_cast<u8>(cb));
            if (!a[i])
                return 0;
        }
        return 0;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy src into dst including null terminator. Prefer strlcpy.      *
     *                                                                              *
     * *****************************************************************************/
    char *strcpy(char *dst, const char *src) noexcept
    {
        char *d = dst;
        while ((*d++ = *src++))
        {
        }
        return dst;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncpy                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy at most n bytes of src into dst. Pads with null bytes.       *
     *            Does not guarantee null termination. Prefer strlcpy.              *
     *                                                                              *
     * *****************************************************************************/
    char *strncpy(char *dst, const char *src, usize n) noexcept
    {
        usize i = 0;
        for (; i < n && src[i]; ++i)
            dst[i] = src[i];
        for (; i < n; ++i)
            dst[i] = '\0';
        return dst;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlcpy                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy src into dst writing at most (size - 1) chars, always null   *
     *            terminating. Returns length of src. Prefer over strcpy.           *
     *                                                                              *
     * *****************************************************************************/
    usize strlcpy(char *dst, const char *src, usize size) noexcept
    {
        usize src_len = strlen(src);
        if (size == 0)
            return src_len;

        usize copy = src_len < (size - 1) ? src_len : (size - 1);
        memcpy(dst, src, copy);
        dst[copy] = '\0';
        return src_len;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcat                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append src to dst. dst must have enough space. Prefer strlcat.    *
     *                                                                              *
     * *****************************************************************************/
    char *strcat(char *dst, const char *src) noexcept
    {
        char *d = dst + strlen(dst);
        while ((*d++ = *src++))
        {
        }
        return dst;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncat                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append at most n characters of src to dst.                        *
     *                                                                              *
     * *****************************************************************************/
    char *strncat(char *dst, const char *src, usize n) noexcept
    {
        char *d = dst + strlen(dst);
        usize i = 0;
        for (; i < n && src[i]; ++i)
            d[i] = src[i];
        d[i] = '\0';
        return dst;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlcat                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append src to dst writing at most (size - strlen(dst) - 1) chars. *
     *            Always null terminates. Returns total length attempted.           *
     *                                                                              *
     * *****************************************************************************/
    usize strlcat(char *dst, const char *src, usize size) noexcept
    {
        usize dst_len = strnlen(dst, size);
        usize src_len = strlen(src);
        if (dst_len >= size)
            return dst_len + src_len;

        usize space = size - dst_len - 1;
        usize copy = src_len < space ? src_len : space;
        memcpy(dst + dst_len, src, copy);
        dst[dst_len + copy] = '\0';
        return dst_len + src_len;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strchr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of c in s. Returns pointer or nullptr.      *
     *                                                                              *
     * *****************************************************************************/
    const char *strchr(const char *s, char c) noexcept
    {
        for (; *s; ++s)
            if (*s == c)
                return s;
        return (c == '\0') ? s : nullptr;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strrchr                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find last occurrence of c in s. Returns pointer or nullptr.       *
     *                                                                              *
     * *****************************************************************************/
    const char *strrchr(const char *s, char c) noexcept
    {
        const char *last = nullptr;
        for (; *s; ++s)
            if (*s == c)
                last = s;
        return (c == '\0') ? s : last;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strstr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of needle in haystack.                      *
     *            Returns pointer to it or nullptr.                                 *
     *                                                                              *
     * *****************************************************************************/
    const char *strstr(const char *haystack, const char *needle) noexcept
    {
        if (!*needle)
            return haystack;
        for (; *haystack; ++haystack)
        {
            const char *h = haystack;
            const char *n = needle;
            while (*h && *n && *h == *n)
            {
                ++h;
                ++n;
            }
            if (!*n)
                return haystack;
        }
        return nullptr;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strnstr                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of needle in at most n bytes of haystack.   *
     *            Returns pointer to it or nullptr.                                 *
     *                                                                              *
     * *****************************************************************************/
    const char *strnstr(const char *haystack, const char *needle, usize n) noexcept
    {
        usize needle_len = strlen(needle);
        if (needle_len == 0)
            return haystack;
        if (n < needle_len)
            return nullptr;

        for (usize i = 0; i <= n - needle_len; ++i)
        {
            if (strncmp(haystack + i, needle, needle_len) == 0)
                return haystack + i;
        }
        return nullptr;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strpbrk                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first character in s that appears in accept.                 *
     *            Returns pointer to it or nullptr.                                 *
     *                                                                              *
     * *****************************************************************************/
    const char *strpbrk(const char *s, const char *accept) noexcept
    {
        for (; *s; ++s)
            for (const char *a = accept; *a; ++a)
                if (*s == *a)
                    return s;
        return nullptr;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strspn                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return length of leading segment of s made entirely of chars      *
     *            found in accept.                                                  *
     *                                                                              *
     * *****************************************************************************/
    usize strspn(const char *s, const char *accept) noexcept
    {
        usize n = 0;
        for (; s[n]; ++n)
        {
            bool found = false;
            for (const char *a = accept; *a; ++a)
                if (s[n] == *a)
                {
                    found = true;
                    break;
                }
            if (!found)
                break;
        }
        return n;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcspn                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return length of leading segment of s made entirely of chars      *
     *            NOT found in reject.                                              *
     *                                                                              *
     * *****************************************************************************/
    usize strcspn(const char *s, const char *reject) noexcept
    {
        usize n = 0;
        for (; s[n]; ++n)
            for (const char *r = reject; *r; ++r)
                if (s[n] == *r)
                    return n;
        return n;
    }

} // namespace tklib