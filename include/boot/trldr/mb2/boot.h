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

#include <attributes.h>
#include <types.h>

namespace cbk::boot
{
    enum class MEMORY_TYPE : DWORD
    {
        Available       = 0,
        Reserved        = 1,
        AcpiReclaimable = 2,
        AcpiNvs         = 3,
        BadRam          = 4,
    };

    struct MemoryRegion
    {
        QWORD base;
        QWORD length;
        MEMORY_TYPE type;

        NO_DISCARD constexpr QWORD
        End() const noexcept
        {
            return base + length;
        }

        NO_DISCARD constexpr BOOL
        Available() const noexcept
        {
            return type == MEMORY_TYPE::Available;
        }
    };

    struct BootInfo
    {
        static constexpr SIZE_T BOOTLOADER_NAME_MAX = 64;
        static constexpr SIZE_T MAX_MMAP_ENTRIES    = 64;

        MemoryRegion mmap[MAX_MMAP_ENTRIES]       = {};
        SIZE_T mmap_count                         = 0;
        CHAR bootloader_name[BOOTLOADER_NAME_MAX] = {};

        /* ***************************************************************************
         *  AUTHOR  : Trollycat                                                      *
         *  FUNC    : TotalAvailableBytes                                            *
         *  DATE    : 2026                                                           *
         *  PURPOSE : Sum of all Available region lengths. Used by the PMM.          *
         ****************************************************************************/
        NO_DISCARD QWORD
        TotalAvailableBytes() const noexcept
        {
            QWORD total = 0;
            for (SIZE_T i = 0; i < mmap_count; ++i)
                if (mmap[i].Available())
                    total += mmap[i].length;
            return total;
        }
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InMemoryTypeToString                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return a short string describing a MEMORY_TYPE value.              *
     ********************************************************************************/
    NO_DISCARD PCSTR
    InMemoryTypeToString(MEMORY_TYPE type) noexcept;

} // namespace cbk::boot