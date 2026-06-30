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
         *  FUNC    : StandardMapAction                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Standard mapping action for all sizes                              *
         ********************************************************************************/
        VOID StandardMapAction(PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept
        {
            if (pte->Bits.present) {
                c.extra = 1;
                return;
            }

            pte->val = c.payload;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : StandardUnmapAction                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Standard unmapping action for all size                             *
         ********************************************************************************/
        VOID StandardUnmapAction(PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept
        {
            pte->val = 0;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InternalMapPage                                                    *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Single page allocation engine                                      *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS InternalMapPage(QWORD virt, QWORD phys, QWORD flags) noexcept
        {
            ASSERT((virt & (PAGE_STRIDE - 1)) == 0, "InternalMapPage: Virtual alignment error");
            ASSERT((phys & (PAGE_STRIDE - 1)) == 0, "InternalMapPage: Physical alignment error");

            QWORD extra_flags = (LEVEL != PAGING_LEVEL::PT) ? PAGE_HUGE : 0;
            PTE_CONTEXT ctx{(phys & PAGE_MASK) | flags | PAGE_PRESENT | extra_flags, 0};

            CBKSTATUS status = MmuExecuteOnPte(virt, LEVEL, TRUE, StandardMapAction, ctx);
            return (status == STATUS_SUCCESS && ctx.extra == 1) ? STATUS_CONFLICTING_ADDRESSES
                                                                : status;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InternalUnmapPage                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Single page deallocation engine                                    *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS InternalUnmapPage(QWORD virt) noexcept
        {
            ASSERT((virt & (PAGE_STRIDE - 1)) == 0, "InternalUnmapPage: Virtual alignment error");
            PTE_CONTEXT ctx{0, 0};
            return MmuExecuteOnPte(virt, LEVEL, FALSE, StandardUnmapAction, ctx);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InternalMapRange                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Multiple page allocation engine                                    *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS InternalMapRange(QWORD vstart, QWORD pstart, SIZE_T size,
                                              QWORD flags) noexcept
        {
            return MmuIterateRange<PAGE_STRIDE>(
                vstart, size, [=](QWORD vaddr, SIZE_T index) noexcept {
                    return InternalMapPage<PAGE_STRIDE, LEVEL>(
                        vaddr, pstart + (index * PAGE_STRIDE), flags);
                });
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : InternalUnmapRange                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Multiple page deallocation engine                                  *
         ********************************************************************************/
        template <QWORD PAGE_STRIDE, PAGING_LEVEL LEVEL>
        NO_DISCARD CBKSTATUS InternalUnmapRange(QWORD start, SIZE_T size) noexcept
        {
            return MmuIterateRange<PAGE_STRIDE>(start, size, [](QWORD vaddr, SIZE_T) noexcept {
                return InternalUnmapPage<PAGE_STRIDE, LEVEL>(vaddr);
            });
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MapPage4K                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the 4 levels, allocates missing sub-tables              *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapPage4K(QWORD virt, QWORD phys, QWORD flags) noexcept
    {
        return InternalMapPage<4 * KB, PAGING_LEVEL::PT>(virt, phys, flags);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : UnmapPage4K                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks down the final PTE, clears the entry, executes TLB flush     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapPage4K(QWORD virt) noexcept
    {
        return InternalUnmapPage<4 * KB, PAGING_LEVEL::PT>(virt);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapPage2M                                                           *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates early at Level 2 PD with PAGE_HUGE bit set               *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapPage2M(QWORD virt, QWORD phys, QWORD flags) noexcept
    {
        return InternalMapPage<2 * MB, PAGING_LEVEL::PD>(virt, phys, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapPage2M                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 2 PDE block, executes TLB cache flush                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapPage2M(QWORD virt) noexcept
    {
        return InternalUnmapPage<2 * MB, PAGING_LEVEL::PD>(virt);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapPage1G                                                           *
     * DATE    : 2026                                                                *
     * PURPOSE : Terminates early at Level 3 PDPT with PAGE_HUGE bit set             *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapPage1G(QWORD virt, QWORD phys, QWORD flags) noexcept
    {
        return InternalMapPage<1 * GB, PAGING_LEVEL::PDPT>(virt, phys, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapPage1G                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Clears Level 3 PDPTE block, executes TLB cache flush                *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapPage1G(QWORD virt) noexcept
    {
        return InternalUnmapPage<1 * GB, PAGING_LEVEL::PDPT>(virt);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapRange4K                                                          *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 4KB pages in one call                    *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapRange4K(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept
    {
        return InternalMapRange<4 * KB, PAGING_LEVEL::PT>(vstart, pstart, size, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapRange4K                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 4KB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapRange4K(QWORD start, SIZE_T size) noexcept
    {
        return InternalUnmapRange<4 * KB, PAGING_LEVEL::PT>(start, size);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapRange2M                                                          *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 2MB pages in one call                    *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapRange2M(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept
    {
        return InternalMapRange<2 * MB, PAGING_LEVEL::PD>(vstart, pstart, size, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapRange2M                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 2MB pages in one call                  *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapRange2M(QWORD start, SIZE_T size) noexcept
    {
        return InternalUnmapRange<2 * MB, PAGING_LEVEL::PD>(start, size);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MapRange1G                                                          *
     * DATE    : 2026                                                                *
     * PURPOSE : Maps a continuous block of 1G pages in one call                     *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS MapRange1G(QWORD vstart, QWORD pstart, SIZE_T size, QWORD flags) noexcept
    {
        return InternalMapRange<1 * GB, PAGING_LEVEL::PDPT>(vstart, pstart, size, flags);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : UnmapRange1G                                                        *
     * DATE    : 2026                                                                *
     * PURPOSE : Unmaps a continuous block of 1G pages in one call                   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS UnmapRange1G(QWORD start, SIZE_T size) noexcept
    {
        return InternalUnmapRange<1 * GB, PAGING_LEVEL::PDPT>(start, size);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsRangeFree                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a range of virtual addresses has any existing mappings   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS IsRangeFree(QWORD start, SIZE_T size) noexcept
    {
        return MmuIterateRange(start, size, [](QWORD vaddr, SIZE_T) noexcept {
            if (TranslateVirtualToPhysical(vaddr) != PHYS_ADDR_MAX)
                return STATUS_CONFLICTING_ADDRESSES;
            return STATUS_SUCCESS;
        });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ProtectPage4K                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modify an existing PTE's attribute bits                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS ProtectPage4K(QWORD virt, QWORD flags) noexcept
    {
        PTE_CONTEXT ctx{flags, 0};
        return MmuExecuteOnPte(
            virt, PAGING_LEVEL::PT, FALSE,
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
     *  FUNC    : IsPagePresent                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks if a page is present                                        *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS IsPagePresent(QWORD virt) noexcept
    {
        CBKSTATUS status = IsRangeFree(virt, PAGE_SIZE);
        if (status == STATUS_CONFLICTING_ADDRESSES)
            return STATUS_SUCCESS;
        return (status == STATUS_SUCCESS) ? STATUS_NOT_FOUND : status;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TranslateVirtualToPhysical                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Finds what physical frame address is mapped to a virtual pointer   *
     ********************************************************************************/
    NO_DISCARD QWORD TranslateVirtualToPhysical(QWORD virt) noexcept
    {
        QWORD page_offset  = virt & (PAGE_SIZE - 1);
        QWORD aligned_virt = virt & PAGE_MASK;

        PTE_CONTEXT ctx{aligned_virt, PHYS_ADDR_MAX};

        CBKSTATUS status = MmuExecuteOnPte(
            aligned_virt, PAGING_LEVEL::PT, FALSE,
            [](PPAGE_TABLE_ENTRY pte, PTE_CONTEXT &c) noexcept {
                if (!pte->Bits.present)
                    return;

                QWORD mask = pte->Bits.large_page ? HUGE_MASK : (PAGE_SIZE - 1);
                c.extra =
                    (static_cast<QWORD>(pte->Bits.page_frame) << PAGE_SHIFT) + (c.payload & mask);
            },
            ctx);

        if (status != STATUS_SUCCESS && status != STATUS_LARGE_PAGE)
            return PHYS_ADDR_MAX;

        return ctx.extra + page_offset;
    }
} // namespace cbk::mem