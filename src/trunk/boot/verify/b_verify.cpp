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
 *  PURPOSE : Boot-stage validation routines.                                   *
 *            All pre-kernel sanity checks live here. boot.cpp calls these      *
 *            and halts immediately on any failure.                             *
 *                                                                              *
 * *****************************************************************************/

#include <trunk/boot/verify/b_verify.h>

namespace trunk::boot
{

    static constexpr u32 MB2_MAGIC = 0x36d76289;
    static constexpr u32 MB2_PTR_MIN = 0x1000;      // Below this is never valid
    static constexpr u32 MB2_PTR_ALIGN = 8;         // MB2 spec requires 8-byte alignment
    static constexpr u32 BOOT_PHYS_BASE = 0x100000; // trboot.elf physical base
    static constexpr u32 BOOT_PHYS_END = 0x200000;  // trboot.elf physical ceiling
    static constexpr u64 KERN_VMA_BASE = 0xFFFFFFFF80000000ULL;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_mb2_magic                                                  *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Check the MB2 handoff magic value matches the spec.               *
     *                                                                              *
     * *****************************************************************************/
    bool verify_mb2_magic(u32 magic) noexcept
    {
        return magic == MB2_MAGIC;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_mb2_ptr                                                    *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Sanity-check the MB2 info struct pointer before walking it.       *
     *                                                                              *
     * *****************************************************************************/
    bool verify_mb2_ptr(u32 phys) noexcept
    {
        if (phys < MB2_PTR_MIN)
            return false;
        if ((phys & (MB2_PTR_ALIGN - 1)) != 0)
            return false;
        return true;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_module_range                                               *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Validate the GRUB module range is non-zero, ordered, and does     *
     *            not overlap the physical boot stage region.                       *
     *                                                                              *
     * *****************************************************************************/
    bool verify_module_range(u32 mod_start, u32 mod_end) noexcept
    {
        if (mod_start == 0)
            return false;

        if (mod_end <= mod_start)
            return false;

        // Overlap check against trboot.elf physical region.
        // Reject if the module starts inside the boot region
        // or the boot region starts inside the module.
        if (mod_start < BOOT_PHYS_END && mod_end > BOOT_PHYS_BASE)
            return false;

        return true;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : verify_elf_result                                                 *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Confirm the ELF load succeeded and the entry point is in the      *
     *            higher-half virtual range where the kernel is expected to live.   *
     *                                                                              *
     * *****************************************************************************/
    bool verify_elf_result(const ElfResult &result) noexcept
    {
        if (result.error != ElfError::None)
            return false;

        if (result.entry < KERN_VMA_BASE)
            return false;

        return true;
    }

} // namespace trunk::boot