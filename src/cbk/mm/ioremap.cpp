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

namespace cbk::mem
{
    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                          *
     * FUNC    : MmIoRemap                                                          *
     * DATE    : 2026                                                               *
     * PURPOSE : Maps physical hardware registers into virtual space                *
     ********************************************************************************/
    NO_DISCARD PVOID
    MmIoRemap(PMM_ADDRESS_SPACE address_space,
              QWORD phys_addr,
              SIZE_T size,
              QWORD flags,
              PMMVAD blank_node) noexcept
    {
        if (address_space == nullptr || size == 0 || blank_node == nullptr)
            return nullptr;

        SIZE_T page_cnt  = (size + PAGE_SIZE - 1) / PAGE_SIZE;
        QWORD target_vpn = MmVadFindFreeGap(address_space, page_cnt, FALSE);
        if (target_vpn == 0)
            return nullptr;

        QWORD io_flags = flags | PAGE_CACHE_DISABLE | PAGE_WRITE_THROUGH;
        PMMVAD initialized_vad =
            MmVadInitializeNode(blank_node, target_vpn, page_cnt, static_cast<ULONG>(io_flags));
        if (initialized_vad == nullptr)
            return nullptr;

        CBKSTATUS status = MmVadInsertNode(address_space, initialized_vad);
        if (status != STATUS_SUCCESS)
            return nullptr;

        QWORD vaddr = target_vpn * PAGE_SIZE;
        status      = MmMapRange4K(vaddr, phys_addr, page_cnt * PAGE_SIZE, io_flags);
        if (status != STATUS_SUCCESS) {
            MmVadDeleteNode(address_space, initialized_vad);
            return nullptr;
        }

        return reinterpret_cast<PVOID>(vaddr);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                          *
     * FUNC    : MmIoUnRemap                                                        *
     * DATE    : 2026                                                               *
     * PURPOSE : Unmaps previously allocated hardware address range                 *
     ********************************************************************************/
    VOID
    MmIoUnRemap(PMM_ADDRESS_SPACE address_space, PVOID virt_addr, SIZE_T size) noexcept
    {
        if (address_space == nullptr || virt_addr == nullptr || size == 0)
            return;

        QWORD vaddr      = reinterpret_cast<QWORD>(virt_addr);
        QWORD target_vpn = vaddr / PAGE_SIZE;

        PMMVAD found_vad = MmVadFindNode(address_space, target_vpn);
        if (found_vad == nullptr || found_vad->starting_vpn != target_vpn)
            return;

        SIZE_T page_cnt  = (size + PAGE_SIZE - 1) / PAGE_SIZE;
        CBKSTATUS status = MmUnmapRange4K(vaddr, page_cnt * PAGE_SIZE);
        ASSERT(status == STATUS_SUCCESS, "MmIoUnRemap: FAILED TO UNMAP RANGE (4K)");

        MmVadDeleteNode(address_space, found_vad);
    }

} // namespace cbk::mem