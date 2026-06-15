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
 *  PURPOSE : MB2 info struct parser for the boot stage.                        *
 * *****************************************************************************/

#pragma once

#include <types.h>
#include <trunk/boot/mb2/boot.h>

namespace trunk::boot
{
    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : parse_mb2                                                         *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Walk all MB2 tags from mb2_phys and populate info with the        *
     *            memory map and bootloader name.                                   *
     * *****************************************************************************/
    void parse_mb2(uptr mb2_phys, BootInfo &info) noexcept;

} // namespace trunk::boot