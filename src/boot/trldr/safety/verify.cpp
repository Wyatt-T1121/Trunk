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
 *  PURPOSE : Boot-stage validation.                                            *
 * *****************************************************************************/
#include <boot/trldr/safety/verify.h>

namespace cbk::boot
{
    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : InVerifyMultiboot2Magic                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Check the MB2 handoff magic value matches the spec                *
     * *****************************************************************************/
    BOOL
    InVerifyMultiboot2Magic(DWORD magic) noexcept
    {
        return magic == MB2_MAGIC;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : InVerifyMultiboot2Pointer                                         *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Check the MB2 info struct pointer before walking it               *
     * *****************************************************************************/
    BOOL
    InVerifyMultiboot2Pointer(DWORD phys) noexcept
    {
        if (phys < MB2_PTR_MIN)
            return FALSE;
        if ((phys & (MB2_PTR_ALIGN - 1)) != 0)
            return FALSE;
        return TRUE;
    }

} // namespace cbk::boot