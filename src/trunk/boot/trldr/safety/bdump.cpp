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

#include <trunk/boot/trldr/safety/bdump.h>
#include <trunk/boot/trldr/mb2/boot.h>
#include <trunk/drivers/serial/serial.h>
#include <tklib/formatter.h>

namespace serial = trunk::drivers::serial;

namespace trunk::boot
{

    static char s_buf[67];

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : bdump                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Dump BootInfo contents to serial output.                           *
     ********************************************************************************/
    void bdump(const BootInfo &info) noexcept
    {
        serial::serial_puts("Bootloader: ");
        serial::serial_puts(info.bootloader_name[0] ? info.bootloader_name : "(unknown)");
        serial::serial_puts("\n");

        serial::serial_puts("Memory map (");
        tklib::fmt_dec(s_buf, sizeof(s_buf), info.mmap_count);
        serial::serial_puts(s_buf);
        serial::serial_puts(" entries):\n");

        for (usize i = 0; i < info.mmap_count; ++i)
        {
            const auto &region = info.mmap[i];

            serial::serial_puts("  ");
            tklib::fmt_hex(s_buf, sizeof(s_buf), region.base);
            serial::serial_puts(s_buf);
            serial::serial_puts(" - ");
            tklib::fmt_hex(s_buf, sizeof(s_buf), region.end());
            serial::serial_puts(s_buf);
            serial::serial_puts("  ");
            serial::serial_puts(memory_type_str(region.type));
            serial::serial_puts("\n");
        }

        serial::serial_puts("Total available: ");
        tklib::fmt_size(s_buf, sizeof(s_buf), info.total_available_bytes());
        serial::serial_puts(s_buf);
        serial::serial_puts("\n");
    }

} // namespace trunk::boot