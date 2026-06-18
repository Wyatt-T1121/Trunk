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
#pragma once

#include <macros.h>
#include <types.h>

namespace tklib
{

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions must not overlap.           *
     * *****************************************************************************/
    PVOID memcpy(PVOID dst, LPCVOID src, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memset                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Fill n bytes of dst with value.                                   *
     * *****************************************************************************/
    PVOID memset(PVOID dst, BYTE value, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memmove                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions may overlap.                *
     * *****************************************************************************/
    PVOID memmove(PVOID dst, LPCVOID src, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcmp                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare n bytes of a and b.                                       *
     * *****************************************************************************/
    NO_DISCARD LONG memcmp(LPCVOID a, LPCVOID b, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memchr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of value in first n bytes of ptr.           *
     * *****************************************************************************/
    NO_DISCARD LPCVOID memchr(LPCVOID ptr, BYTE value, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlen                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return the length of a null-terminated string, not including      *
     *            the null terminator.                                              *
     * *****************************************************************************/
    NO_DISCARD SIZE_T strlen(const char *s) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strnlen                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return the length of s, stopping at max.                          *
     * *****************************************************************************/
    NO_DISCARD SIZE_T strnlen(const char *s, SIZE_T max) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcmp                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare two null-terminated strings.                              *
     * *****************************************************************************/
    NO_DISCARD LONG strcmp(const char *a, const char *b) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncmp                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare up to n characters of two strings.                        *
     * *****************************************************************************/
    NO_DISCARD LONG strncmp(const char *a, const char *b, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcasecmp                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Case-insensitive comparison of two null-terminated strings.       *
     * *****************************************************************************/
    NO_DISCARD LONG strcasecmp(const char *a, const char *b) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncasecmp                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Case-insensitive comparison of up to n characters.                *
     * *****************************************************************************/
    NO_DISCARD LONG strncasecmp(const char *a, const char *b, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy src into dst including null terminator. Prefer strlcpy.      *
     * *****************************************************************************/
    char *strcpy(char *dst, const char *src) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncpy                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy at most n bytes of src into dst.                             *
     * *****************************************************************************/
    char *strncpy(char *dst, const char *src, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlcpy                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy src into dst writing at most (size - 1) chars                *
     * *****************************************************************************/
    SIZE_T strlcpy(char *dst, const char *src, SIZE_T size) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcat                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append src to dst. dst must have enough space. Prefer strlcat.    *
     * *****************************************************************************/
    char *strcat(char *dst, const char *src) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strncat                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append at most n characters of src to dst.                        *
     * *****************************************************************************/
    char *strncat(char *dst, const char *src, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strlcat                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Append src to dst writing at most (size - strlen(dst) - 1).       *
     * *****************************************************************************/
    SIZE_T strlcat(char *dst, const char *src, SIZE_T size) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strchr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of c in s. Returns pointer or nullptr.      *
     * *****************************************************************************/
    NO_DISCARD const char *strchr(const char *s, char c) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strrchr                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find last occurrence of c in s. Returns pointer or nullptr.       *
     * *****************************************************************************/
    NO_DISCARD const char *strrchr(const char *s, char c) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strstr                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of needle in haystack.                      *
     * *****************************************************************************/
    NO_DISCARD const char *strstr(const char *haystack, const char *needle) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strnstr                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first occurrence of needle in at most n bytes of haystack.   *
     * *****************************************************************************/
    NO_DISCARD const char *strnstr(const char *haystack, const char *needle, SIZE_T n) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strpbrk                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Find first character in s that appears in accept.                 *
     * *****************************************************************************/
    NO_DISCARD const char *strpbrk(const char *s, const char *accept) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strspn                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return length of leading segment of s made entirely of chars      *
     *            found in accept.                                                  *
     * *****************************************************************************/
    NO_DISCARD SIZE_T strspn(const char *s, const char *accept) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : strcspn                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return length of leading segment of s made entirely of chars      *
     *            NOT found in reject.                                              *
     * *****************************************************************************/
    NO_DISCARD SIZE_T strcspn(const char *s, const char *reject) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_alpha                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is an alphabetic character.                      *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_alpha(char c) noexcept
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_digit                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is a decimal digit.                              *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_digit(char c) noexcept
    {
        return c >= '0' && c <= '9';
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_alnum                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is alphanumeric.                                 *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_alnum(char c) noexcept
    {
        return is_alpha(c) || is_digit(c);
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_space                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is a whitespace character.                       *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_space(char c) noexcept
    {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_upper                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is an uppercase letter.                          *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_upper(char c) noexcept
    {
        return c >= 'A' && c <= 'Z';
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_lower                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is a lowercase letter.                           *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_lower(char c) noexcept
    {
        return c >= 'a' && c <= 'z';
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_hex_digit                                                      *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is a valid hexadecimal digit.                    *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_hex_digit(char c) noexcept
    {
        return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : is_print                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if c is a printable character including space.        *
     * *****************************************************************************/
    NO_DISCARD constexpr BOOL is_print(char c) noexcept
    {
        return c >= ' ' && c <= '~';
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : to_upper                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Convert c to uppercase. Returns c unchanged if not lowercase.     *
     * *****************************************************************************/
    NO_DISCARD constexpr char to_upper(char c) noexcept
    {
        return is_lower(c) ? static_cast<char>(c - 32) : c;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : to_lower                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Convert c to lowercase. Returns c unchanged if not uppercase.     *
     * *****************************************************************************/
    NO_DISCARD constexpr char to_lower(char c) noexcept
    {
        return is_upper(c) ? static_cast<char>(c + 32) : c;
    }

} // namespace tklib