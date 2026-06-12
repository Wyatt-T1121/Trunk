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
 *  PURPOSE : Multiboot2 bridge. Validates the bootloader handoff, parses       *
 *            the MB2 info struct into a clean BootInfo, then calls             *
 *            TrkStartup. Single-ELF design, GRUB loads trunk.elf directly,     *
 *            no module loading or ELF parsing required.                        *
 *                                                                              *
 *                                                                              *
 * *****************************************************************************/

#include <trunk/boot/mb2/boot.h>

#include <trunk/boot/mm/b_mmap.h>
#include <trunk/boot/verify/b_verify.h>

namespace trunk::boot
{
    extern "C" [[noreturn]] void TrkStartup(const BootInfo &info) noexcept;

    static BootInfo g_boot_info{};

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : VerifyMB2                                                         *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Wrapper for verify_mb2_(NAME)                                     *
     *                                                                              *
     * *****************************************************************************/
    [[nodiscard]] bool VerifyMB2(u32 mb2_m, u32 mb2_ph) noexcept
    {
        if (!verify_mb2_magic(mb2_m) || !verify_mb2_ptr(mb2_ph))
            return false;
        return true;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : Trkload                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Called from TrSystemStartup. Validates the MB2 handoff,           *
     *            builds BootInfo from the MB2 info struct, then calls              *
     *            TrkStartup. Never returns.                                        *
     *                                                                              *
     * *****************************************************************************/
    extern "C" [[noreturn]]
    void Trkload(u32 mb2_magic, u32 mb2_phys) noexcept
    {
        if (!VerifyMB2(mb2_magic, mb2_phys))
            for (;;)
                asm volatile("cli; hlt");

        parse_mb2(static_cast<uptr>(mb2_phys), g_boot_info);

        TrkStartup(g_boot_info);

        for (;;)
        {
            asm volatile("cli; hlt");
        }
    }

} // namespace trunk::boot