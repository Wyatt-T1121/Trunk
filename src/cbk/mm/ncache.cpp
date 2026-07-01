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
 *  MODULE  : Non-cached manager                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : High-level Non-Cached(NCACHE) manager                              *
 ********************************************************************************/
#include <cbk/mm/ncache.h>

namespace cbk::mem
{
    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiPurgePageMapping                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Purge a page mapping (or allocation)                               *
         ********************************************************************************/
        VOID
        MiPurgePageMapping(QWORD vaddr, PFN_NUM pfn) noexcept
        {
            CBKSTATUS status = MmUnmapPage4K(vaddr);
            ASSERT(status == STATUS_SUCCESS, "MiPurgePageMapping: UNMAP FAILURE!!");
            TlbFlushPage(vaddr);
            MmDereferencePage(pfn);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiUnwindAllocation                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Unwind an allocation                                               *
         ********************************************************************************/
        VOID
        MiUnwindAllocation(QWORD vstart, QWORD curr_vaddr, SIZE_T allocated_cnt) noexcept
        {
            SIZE_T i = allocated_cnt;
            while (i > 0) {
                --i;
                if (vstart != 0) {
                    curr_vaddr       -= PAGE_SIZE;
                    QWORD alloc_phys  = MmTranslateVirtualToPhysical(curr_vaddr);
                    if (alloc_phys != PHYS_ADDR_MAX)
                        MiPurgePageMapping(curr_vaddr, alloc_phys >> PAGE_SHIFT);
                }
            }
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiPopulateRange                                                    *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Populate a range                                                   *
         ********************************************************************************/
        CBKSTATUS
        MiPopulateRange(QWORD vstart, SIZE_T page_cnt, QWORD cache_flags) noexcept
        {
            QWORD curr_vaddr = vstart;

            for (SIZE_T i = 0; i < page_cnt; ++i) {
                PFN_NUM new_pfn = MmAllocPage(static_cast<ULONG>(MC_TYPE::SYSTEM));

                if (new_pfn == 0) {
                    MiUnwindAllocation(vstart, curr_vaddr, i);
                    return STATUS_INSUFFICIENT_RESOURCES;
                }

                CBKSTATUS status = MmMapPage4K(curr_vaddr, new_pfn << PAGE_SHIFT, cache_flags);
                if (status != STATUS_SUCCESS) {
                    MmDereferencePage(new_pfn);
                    MiUnwindAllocation(vstart, curr_vaddr, i);
                    return status;
                }

                TlbFlushPage(curr_vaddr);
                curr_vaddr += PAGE_SIZE;
            }

            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MiTeardownRange                                                     *
         * DATE    : 2026                                                                *
         * PURPOSE : Clears page mappings page-by-page across the requested span         *
         ********************************************************************************/
        VOID
        MiTeardownRange(QWORD vstart, SIZE_T size) noexcept
        {
            SIZE_T page_cnt  = size / PAGE_SIZE;
            QWORD curr_vaddr = vstart;

            for (SIZE_T i = 0; i < page_cnt; ++i) {
                QWORD phys_page = MmTranslateVirtualToPhysical(curr_vaddr);
                if (phys_page != PHYS_ADDR_MAX) {
                    MiPurgePageMapping(curr_vaddr, phys_page >> PAGE_SHIFT);
                }
                curr_vaddr += PAGE_SIZE;
            }
        }

    } // namespace

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmNcacheAllocateBuffer                                              *
     * DATE    : 2026                                                                *
     * PURPOSE : Reserves virtual space via VMM and wires uncached physical entries  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmNcacheAllocateBuffer(PMM_ADDRESS_SPACE address_space,
                           SIZE_T size,
                           QWORD tag,
                           PNCACHE_DESCRIPTOR out_desc) noexcept
    {
        if (address_space == nullptr || out_desc == nullptr || size == 0 ||
            size > (limits::SIZE_T_max - PAGE_SIZE))
            return STATUS_INVALID_PARAMETER;

        PVOID base_address = nullptr;
        SIZE_T region_size = size;
        ULONG protect      = PAGE_CACHE_DISABLE | PAGE_WRITE_THROUGH;

        CBKSTATUS status = MmAllocateVirtualMemory(address_space,
                                                   &base_address,
                                                   &region_size,
                                                   MEM_RESERVE | MEM_COMMIT,
                                                   protect);
        if (status != STATUS_SUCCESS)
            return status;

        QWORD vstart    = reinterpret_cast<QWORD>(base_address);
        SIZE_T page_cnt = region_size / PAGE_SIZE;

        status = MiPopulateRange(vstart, page_cnt, protect);
        if (status != STATUS_SUCCESS) {
            CBKSTATUS free_status =
                MmFreeVirtualMemory(address_space, &base_address, &region_size, MEM_RELEASE);
            ASSERT(free_status == STATUS_SUCCESS, "MmNcacheAllocateBuffer: Unwind free failed!");
            return status;
        }

        MemoryFence();

        out_desc->virt_address = base_address;
        out_desc->size         = region_size;
        out_desc->phys_base    = MmTranslateVirtualToPhysical(vstart);
        out_desc->alloc_tag    = tag;

        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmNcacheFreeBuffer                                                  *
     * DATE    : 2026                                                                *
     * PURPOSE : Tears down translations, releases frames, and frees VMM region      *
     ********************************************************************************/
    VOID
    MmNcacheFreeBuffer(PMM_ADDRESS_SPACE address_space, PNCACHE_DESCRIPTOR descriptor) noexcept
    {
        if (address_space == nullptr || descriptor == nullptr ||
            descriptor->virt_address == nullptr || descriptor->size == 0)
            return;

        PVOID base_address = descriptor->virt_address;
        SIZE_T region_size = descriptor->size;
        QWORD vstart       = reinterpret_cast<QWORD>(base_address);

        MiTeardownRange(vstart, region_size);

        CBKSTATUS free_status =
            MmFreeVirtualMemory(address_space, &base_address, &region_size, MEM_RELEASE);
        ASSERT(free_status == STATUS_SUCCESS,
               "NcacheFreeBuffer: Backing address space release failed!");
        MemoryFence();

        descriptor->virt_address = nullptr;
        descriptor->phys_base    = 0;
        descriptor->size         = 0;
    }

} // namespace cbk::mem
