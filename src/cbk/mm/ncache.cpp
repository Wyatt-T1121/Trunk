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
        VOID InternalPurgePageMapping(QWORD vaddr, PFN_NUM pfn) noexcept
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
        VOID InternalUnwindAllocation(QWORD vstart, QWORD curr_vaddr, SIZE_T allocated_cnt) noexcept
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
        CBKSTATUS InternalPopulateRange(QWORD vstart, SIZE_T page_cnt, QWORD cache_flags,
                                        QWORD &out_phys_base) noexcept
        {
            QWORD curr_vaddr = vstart;

            for (SIZE_T i = 0; i < page_cnt; ++i) {
                PFN_NUM new_pfn = MmAllocPage(static_cast<ULONG>(MC_TYPE::SYSTEM));

                if (new_pfn == 0) {
                    InternalUnwindAllocation(vstart, curr_vaddr, i);
                    return STATUS_INSUFFICIENT_RESOURCES;
                }

                QWORD target_vaddr = (vstart == 0) ? (new_pfn << PAGE_SHIFT) : curr_vaddr;

                CBKSTATUS status = MapPage4K(target_vaddr, new_pfn << PAGE_SHIFT, cache_flags);
                if (status != STATUS_SUCCESS) {
                    MmDereferencePage(new_pfn);
                    InternalUnwindAllocation(vstart, curr_vaddr, i);
                    return status;
                }

                TlbFlushPage(target_vaddr);

                if (vstart != 0)
                    curr_vaddr += PAGE_SIZE;
                else if (i == 0)
                    out_phys_base = new_pfn << PAGE_SHIFT;
            }

            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InternalTeardownRange                                              *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Teardown a range                                                   *
         ********************************************************************************/
        VOID InternalTeardownRange(QWORD vstart, SIZE_T size) noexcept
        {
            CBKSTATUS status = MmuIterateRange(vstart, size, [](QWORD vaddr, SIZE_T) noexcept {
                QWORD phys_page = TranslateVirtualToPhysical(vaddr);
                if (phys_page != PHYS_ADDR_MAX)
                    InternalPurgePageMapping(vaddr, phys_page >> PAGE_SHIFT);
                return STATUS_SUCCESS;
            });

            ASSERT(status == STATUS_SUCCESS, "InternalTeardownRange: FAILED TO ITERATE RANGE");
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : NcacheAllocateBuffer                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocates a new non-cached continuous buffer                       *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS NcacheAllocateBuffer(SIZE_T size, QWORD tag,
                                              PNCACHE_DESCRIPTOR out_desc) noexcept
    {
        if (out_desc == nullptr || size == 0 || size > (limits::SIZE_T_max - PAGE_SIZE))
            return STATUS_INVALID_PARAMETER;

        SIZE_T page_cnt = (size + (PAGE_SIZE - 1)) / PAGE_SIZE;

        // NOTE: Early testing parameters without a VMM region...
        QWORD vstart              = 0;
        QWORD cache_flags         = PAGE_CACHE_DISABLE | PAGE_WRITE_THROUGH;
        QWORD allocated_phys_base = 0;

        CBKSTATUS status =
            InternalPopulateRange(vstart, page_cnt, cache_flags, allocated_phys_base);
        if (status != STATUS_SUCCESS)
            return status;

        MemoryFence();

        out_desc->virt_address = (vstart == 0) ? reinterpret_cast<PVOID>(allocated_phys_base)
                                               : reinterpret_cast<PVOID>(vstart);
        out_desc->size         = page_cnt * PAGE_SIZE;
        out_desc->phys_base =
            (vstart == 0) ? allocated_phys_base : TranslateVirtualToPhysical(vstart);
        out_desc->alloc_tag = tag;

        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : NcacheFreeBuffer                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Restores cache state and releases frames back to PMM               *
     ********************************************************************************/
    VOID NcacheFreeBuffer(PNCACHE_DESCRIPTOR descriptor) noexcept
    {
        if (descriptor == nullptr || descriptor->virt_address == nullptr || descriptor->size == 0)
            return;

        InternalTeardownRange(reinterpret_cast<QWORD>(descriptor->virt_address), descriptor->size);

        MemoryFence();

        descriptor->virt_address = nullptr;
        descriptor->phys_base    = 0;
        descriptor->size         = 0;
    }
} // namespace cbk::mem
