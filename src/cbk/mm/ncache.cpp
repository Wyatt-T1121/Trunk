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
         *  FUNC    : InternalPurgePageMapping                                           *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Purge a page mapping (or allocation)                               *
         ********************************************************************************/
        VOID
        InternalPurgePageMapping(QWORD vaddr, PFN_NUM pfn) noexcept
        {
            CBKSTATUS status = UnmapPage4K(vaddr);
            ASSERT(status == STATUS_SUCCESS, "InternalPurgePageMapping: UNMAP FAILURE!!");
            TlbFlushPage(vaddr);
            MmDereferencePage(pfn);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InternalUnwindAllocation                                           *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Unwind an allocation                                               *
         ********************************************************************************/
        VOID
        InternalUnwindAllocation(QWORD vstart, QWORD curr_vaddr, SIZE_T allocated_cnt) noexcept
        {
            SIZE_T i = allocated_cnt;
            while (i > 0) {
                --i;
                if (vstart != 0) {
                    curr_vaddr       -= PAGE_SIZE;
                    QWORD alloc_phys  = TranslateVirtualToPhysical(curr_vaddr);
                    if (alloc_phys != PHYS_ADDR_MAX)
                        InternalPurgePageMapping(curr_vaddr, alloc_phys >> PAGE_SHIFT);
                }
            }
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InternalPopulateRange                                              *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Populate a range                                                   *
         ********************************************************************************/
        CBKSTATUS
        InternalPopulateRange(QWORD vstart, SIZE_T page_cnt, QWORD cache_flags) noexcept
        {
            QWORD curr_vaddr = vstart;

            for (SIZE_T i = 0; i < page_cnt; ++i) {
                PFN_NUM new_pfn = MmAllocPage(static_cast<ULONG>(MC_TYPE::SYSTEM));

                if (new_pfn == 0) {
                    InternalUnwindAllocation(vstart, curr_vaddr, i);
                    return STATUS_INSUFFICIENT_RESOURCES;
                }

                CBKSTATUS status = MapPage4K(curr_vaddr, new_pfn << PAGE_SHIFT, cache_flags);
                if (status != STATUS_SUCCESS) {
                    MmDereferencePage(new_pfn);
                    InternalUnwindAllocation(vstart, curr_vaddr, i);
                    return status;
                }

                TlbFlushPage(curr_vaddr);
                curr_vaddr += PAGE_SIZE;
            }

            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : InternalTeardownRange                                               *
         * DATE    : 2026                                                                *
         * PURPOSE : Clears page mappings page-by-page across the requested span         *
         ********************************************************************************/
        VOID
        InternalTeardownRange(QWORD vstart, SIZE_T size) noexcept
        {
            SIZE_T page_cnt  = size / PAGE_SIZE;
            QWORD curr_vaddr = vstart;

            for (SIZE_T i = 0; i < page_cnt; ++i) {
                QWORD phys_page = TranslateVirtualToPhysical(curr_vaddr);
                if (phys_page != PHYS_ADDR_MAX) {
                    InternalPurgePageMapping(curr_vaddr, phys_page >> PAGE_SHIFT);
                }
                curr_vaddr += PAGE_SIZE;
            }
        }
    } // namespace

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : NcacheAllocateBuffer                                                *
     * DATE    : 2026                                                                *
     * PURPOSE : Reserves virtual space via VMM and wires uncached physical entries  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    NcacheAllocateBuffer(PMM_ADDRESS_SPACE address_space,
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

        status = InternalPopulateRange(vstart, page_cnt, protect);
        if (status != STATUS_SUCCESS) {
            CBKSTATUS free_status =
                MmFreeVirtualMemory(address_space, &base_address, &region_size, MEM_RELEASE);
            ASSERT(free_status == STATUS_SUCCESS, "NcacheAllocateBuffer: Unwind free failed!");
            return status;
        }

        MemoryFence();

        out_desc->virt_address = base_address;
        out_desc->size         = region_size;
        out_desc->phys_base    = TranslateVirtualToPhysical(vstart);
        out_desc->alloc_tag    = tag;

        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : NcacheFreeBuffer                                                    *
     * DATE    : 2026                                                                *
     * PURPOSE : Tears down translations, releases frames, and frees VMM region      *
     ********************************************************************************/
    VOID
    NcacheFreeBuffer(PMM_ADDRESS_SPACE address_space, PNCACHE_DESCRIPTOR descriptor) noexcept
    {
        if (address_space == nullptr || descriptor == nullptr ||
            descriptor->virt_address == nullptr || descriptor->size == 0)
            return;

        PVOID base_address = descriptor->virt_address;
        SIZE_T region_size = descriptor->size;
        QWORD vstart       = reinterpret_cast<QWORD>(base_address);

        InternalTeardownRange(vstart, region_size);

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
