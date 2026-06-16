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
 *  PURPOSE : Boot-stage validation routines.                                   *
 * *****************************************************************************/
#include <trunk/boot/trldr/safety/verify.h>

namespace trunk::boot
{

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_mb2_magic                                                  *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Check the MB2 handoff magic value matches the spec.               *
     * *****************************************************************************/
    bool verify_mb2_magic(u32 magic) noexcept
    {
        return magic == MB2_MAGIC;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_mb2_ptr                                                    *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Sanity-check the MB2 info struct pointer before walking it.       *
     * *****************************************************************************/
    bool verify_mb2_ptr(u32 phys) noexcept
    {
        if (phys < MB2_PTR_MIN)
            return false;
        if ((phys & (MB2_PTR_ALIGN - 1)) != 0)
            return false;
        return true;
    }

} // namespace trunk::boot