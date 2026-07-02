/* *******************************************************************************
 *                                                                               *
 *  Copyright 2026 Trollycat                                                     *
 *                                                                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");              *
 *  you may not use this file except in compliance with the License.             *
 *  You may obtain a copy of the License at                                      *
 *                                                                               *
 *      http://www.apache.org/licenses/LICENSE-2.0                               *
 *                                                                               *
 *  Unless required by applicable law or agreed to in writing, software          *
 *  distributed under the License is distributed on an "AS IS" BASIS,            *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     *
 *  See the License for the specific language governing permissions and          *
 *  limitations under the License.                                               *
 *                                                                               *
 *********************************************************************************
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Bootstrapping                                                      *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Boot information serial dump.                                      *
 ********************************************************************************/
#include <boot/trldr/mb2/boot.h>
#include <boot/trldr/safety/bdump.h>

#include <drivers/serial/serial.h>

#include <tklib/formatter.h>

namespace serial = cbk::drivers::serial;

namespace cbk::boot
{
    static CHAR s_buf[67];

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InDumpBootInformation                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Dump BootInfo contents to serial output.                           *
     ********************************************************************************/
    VOID
    InDumpBootInformation(const BootInfo &info) noexcept
    {
        serial::SerialPuts("Bootloader: ");
        serial::SerialPuts(info.bootloader_name[0] ? info.bootloader_name : "(unknown)");
        serial::SerialPuts("\n");

        serial::SerialPuts("Memory map (");
        tklib::fmt_dec(s_buf, sizeof(s_buf), info.mmap_count);
        serial::SerialPuts(s_buf);
        serial::SerialPuts(" entries):\n");

        for (SIZE_T i = 0; i < info.mmap_count; ++i) {
            const auto &region = info.mmap[i];

            serial::SerialPuts("  ");
            tklib::fmt_hex(s_buf, sizeof(s_buf), region.base);
            serial::SerialPuts(s_buf);
            serial::SerialPuts(" - ");
            tklib::fmt_hex(s_buf, sizeof(s_buf), region.End());
            serial::SerialPuts(s_buf);
            serial::SerialPuts("  ");
            serial::SerialPuts(InMemoryTypeToString(region.type));
            serial::SerialPuts("\n");
        }

        serial::SerialPuts("Total available: ");
        tklib::fmt_size(s_buf, sizeof(s_buf), info.TotalAvailableBytes());
        serial::SerialPuts(s_buf);
        serial::SerialPuts("\n");
    }

} // namespace cbk::boot