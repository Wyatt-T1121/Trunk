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
 *  PURPOSE : Stores BootInfo - All information pulled from boot stage.          *
 ********************************************************************************/

#pragma once

#include <types.h>

namespace trunk::boot
{
    enum class MemoryType : u32
    {
        Available = 0,
        Reserved = 1,
        AcpiReclaimable = 2,
        AcpiNvs = 3,
        BadRam = 4,
    };

    struct MemoryRegion
    {
        u64 base;
        u64 length;
        MemoryType type;

        [[nodiscard]] constexpr u64 end() const noexcept { return base + length; }
        [[nodiscard]] constexpr bool available() const noexcept { return type == MemoryType::Available; }
    };

    struct BootInfo
    {
        static constexpr usize BOOTLOADER_NAME_MAX = 64;
        static constexpr usize MAX_MMAP_ENTRIES = 64;

        MemoryRegion mmap[MAX_MMAP_ENTRIES] = {};
        usize mmap_count = 0;
        char bootloader_name[BOOTLOADER_NAME_MAX] = {};

        /* ***************************************************************************
         *  AUTHOR  : Trollycat                                                      *
         *  FUNC    : total_available_bytes                                          *
         *  DATE    : 2026                                                           *
         *  PURPOSE : Sum of all Available region lengths. Used by the PMM.          *
         ****************************************************************************/
        [[nodiscard]]
        u64 total_available_bytes() const noexcept
        {
            u64 total = 0;
            for (usize i = 0; i < mmap_count; ++i)
                if (mmap[i].available())
                    total += mmap[i].length;
            return total;
        }
    };

} // namespace trunk::boot