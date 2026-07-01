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
 *  MODULE  : Virtual memory manager                                             *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Manages virtual memory and related allocations                     *
 ********************************************************************************/
#include <cbk/mm/virtual.h>

namespace cbk::mem
{
    MMVAD static_boot_nodes[ARR_BOOT_NODE_SIZE];
    ULONG next_free_boot_node = 0;

    static const MM_LEVEL_TRAITS level_traits[] = {{PAGE_SIZE, MmUnmapPage4K},
                                                   {2 * MB, MmUnmapPage2M},
                                                   {1 * GB, MmUnmapPage1G}};

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiVirtualPageNumberToAddress                                       *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Converts a VPN back to a virtual address                           *
         ********************************************************************************/
        NO_DISCARD INLINE QWORD
        MiVirtualPageNumberToAddress(QWORD vpn) noexcept
        {
            constexpr QWORD MAX_USER_VPN = PHYS_ADDR_48_BIT_MAX >> PAGE_SHIFT;
            QWORD raw_address            = vpn << PAGE_SHIFT;

            // If the VPN is greater than the max user VPN...
            // This is not a user address, so we need to sign extend the address...
            if (vpn > MAX_USER_VPN)
                raw_address |= ~(PHYS_ADDR_48_BIT_MAX);

            // This could be either a user address,
            // or a kernel address...
            return raw_address;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiGetLevelTraits                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Gets the traits for a given paging level                           *
         ********************************************************************************/
        NO_DISCARD INLINE const MM_LEVEL_TRAITS &
        MiGetLevelTraits(PAGING_LEVEL level, PAGE_TABLE_ENTRY entry) noexcept
        {
            if (entry.Bits.large_page &&
                static_cast<SIZE_T>(level) <= static_cast<SIZE_T>(PAGING_LEVEL::PDPT))
                return level_traits[static_cast<SIZE_T>(level)];
            return level_traits[0];
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiEncodeHardwreProtectionBits                                      *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Encode hardware protection bits                                    *
         ********************************************************************************/
        NO_DISCARD INLINE PAGE_TABLE_ENTRY
        MiEncodeHardwreProtectionBits(ULONG protect) noexcept
        {
            PAGE_TABLE_ENTRY pte{};

            if (protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE))
                pte.val |= PAGE_WRITABLE;
            if (!(protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)))
                pte.val |= PAGE_NX;
            if (protect & PAGE_USER)
                pte.val |= PAGE_USER;

            return pte;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiValidateAllocateParameters                                       *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Validate parameters for vmm allocate                               *
         ********************************************************************************/
        NO_DISCARD INLINE CBKSTATUS
        MiValidateAllocateParameters(PMM_ADDRESS_SPACE address_space,
                                     PVOID *base_address,
                                     PSIZE_T region_size,
                                     ULONG allocation_type) noexcept
        {
            if (!address_space || !base_address || !region_size)
                return STATUS_INVALID_PARAMETER;

            if (*region_size == 0)
                return STATUS_INVALID_PARAMETER;

            if (!(allocation_type & (MEM_RESERVE | MEM_COMMIT)))
                return STATUS_INVALID_PARAMETER;

            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiProcessVirtualRange                                              *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Process a virtual range (abstraction helper basically...)          *
         ********************************************************************************/
        template <typename TAction>
        NO_DISCARD INLINE SIZE_T
        MiProcessVirtualRange(QWORD pml4_phys,
                              QWORD start_va,
                              SIZE_T total_size,
                              PAGING_LEVEL target_level,
                              TAction action) noexcept
        {
            QWORD current_va            = start_va;
            SIZE_T bytes_remaining      = total_size;
            SIZE_T tracking_accumulator = 0;

            while (bytes_remaining > 0) {
                PPAGE_TABLE_ENTRY entry   = nullptr;
                PAGING_LEVEL resolved_lvl = PAGING_LEVEL::PML4;
                CBKSTATUS status =
                    MmWalkToTable(pml4_phys, current_va, target_level, FALSE, entry, resolved_lvl);
                if (status != STATUS_SUCCESS || entry == nullptr) {
                    SIZE_T step_size = PAGE_SIZE;
                    if (target_level == PAGING_LEVEL::PD)
                        step_size = 2 * 1024 * 1024;
                    else if (target_level == PAGING_LEVEL::PDPT)
                        step_size = 1024 * 1024 * 1024;

                    current_va      += step_size;
                    bytes_remaining -= (bytes_remaining < step_size) ? bytes_remaining : step_size;
                    continue;
                }

                const auto &traits = MiGetLevelTraits(resolved_lvl, *entry);

                QWORD page_offset  = current_va & (traits.bytes_spanned - 1);
                SIZE_T chunk_avail = traits.bytes_spanned - page_offset;
                SIZE_T chunk_size = (bytes_remaining < chunk_avail) ? bytes_remaining : chunk_avail;

                QWORD base_phys = (status == STATUS_SUCCESS && entry->Bits.present)
                                      ? (static_cast<QWORD>(entry->Bits.page_frame) << PAGE_SHIFT)
                                      : 0;

                PTE_INFO info = {entry, current_va, base_phys, traits.bytes_spanned, chunk_size};

                if (!action(info, traits, status, tracking_accumulator))
                    break;

                current_va      += chunk_size;
                bytes_remaining -= chunk_size;
            }
            return tracking_accumulator;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiCommitVirtualRange                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Commit a virtual range                                             *
         ********************************************************************************/
        NO_DISCARD INLINE CBKSTATUS
        MiCommitVirtualRange(PMM_ADDRESS_SPACE address_space,
                             QWORD target_va,
                             SIZE_T aligned_size,
                             ULONG protect,
                             PMMVAD vad) noexcept
        {
            SIZE_T pages_mapped           = 0;
            PAGE_TABLE_ENTRY template_pte = MiEncodeHardwreProtectionBits(protect);

            PAGING_LEVEL target_level = PAGING_LEVEL::PT;
            if (protect & MEM_LARGE_PAGES)
                target_level = PAGING_LEVEL::PD;

            SIZE_T processed_bytes = MiProcessVirtualRange(address_space->hardware_map->pml4_phys,
                                                           target_va,
                                                           aligned_size,
                                                           target_level,
                                                           [&](const PTE_INFO &info,
                                                               const MM_LEVEL_TRAITS &traits,
                                                               CBKSTATUS walk_status,
                                                               SIZE_T &accum) noexcept -> BOOL {
                if (walk_status != STATUS_SUCCESS || info.entry == nullptr)
                    return FALSE;

                if (!info.entry->Bits.present) {
                    PFN_NUM pfn         = 0;
                    SIZE_T pages_needed = traits.bytes_spanned / PAGE_SIZE;

                    if (traits.bytes_spanned > PAGE_SIZE)
                        pfn = MmAllocContiguousPages(pages_needed, traits.bytes_spanned);
                    else
                        pfn = MmAllocPage(static_cast<ULONG>(MM_PFN_STATE::ZEROED_PAGE_LIST));

                    if (pfn == 0)
                        return FALSE;

                    template_pte.Bits.page_frame = pfn;
                    template_pte.Bits.large_page = (traits.bytes_spanned > PAGE_SIZE) ? 1 : 0;

                    *info.entry = template_pte;
                    hal::InvLpg(info.va);
                    pages_mapped += pages_needed;
                }
                accum += traits.bytes_spanned;
                return TRUE;
            });

            if (processed_bytes < aligned_size) {
                if (pages_mapped > 0)
                    MmDeleteVirtualAddresses(address_space,
                                             target_va,
                                             target_va + (pages_mapped * PAGE_SIZE));
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            vad->u.long_flags |= VAD_STATE_COMMITTED;
            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiReserveVirtualRange                                              *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Reserve a virtual range                                            *
         ********************************************************************************/
        NO_DISCARD INLINE CBKSTATUS
        MiReserveVirtualRange(PMM_ADDRESS_SPACE address_space,
                              QWORD &target_va,
                              SIZE_T page_count,
                              ULONG allocation_type,
                              ULONG protect,
                              PMMVAD &out_vad) noexcept
        {
            if (target_va == 0) {

                QWORD free_vpn =
                    MmVadFindFreeGap(address_space, page_count, (allocation_type & MEM_TOP_DOWN));

                if (free_vpn == 0)
                    return STATUS_INSUFFICIENT_RESOURCES;

                target_va = free_vpn << PAGE_SHIFT;

            } else {
                if (!IsPageAligned(target_va))
                    return STATUS_INVALID_PARAMETER;

                QWORD start_vpn = target_va >> PAGE_SHIFT;

                for (SIZE_T i = 0; i < page_count; ++i)
                    if (MmVadFindNode(address_space, start_vpn + i) != nullptr)
                        return STATUS_CONFLICTING_ADDRESSES;
            }

            if (next_free_boot_node >= ARR_BOOT_NODE_SIZE)
                return STATUS_INSUFFICIENT_RESOURCES;

            PMMVAD blank_slot = &static_boot_nodes[next_free_boot_node++];
            out_vad = MmVadInitializeNode(blank_slot, target_va >> PAGE_SHIFT, page_count, protect);
            if (out_vad == nullptr) {
                next_free_boot_node--;
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            CBKSTATUS status = MmVadInsertNode(address_space, out_vad);
            if (status != STATUS_SUCCESS) {
                next_free_boot_node--;
                return status;
            }

            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiVerifyExistingReservation                                        *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Verify an existing reservation                                     *
         ********************************************************************************/
        NO_DISCARD INLINE CBKSTATUS
        MiVerifyExistingReservation(PMM_ADDRESS_SPACE address_space,
                                    QWORD target_va,
                                    SIZE_T page_count,
                                    PMMVAD &out_vad) noexcept
        {
            if (target_va == 0 || !IsPageAligned(target_va))
                return STATUS_INVALID_PARAMETER;

            out_vad = MmVadFindNode(address_space, target_va >> PAGE_SHIFT);
            if (out_vad == nullptr)
                return STATUS_NOT_FOUND;

            if ((target_va >> PAGE_SHIFT) + page_count - 1 > out_vad->ending_vpn)
                return STATUS_INVALID_PARAMETER;

            return STATUS_SUCCESS;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiProbeVirtualRangeForRead                                         *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Verify a virtual range is fully within user space                  *
         ********************************************************************************/
        NO_DISCARD INLINE BOOL
        MiProbeVirtualRangeForRead(PVOID address, SIZE_T length) noexcept
        {
            if (length == 0)
                return TRUE;

            QWORD start = reinterpret_cast<QWORD>(address);
            QWORD end   = start + length - 1;

            if (end < start)
                return FALSE;

            return MM_IS_USER_ADDRESS(end);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiProbeVirtualRangeForWrite                                        *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Verify a virtual range is safe for user-mode writing               *
         ********************************************************************************/
        NO_DISCARD INLINE BOOL
        MiProbeVirtualRangeForWrite(PVOID address, SIZE_T length) noexcept
        {
            return MiProbeVirtualRangeForRead(address, length);
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiSplitVadNode                                                     *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Splis a VAD node into three fragments                              *
         ********************************************************************************/
        NO_DISCARD CBKSTATUS
        MiSplitVadNode(PMM_ADDRESS_SPACE address_space,
                       PMMVAD original_vad,
                       QWORD target_start_vpn,
                       QWORD target_end_vpn) noexcept
        {
            if (next_free_boot_node >= ARR_BOOT_NODE_SIZE)
                return STATUS_INSUFFICIENT_RESOURCES;

            QWORD orig_start = original_vad->starting_vpn;
            QWORD orig_end   = original_vad->ending_vpn;
            CBKSTATUS status = STATUS_SUCCESS;

            MmVadDeleteNode(address_space, original_vad);

            if (target_start_vpn == orig_start && target_end_vpn < orig_end) {
                original_vad->starting_vpn = orig_start;
                original_vad->ending_vpn   = target_end_vpn;

                status = MmVadInsertNode(address_space, original_vad);
                if (status != STATUS_SUCCESS)
                    return status;

                PMMVAD right_node          = &static_boot_nodes[next_free_boot_node++];
                right_node->starting_vpn   = target_end_vpn + 1;
                right_node->ending_vpn     = orig_end;
                right_node->u.long_flags   = original_vad->u.long_flags;
                right_node->backing_object = original_vad->backing_object;

                status = MmVadInsertNode(address_space, right_node);
                if (status != STATUS_SUCCESS)
                    return status;

            } else if (target_start_vpn > orig_start && target_end_vpn == orig_end) {
                original_vad->starting_vpn = target_start_vpn;
                original_vad->ending_vpn   = orig_end;

                status = MmVadInsertNode(address_space, original_vad);
                if (status != STATUS_SUCCESS)
                    return status;

                PMMVAD left_node          = &static_boot_nodes[next_free_boot_node++];
                left_node->starting_vpn   = orig_start;
                left_node->ending_vpn     = target_start_vpn - 1;
                left_node->u.long_flags   = original_vad->u.long_flags;
                left_node->backing_object = original_vad->backing_object;

                status = MmVadInsertNode(address_space, left_node);
                if (status != STATUS_SUCCESS)
                    return status;

            } else {
                original_vad->starting_vpn = orig_start;
                original_vad->ending_vpn   = target_start_vpn - 1;

                status = MmVadInsertNode(address_space, original_vad);
                if (status != STATUS_SUCCESS)
                    return status;

                if (next_free_boot_node + 1 >= ARR_BOOT_NODE_SIZE)
                    return STATUS_INSUFFICIENT_RESOURCES;

                PMMVAD middle_node          = &static_boot_nodes[next_free_boot_node++];
                middle_node->starting_vpn   = target_start_vpn;
                middle_node->ending_vpn     = target_end_vpn;
                middle_node->u.long_flags   = original_vad->u.long_flags;
                middle_node->backing_object = original_vad->backing_object;

                status = MmVadInsertNode(address_space, middle_node);
                if (status != STATUS_SUCCESS)
                    return status;

                PMMVAD right_node          = &static_boot_nodes[next_free_boot_node++];
                right_node->starting_vpn   = target_end_vpn + 1;
                right_node->ending_vpn     = orig_end;
                right_node->u.long_flags   = original_vad->u.long_flags;
                right_node->backing_object = original_vad->backing_object;

                status = MmVadInsertNode(address_space, right_node);
                if (status != STATUS_SUCCESS)
                    return status;
            }

            return STATUS_SUCCESS;
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmProtectVirtualMemory                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Modifies virtual memory protections and syncs translation tables   *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmProtectVirtualMemory(PMM_ADDRESS_SPACE space,
                           PVOID *base_address,
                           PSIZE_T number_of_bytes_to_protect,
                           ULONG new_access_protection,
                           PULONG old_access_protection) noexcept
    {
        if (!space || !base_address || !number_of_bytes_to_protect || !old_access_protection)
            return STATUS_INVALID_PARAMETER;

        QWORD bytes_requested = *number_of_bytes_to_protect;
        if (bytes_requested == 0)
            return STATUS_INVALID_PARAMETER;

        QWORD start_addr = reinterpret_cast<QWORD>(*base_address) & PAGE_MASK;

        if (bytes_requested > (PHYS_ADDR_48_BIT_MAX - start_addr))
            return STATUS_INVALID_PARAMETER;

        if (bytes_requested > (MM_USER_SPACE_END - start_addr))
            return STATUS_INVALID_PARAMETER;

        QWORD end_addr = PAGE_ALIGN(start_addr + bytes_requested);
        if (end_addr <= start_addr)
            return STATUS_UNSUCCESSFUL;

        SIZE_T size = end_addr - start_addr;

        PMMVAD vad = MmVadFindNode(space, start_addr >> PAGE_SHIFT);
        if (vad == nullptr)
            return STATUS_UNSUCCESSFUL;

        QWORD start_vpn = start_addr >> PAGE_SHIFT;
        QWORD end_vpn   = end_addr >> PAGE_SHIFT;

        if (end_vpn > (vad->ending_vpn + 1))
            return STATUS_CONFLICTING_ADDRESSES;

        if (start_vpn > vad->starting_vpn || (end_vpn - 1) < vad->ending_vpn) {
            CBKSTATUS split_status = MiSplitVadNode(space, vad, start_vpn, end_vpn - 1);
            if (split_status != STATUS_SUCCESS)
                return split_status;

            vad = MmVadFindNode(space, start_vpn);
            if (vad == nullptr)
                return STATUS_UNSUCCESSFUL;
        }

        CBKSTATUS status = MmProtectRange(start_addr, size, new_access_protection);
        if (status != STATUS_SUCCESS)
            return status;

        MmFlushTbAndCapture(space, vad, nullptr, new_access_protection, nullptr, FALSE);

        *old_access_protection      = vad->u.flags.protection;
        *base_address               = reinterpret_cast<PVOID>(start_addr);
        *number_of_bytes_to_protect = size;

        vad->u.flags.protection = new_access_protection;
        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmFlushTbAndCapture                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flushes TLB caches and captures hardware page state bits           *
     ********************************************************************************/
    VOID
    MmFlushTbAndCapture(PMM_ADDRESS_SPACE address_space,
                        PMMVAD found_vad,
                        PMMPTE pointer_pte,
                        ULONG protection_mask,
                        PMMPFN pfn1,
                        BOOL capture_dirty_bit) noexcept
    {
        if (address_space == nullptr || found_vad == nullptr ||
            found_vad->ending_vpn < found_vad->starting_vpn)
            return;

        if (capture_dirty_bit && pfn1 && pointer_pte) {
            PPAGE_TABLE_ENTRY pte = reinterpret_cast<PPAGE_TABLE_ENTRY>(pointer_pte);
            if (pte->Bits.dirty) {
                pte->Bits.dirty      = 0;
                pte->Bits.available |= PTE_DIRTY_FLAG_BIT;
            }
        }

        QWORD current_address = MiVirtualPageNumberToAddress(found_vad->starting_vpn);
        QWORD end_address     = MiVirtualPageNumberToAddress(found_vad->ending_vpn) + PAGE_SIZE;

        while (current_address < end_address) {
            hal::InvLpg(current_address);

            PPAGE_TABLE_ENTRY entry   = nullptr;
            PAGING_LEVEL resolved_lvl = PAGING_LEVEL::PML4;

            CBKSTATUS status = MmWalkToTable(address_space->hardware_map->pml4_phys,
                                             current_address,
                                             PAGING_LEVEL::PT,
                                             FALSE,
                                             entry,
                                             resolved_lvl);

            SIZE_T step = PAGE_SIZE;
            if (status == STATUS_SUCCESS && entry && entry->Bits.large_page)
                step = level_traits[static_cast<SIZE_T>(resolved_lvl)].bytes_spanned;

            current_address += step;
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmCalculatePageCommitment                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Counts active page mappings within a virtual address range         *
     ********************************************************************************/
    NO_DISCARD SIZE_T
    MmCalculatePageCommitment(PMM_ADDRESS_SPACE address_space,
                              QWORD starting_address,
                              QWORD ending_address) noexcept
    {
        if (address_space == nullptr || ending_address <= starting_address)
            return 0;

        QWORD aligned_start = starting_address & PAGE_MASK;
        QWORD aligned_end   = PAGE_ALIGN(ending_address);
        SIZE_T size         = aligned_end - aligned_start;

        return MiProcessVirtualRange(address_space->hardware_map->pml4_phys,
                                     aligned_start,
                                     size,
                                     PAGING_LEVEL::PT,

                                     [](const PTE_INFO &info,
                                        const MM_LEVEL_TRAITS &,
                                        CBKSTATUS status,
                                        SIZE_T &accum) noexcept -> BOOL {
            if (status == STATUS_SUCCESS && info.entry->Bits.present)
                accum += info.step_size / PAGE_SIZE;
            else if (status == STATUS_SUCCESS && (info.entry->Bits.available & PTE_DIRTY_FLAG_BIT))
                accum += info.step_size / PAGE_SIZE;
            return TRUE;
        });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmDeleteVirtualAddresses                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Iterates through a virtual range, frees frames, unmaps hardware    *
     ********************************************************************************/
    VOID
    MmDeleteVirtualAddresses(PMM_ADDRESS_SPACE address_space,
                             QWORD starting_va,
                             QWORD ending_va) noexcept
    {
        if (address_space == nullptr || ending_va <= starting_va)
            return;

        QWORD aligned_start = starting_va & PAGE_MASK;
        QWORD aligned_end   = PAGE_ALIGN(ending_va);
        SIZE_T size         = aligned_end - aligned_start;

        MAYBE_UNUSED SIZE_T unused = MiProcessVirtualRange(address_space->hardware_map->pml4_phys,
                                                           aligned_start,
                                                           size,
                                                           PAGING_LEVEL::PT,
                                                           [](const PTE_INFO &info,
                                                              const MM_LEVEL_TRAITS &traits,
                                                              CBKSTATUS status,
                                                              SIZE_T &) noexcept -> BOOL {
            if (status != STATUS_SUCCESS || !info.entry->Bits.present)
                return TRUE;
            PFN_NUM base_pfn = static_cast<PFN_NUM>(info.base_phys >> PAGE_SHIFT);
            if (base_pfn <= mm_highest_physical_page && base_pfn != 0) {
                SIZE_T pages = info.step_size / PAGE_SIZE;
                for (SIZE_T i = 0; i < pages; ++i)
                    MmDereferencePage(base_pfn + i);
            }
            traits.unmap(info.va);
            return TRUE;
        });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmCopyProcessMemory                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Copies data between a target process and the kernel                *
     ********************************************************************************/
    NO_DISCARD SIZE_T
    MmCopyProcessMemory(PMM_ADDRESS_SPACE target_address_space,
                        PVOID target_virtual_address,
                        PVOID kernel_buffer,
                        SIZE_T buffer_size,
                        BOOL write_to_target) noexcept
    {
        if (target_address_space == nullptr || target_address_space->hardware_map == nullptr ||
            kernel_buffer == nullptr || buffer_size == 0)
            return 0;

        BOOL probe_success = write_to_target
                                 ? MiProbeVirtualRangeForWrite(target_virtual_address, buffer_size)
                                 : MiProbeVirtualRangeForRead(target_virtual_address, buffer_size);

        if (!probe_success)
            return 0;

        QWORD start_va = reinterpret_cast<QWORD>(target_virtual_address);
        BYTE *curr_buf = reinterpret_cast<BYTE *>(kernel_buffer);

        return MiProcessVirtualRange(target_address_space->hardware_map->pml4_phys,
                                     start_va,
                                     buffer_size,
                                     PAGING_LEVEL::PT,
                                     [&](const PTE_INFO &info,
                                         const MM_LEVEL_TRAITS &,
                                         CBKSTATUS status,
                                         SIZE_T &accum) noexcept -> BOOL {
            if (status != STATUS_SUCCESS || !info.entry->Bits.present)
                return FALSE;
            if (write_to_target) {
                ASSERT(info.entry->Bits.writable,
                       "MmCopyProcessMemory: Write violation to target read-only page!");
                if (!info.entry->Bits.writable)
                    return FALSE;
            }
            QWORD mask       = ~(info.step_size - 1);
            QWORD exact_phys = (info.base_phys & mask) + (info.va & ~mask);
            BYTE *phys_ptr   = reinterpret_cast<BYTE *>(PaddrToKvaddr(exact_phys));
            if (write_to_target) {
                tklib::memcpy(phys_ptr, curr_buf, info.chunk_size);
                hal::InvLpg(info.va);
            } else
                tklib::memcpy(curr_buf, phys_ptr, info.chunk_size);
            curr_buf += info.chunk_size;
            accum    += info.chunk_size;
            return TRUE;
        });
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmCopyVirtualMemory                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Inter-process copying (gateway for MmCopyProcessMemory)            *
     ********************************************************************************/
    NO_DISCARD SIZE_T
    MmCopyVirtualMemory(PMM_ADDRESS_SPACE source_address_space,
                        PVOID source_address,
                        PMM_ADDRESS_SPACE target_address_space,
                        PVOID target_address,
                        SIZE_T buffer_size) noexcept
    {
        if (source_address_space == nullptr || target_address_space == nullptr ||
            source_address == nullptr || target_address == nullptr || buffer_size == 0)
            return 0;

        if (!MiProbeVirtualRangeForRead(source_address, buffer_size) ||
            !MiProbeVirtualRangeForWrite(target_address, buffer_size))
            return 0;

        if (source_address_space == target_address_space) {
            tklib::memcpy(target_address, source_address, buffer_size);
            return buffer_size;
        }

        constexpr SIZE_T CHUNK_SIZE = 4 * PAGE_SIZE;
        BYTE stack_buffer[CHUNK_SIZE];

        SIZE_T total_bytes_copied = 0;
        SIZE_T bytes_remaining    = buffer_size;

        BYTE *curr_source = reinterpret_cast<BYTE *>(source_address);
        BYTE *curr_target = reinterpret_cast<BYTE *>(target_address);

        while (bytes_remaining > 0) {
            SIZE_T current_chunk = (bytes_remaining < CHUNK_SIZE) ? bytes_remaining : CHUNK_SIZE;

            SIZE_T bytes_read = MmCopyProcessMemory(source_address_space,
                                                    curr_source,
                                                    stack_buffer,
                                                    current_chunk,
                                                    FALSE);
            if (bytes_read == 0)
                break;

            SIZE_T bytes_written = MmCopyProcessMemory(target_address_space,
                                                       curr_target,
                                                       stack_buffer,
                                                       bytes_read,
                                                       TRUE);
            if (bytes_written == 0)
                break;

            curr_source        += bytes_written;
            curr_target        += bytes_written;
            bytes_remaining    -= bytes_written;
            total_bytes_copied += bytes_written;

            if (bytes_written < bytes_read)
                break;
        }

        return total_bytes_copied;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetPageProtection                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Decodes PTE flag bits into standard protection flags               *
     ********************************************************************************/
    NO_DISCARD ULONG
    MmGetPageProtection(QWORD pte_value) noexcept
    {
        if (!(pte_value & PAGE_PRESENT))
            return PAGE_NOACCESS;

        static constexpr ULONG protection_lut[4] = {PAGE_EXECUTE_READ,
                                                    PAGE_EXECUTE_READWRITE,
                                                    PAGE_READONLY,
                                                    PAGE_READWRITE};

        ULONG index = ((pte_value & PAGE_WRITABLE) ? 1 : 0) | ((pte_value & PAGE_NX) ? 2 : 0);

        return protection_lut[index];
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmAllocateVirtualMemory                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Allocates or reserves a region of virtual pages for a process      *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmAllocateVirtualMemory(PMM_ADDRESS_SPACE address_space,
                            PVOID *base_address,
                            PSIZE_T region_size,
                            ULONG allocation_type,
                            ULONG protect) noexcept
    {
        CBKSTATUS status =
            MiValidateAllocateParameters(address_space, base_address, region_size, allocation_type);
        if (status != STATUS_SUCCESS)
            return status;

        SIZE_T aligned_size = PageAlignUp(*region_size);
        SIZE_T page_count   = aligned_size / PAGE_SIZE;
        QWORD target_va     = reinterpret_cast<QWORD>(*base_address);
        PMMVAD vad          = nullptr;

        status = (allocation_type & MEM_RESERVE)
                     ? MiReserveVirtualRange(address_space,
                                             target_va,
                                             page_count,
                                             allocation_type,
                                             protect,
                                             vad)
                     : MiVerifyExistingReservation(address_space, target_va, page_count, vad);

        if (status != STATUS_SUCCESS)
            return status;

        if (allocation_type & MEM_COMMIT) {
            status = MiCommitVirtualRange(address_space, target_va, aligned_size, protect, vad);
            if (status != STATUS_SUCCESS)
                return status;
        }

        *base_address = reinterpret_cast<PVOID>(target_va);
        *region_size  = aligned_size;
        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmFreeVirtualMemory                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Frees a region of virtual pages previously allocated               *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmFreeVirtualMemory(PMM_ADDRESS_SPACE address_space,
                        PVOID *base_address,
                        PSIZE_T region_size,
                        ULONG free_type) noexcept
    {
        if (address_space == nullptr || base_address == nullptr || *base_address == nullptr ||
            region_size == nullptr)
            return STATUS_INVALID_PARAMETER;

        if ((free_type & (MEM_RELEASE | MEM_DECOMMIT)) == (MEM_RELEASE | MEM_DECOMMIT))
            return STATUS_INVALID_PARAMETER;

        if ((free_type & (MEM_RELEASE | MEM_DECOMMIT)) == 0)
            return STATUS_INVALID_PARAMETER;

        QWORD target_va  = reinterpret_cast<QWORD>(*base_address);
        QWORD target_vpn = target_va / PAGE_SIZE;

        PMMVAD found_vad = MmVadFindNode(address_space, target_vpn);
        if (found_vad == nullptr)
            return STATUS_CONFLICTING_ADDRESSES;

        if (found_vad->starting_vpn != target_vpn)
            return STATUS_INVALID_PARAMETER;

        QWORD starting_va = found_vad->starting_vpn * PAGE_SIZE;
        QWORD ending_va   = (found_vad->ending_vpn * PAGE_SIZE) + (PAGE_SIZE - 1);
        SIZE_T total_size = (ending_va - starting_va) + 1;

        if (free_type & MEM_RELEASE)
            if (*region_size != 0 && *region_size != total_size)
                return STATUS_INVALID_PARAMETER;

        if (free_type & MEM_DECOMMIT)
            MmDeleteVirtualAddresses(address_space, starting_va, ending_va);
        else if (free_type & MEM_RELEASE) {
            MmDeleteVirtualAddresses(address_space, starting_va, ending_va);

            MmVadDeleteNode(address_space, found_vad);

            if (found_vad >= static_boot_nodes &&
                found_vad < &static_boot_nodes[MM_MAPPED_COPY_PAGES])
                tklib::memset(found_vad, 0, sizeof(MmVad));
        }

        *base_address = nullptr;
        *region_size  = total_size;

        return STATUS_SUCCESS;
    }

} // namespace cbk::mem