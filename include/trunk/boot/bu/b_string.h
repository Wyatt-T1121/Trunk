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
 *  PURPOSE : Freestanding memory utilities for the boot stage.                 *
 *            trboot.elf is statically isolated from tklib — it cannot link     *
 *            against tklib.a. These provide the minimal memory primitives      *
 *            Boot.cpp needs to implement the ELF loader.                       *
 *            Same names as tklib — different namespace, no collision.          *
 *                                                                              *
 * *****************************************************************************/

#pragma once

#include <types.h>

namespace trunk::boot
{

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcpy                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Copy n bytes from src to dst. Regions must not overlap.           *
     *            Used by the ELF loader to copy PT_LOAD segments into RAM.         *
     *                                                                              *
     * *****************************************************************************/
    void *memcpy(void *dst, const void *src, usize n) noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memset                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Fill n bytes of dst with val.                                     *
     *            Used by the ELF loader to zero BSS regions of troskern.elf.       *
     *                                                                              *
     * *****************************************************************************/
    void *memset(void *dst, u8 val, usize n) noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : memcmp                                                            *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Compare n bytes of a and b.                                       *
     *            Returns 0 if equal, <0 if a < b, >0 if a > b.                     *
     *                                                                              *
     * *****************************************************************************/
    [[nodiscard]] int memcmp(const void *a, const void *b, usize n) noexcept;

} // namespace trunk::boot