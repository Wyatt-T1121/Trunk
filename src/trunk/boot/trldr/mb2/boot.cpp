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
 *  PURPOSE : First C++ Boot-level code to be called.                           *
 * *****************************************************************************/

#include <trunk/boot/mb2/boot.h>
#include <trunk/boot/mm/b_mmap.h>
#include <trunk/boot/verify/b_verify.h>

#include <trunk/drivers/serial/serial.h>
#include <trunk/tros/kern/kabort.h>

#include <assert.h>

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
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : Trkload                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Called from TrSystemStartup. Builds BootInfo struct               *
     * *****************************************************************************/
    extern "C" void Trkload(u32 mb2_magic, u32 mb2_phys) noexcept
    {
        // TODO: IM PLANNING ON WRITING A BASIC NO BUFFER UART DRIVER FOR BOOT STAGE
        // THIS IS THE ACTUAL DRIVER, THIS CALL WILL BE REMOVED AND REPLACED WITH THE NEW BOOT CODE DRIVER.
        drivers::serial::serial_init();

        if (!VerifyMB2(mb2_magic, mb2_phys))
            kernel::kabort("Fatal: Multiboot2 verification failed. Magic number or alignment mismatch.");

        parse_mb2(static_cast<uptr>(mb2_phys), g_boot_info);

        TrkStartup(g_boot_info);

        ASSERT(false, "TrkStartup() suddenly dropped: Trkload()");
    }

} // namespace trunk::boot