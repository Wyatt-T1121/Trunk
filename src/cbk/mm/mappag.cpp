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
 *  MODULE  : Page mapper                                                        *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Handles all mapping requests of all sizes                          *
 ********************************************************************************/
#include <cbk/mm/mappag.h>

namespace cbk::mem
{
    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiGenericMapAction                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Standard mapping action for all sizes                              *
         ********************************************************************************/
        VOID
        MiGenericMapAction(PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept
        {
            if (pte->Bits.present) {
                c.extra = 1;
                return;
            }

            pte->val = c.payload;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiGenericUnmapAction                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Standard unmapping action for all size                             *
         ********************************************************************************/
        VOID
        MiGenericUnmapAction(PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept
        {
            pte->val = 0;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiGenericMapPage                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Single page allocation engine                                      *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS
        MiGenericMapPage(QWORD virt, QWORD phys, QWORD flags) noexcept
        {
            ASSERT((virt & (PAGE_STRIDE - 1)) == 0, "MiGenericMapPage: Virtual alignment error");
            ASSERT((phys & (PAGE_STRIDE - 1)) == 0, "MiGenericMapPage: Physical alignment error");

            QWORD extra_flags = (LEVEL != PAGING_LEVEL::PT) ? PAGE_HUGE : 0;
            PTE_CONTEXT ctx{(phys & PAGE_MASK) | flags | PAGE_PRESENT | extra_flags, 0};

            CBKSTATUS status = MmExecuteOnPte(virt, LEVEL, TRUE, MiGenericMapAction, ctx);
            return (status == STATUS_SUCCESS && ctx.extra == 1) ? STATUS_CONFLICTING_ADDRESSES
                                                                : status;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiGenericUnmapPage                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Single page deallocation engine                                    *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS
        MiGenericUnmapPage(QWORD virt) noexcept
        {
            ASSERT((virt & (PAGE_STRIDE - 1)) == 0, "MiGenericUnmapPage: Virtual alignment error");
            PTE_CONTEXT ctx{0, 0};
            return MmExecuteOnPte(virt, LEVEL, FALSE, MiGenericUnmapAction, ctx);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiGenericMapRange                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Multiple page allocation engine                                    *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS
        MiGenericMapRange(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept
        {
            return MmIterateRange<PAGE_STRIDE>(vstart,
                                               size,
                                               [=](QWORD vaddr, SIZE_T index) noexcept {
                return MiGenericMapPage<PAGE_STRIDE, LEVEL>(vaddr,
                                                            pstart + (index * PAGE_STRIDE),
                                                            flags);
            });
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiGenericUnmapRange                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Multiple page deallocation engine                                  *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS
        MiGenericUnmapRange(QWORD start, SIZE_T size) noexcept
        {
            return MmIterateRange<PAGE_STRIDE>(start, size, [](QWORD vaddr, SIZE_T) noexcept {
                return MiGenericUnmapPage<PAGE_STRIDE, LEVEL>(vaddr);
            });
        }

    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmMapPage4K                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the 4 levels, allocates missing sub-tables              *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapPage4K(QWORD virt, QWORD phys, QWORD flags) noexcept
    {
        return MiGenericMapPage<4 * KB, PAGING_LEVEL::PT>(virt, phys, flags);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmUnmapPage4K                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the final PTE, clears the entry, executes TLB flush     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapPage4K(QWORD virt) noexcept
    {
        return MiGenericUnmapPage<4 * KB, PAGING_LEVEL::PT>(virt);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapPage2M                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates early at Level 2 PD with PAGE_HUGE bit set               *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapPage2M(QWORD virt, QWORD phys, QWORD flags) noexcept
    {
        return MiGenericMapPage<2 * MB, PAGING_LEVEL::PD>(virt, phys, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapPage2M                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 2 PDE block, executes TLB cache flush                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapPage2M(QWORD virt) noexcept
    {
        return MiGenericUnmapPage<2 * MB, PAGING_LEVEL::PD>(virt);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapPage1G                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates early at Level 3 PDPT with PAGE_HUGE bit set             *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapPage1G(QWORD virt, QWORD phys, QWORD flags) noexcept
    {
        return MiGenericMapPage<1 * GB, PAGING_LEVEL::PDPT>(virt, phys, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapPage1G                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 3 PDPTE block, executes TLB cache flush                *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapPage1G(QWORD virt) noexcept
    {
        return MiGenericUnmapPage<1 * GB, PAGING_LEVEL::PDPT>(virt);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapRange4K                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 4KB pages in one call                    *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapRange4K(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept
    {
        return MiGenericMapRange<4 * KB, PAGING_LEVEL::PT>(vstart, pstart, size, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapRange4K                                                      *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 4KB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapRange4K(QWORD start, SIZE_T size) noexcept
    {
        return MiGenericUnmapRange<4 * KB, PAGING_LEVEL::PT>(start, size);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmMapRange2M                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 2MB pages in one call                    *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmMapRange2M(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept
    {
        return MiGenericMapRange<2 * MB, PAGING_LEVEL::PD>(vstart, pstart, size, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmUnmapRange2M                                                      *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 2MB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmUnmapRange2M(QWORD start, SIZE_T size) noexcept
    {
        return MiGenericUnmapRange<2 * MB, PAGING_LEVEL::PD>(start, size);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapRange1G                                                          *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 1G pages in one call                     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MapRange1G(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept
    {
        return MiGenericMapRange<1 * GB, PAGING_LEVEL::PDPT>(vstart, pstart, size, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapRange1G                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 1G pages in one call                   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    UnmapRange1G(QWORD start, SIZE_T size) noexcept
    {
        return MiGenericUnmapRange<1 * GB, PAGING_LEVEL::PDPT>(start, size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIsRangeFree                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a range of virtual addresses has any existing mappings   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmIsRangeFree(QWORD start, SIZE_T size) noexcept
    {
        return MmIterateRange(start, size, [](QWORD vaddr, SIZE_T) noexcept {
            if (MmTranslateVirtualToPhysical(vaddr) != PHYS_ADDR_MAX)
                return STATUS_CONFLICTING_ADDRESSES;
            return STATUS_SUCCESS;
        });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmProtectPage4K                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modify an existing PTE's attribute bits                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmProtectPage4K(QWORD virt, QWORD flags) noexcept
    {
        PTE_CONTEXT ctx{flags, 0};
        return MmExecuteOnPte(virt,
                              PAGING_LEVEL::PT,
                              FALSE,
                              [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept {
            if (!pte->Bits.present)
                return;
            QWORD original_phys = static_cast<QWORD>(pte->Bits.page_frame) << PAGE_SHIFT;
            pte->val            = (original_phys & PAGE_MASK) | c.payload | PAGE_PRESENT;
        },
                              ctx);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmIsPagePresent                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a page is present                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmIsPagePresent(QWORD virt) noexcept
    {
        CBKSTATUS status = MmIsRangeFree(virt, PAGE_SIZE);
        if (status == STATUS_CONFLICTING_ADDRESSES)
            return STATUS_SUCCESS;
        return (status == STATUS_SUCCESS) ? STATUS_NOT_FOUND : status;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmTranslateVirtualToPhysical                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds what physical frame address is mapped to a virtual pointer   *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmTranslateVirtualToPhysical(QWORD virt) noexcept
    {
        QWORD page_offset  = virt & (PAGE_SIZE - 1);
        QWORD aligned_virt = virt & PAGE_MASK;

        PTE_CONTEXT ctx{aligned_virt, PHYS_ADDR_MAX};

        CBKSTATUS status = MmExecuteOnPte(aligned_virt,
                                          PAGING_LEVEL::PT,
                                          FALSE,
                                          [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept {
            if (!pte->Bits.present)
                return;

            QWORD mask = pte->Bits.large_page ? HUGE_MASK : (PAGE_SIZE - 1);
            c.extra = (static_cast<QWORD>(pte->Bits.page_frame) << PAGE_SHIFT) + (c.payload & mask);
        },
                                          ctx);

        if (status != STATUS_SUCCESS && status != STATUS_LARGE_PAGE)
            return PHYS_ADDR_MAX;

        return ctx.extra + page_offset;
    }
} // namespace cbk::mem