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

#include <boot/trldr/mb2/boot.h>
#include <boot/trldr/mem/mmap.h>
#include <boot/trldr/safety/bdump.h>
#include <boot/trldr/safety/verify.h>

#include <cbk/kern/kabort.h>
#include <drivers/serial/serial.h>

#include <assert.h>
#include <version.h>

namespace trunk::boot
{
    extern "C" NO_RETURN VOID CbkStartup(const BootInfo &info) noexcept;

    static BootInfo g_boot_info{};

    NO_DISCARD const char *MemoryTypeString(MemoryType type) noexcept
    {
        switch (type) {
        case MemoryType::Available:
            return "Available";
        case MemoryType::Reserved:
            return "Reserved";
        case MemoryType::AcpiReclaimable:
            return "ACPI Reclaimable";
        case MemoryType::AcpiNvs:
            return "ACPI NVS";
        case MemoryType::BadRam:
            return "Bad RAM";
        default:
            return "Unknown";
        }
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : VerifyMB2                                                         *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Wrapper for verify_mb2_(NAME)                                     *
     *                                                                              *
     * *****************************************************************************/
    NO_DISCARD BOOL VerifyMB2(DWORD mb2_m, DWORD mb2_ph) noexcept
    {
        if (!VerifyMb2Magic(mb2_m) || !VerifyMb2Pointer(mb2_ph))
            return false;
        return true;
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : CbkLoad                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Called from CbkSystemStartup. Builds BootInfo struct              *
     * *****************************************************************************/
    extern "C" VOID CbkLoad(DWORD mb2_magic, DWORD mb2_phys) noexcept
    {
        // TODO: IM PLANNING ON WRITING A BASIC NO BUFFER UART DRIVER FOR BOOT STAGE
        // THIS IS THE ACTUAL DRIVER, THIS CALL WILL BE REMOVED AND REPLACED WITH THE NEW BOOT CODE
        // DRIVER.
        drivers::serial::SerialInit();

        if (!VerifyMB2(mb2_magic, mb2_phys))
            kernel::KAbort(
                "Fatal: Multiboot2 verification failed. Magic number or alignment mismatch.");

        ParseMb2(static_cast<ULONG_PTR>(mb2_phys), g_boot_info);
        BDump(g_boot_info);

        CbkStartup(g_boot_info);

        ASSERT(false, "CbkStartup() suddenly dropped: CbkLoad()");
    }

} // namespace trunk::boot