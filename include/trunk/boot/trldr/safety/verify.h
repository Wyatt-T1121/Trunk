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
 *  MODULE  : Bootstrapping                                                     *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Boot-stage validation functions                                   *
 * *****************************************************************************/

#pragma once

#include <macros.h>
#include <types.h>

namespace trunk::boot
{
    inline constexpr u32 MB2_MAGIC     = 0x36d76289;
    inline constexpr u32 MB2_PTR_MIN   = 0x1000;
    inline constexpr u32 MB2_PTR_ALIGN = 8;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_mb2_magic                                                  *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Confirm the MB2 magic value left in EAX by GRUB is correct.       *
     * *****************************************************************************/
    NO_DISCARD bool verify_mb2_magic(u32 magic) noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_mb2_ptr                                                    *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Confirm the MB2 info pointer is non-null, above the first page,   *
     *            and 8-byte aligned as required by the MB2 specification.          *
     * *****************************************************************************/
    NO_DISCARD bool verify_mb2_ptr(u32 phys) noexcept;

} // namespace trunk::boot