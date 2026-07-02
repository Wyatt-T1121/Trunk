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

#include <cbk/bgchk/bug.h>
#include <drivers/serial/serial.h>

#include <assert.h>
#include <version.h>

namespace cbk::boot
{
    static BootInfo g_boot_info{};

    // Kernel entry point...
    // We could just include the header...
    // But I think external linking is probably the better choice here...
    EXTERN_C NO_RETURN VOID
    KeInitializeKernel(const BootInfo &info) noexcept;

    namespace
    {
        /* ******************************************************************************
         *  AUTHOR  : Trollycat                                                         *
         *  FUNC    : IniVerifyMultiboot2                                               *
         *  DATE    : 2026                                                              *
         *  PURPOSE : Wrapper for verify_mb2_(NAME)                                     *
         * *****************************************************************************/
        NO_DISCARD BOOL
        IniVerifyMultiboot2(DWORD mb2_m, DWORD mb2_ph) noexcept
        {
            if (!InVerifyMultiboot2Magic(mb2_m) || !InVerifyMultiboot2Pointer(mb2_ph))
                return FALSE;
            return TRUE;
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InMemoryTypeToString                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return a short string describing a MEMORY_TYPE value.              *
     ********************************************************************************/
    NO_DISCARD PCSTR
    InMemoryTypeToString(MEMORY_TYPE type) noexcept
    {
        switch (type) {
        case MEMORY_TYPE::Available:
            return "AVAILABLE";
        case MEMORY_TYPE::Reserved:
            return "RESERVED";
        case MEMORY_TYPE::AcpiReclaimable:
            return "ACPI RECLAIMABLE";
        case MEMORY_TYPE::AcpiNvs:
            return "ACPI NVS";
        case MEMORY_TYPE::BadRam:
            return "BAD RAM";
        default:
            return "UNKNOWN";
        }
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : InLoadKernel                                                      *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Called from KeSystemStartup. Builds BootInfo struct              *
     * *****************************************************************************/
    EXTERN_C VOID
    InLoadKernel(DWORD mb2_magic, DWORD mb2_phys) noexcept
    {
        // TODO: IM PLANNING ON WRITING A BASIC NO BUFFER UART DRIVER FOR BOOT STAGE
        // THIS IS THE ACTUAL DRIVER, THIS CALL WILL BE REMOVED AND REPLACED WITH THE NEW BOOT CODE
        // DRIVER.
        CBKSTATUS status = drivers::serial::SerialInit();

        ASSERT(status == STATUS_SUCCESS, "InLoadKernel: YOU SHOULDN'T BE HERE...");
        ASSERT(IniVerifyMultiboot2(mb2_magic, mb2_phys), "InLoadKernel: INVALID MULTIBOO2 2....");

        InParseMultiboot2(static_cast<ULONG_PTR>(mb2_phys), g_boot_info);
        InDumpBootInformation(g_boot_info);

        KeInitializeKernel(g_boot_info);

        ASSERT(FALSE, "KeInitializeKernel() suddenly dropped: InLoadKernel()");
    }

} // namespace cbk::boot