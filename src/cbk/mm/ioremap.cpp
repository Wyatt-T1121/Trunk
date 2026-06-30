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
 *  MODULE  : INPUT/OUTPUT REMAPPING                                             *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Memory-mapped INPUT/OUTPUT configurations                          *
 ********************************************************************************/
#include <cbk/mm/ioremap.h>

// VERY IMPORTANT NOTICE:
// IOREMAP REQUIRES A VIRTUAL MEMORY MANAGER TO BE PROPER...
// RIGHT NOW, WE DON'T HAVE ONE...
// WE ARE DOING A TRICK CALLED 'IDENTITY MAPPING'...
// vaddr = phys_addr...
// THIS IS NOT SAFE...

// THIS IMPLEMENTATION IS DEAD SIMPLE...
// NO SECURITY...
// WILL BE IMPROVED AFTER VMM...

namespace cbk::mem
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIoRemap                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Maps physical hardware registers into virtual space                *
     ********************************************************************************/
    NO_DISCARD PVOID MmIoRemap(QWORD phys_addr, SIZE_T size, QWORD flags) noexcept
    {
        QWORD io_flags = flags | PAGE_CACHE_DISABLE | PAGE_WRITE_THROUGH;
        QWORD vaddr    = phys_addr;

        CBKSTATUS status = MapRange4K(vaddr, phys_addr, size, io_flags);
        if (status != STATUS_SUCCESS)
            return nullptr;

        return reinterpret_cast<PVOID>(vaddr);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIoUnRemap                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmaps previously allocated hardware address range                 *
     ********************************************************************************/
    VOID MmIoUnRemap(PVOID virt_addr, SIZE_T size) noexcept
    {
        if (virt_addr == nullptr)
            return;

        QWORD vaddr = reinterpret_cast<QWORD>(virt_addr);
        UnmapRange4K(vaddr, size);
    }
} // namespace cbk::mem