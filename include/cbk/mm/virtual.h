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
#pragma once

#include <types.h>

#include <attributes.h>
#include <status.h>

#include <cbk/mm/freelist.h>
#include <cbk/mm/mappag.h>
#include <cbk/mm/mmdefs.h>
#include <cbk/mm/mmunit.h>
#include <cbk/mm/vad.h>

#define MM_MAPPED_COPY_PAGES 14
#define MM_POOL_COPY_BYTES 512
#define MM_MAX_TRANSFER_SIZE (64 * 1024)

namespace cbk::mem
{
    extern MMVAD static_boot_nodes[];
    extern ULONG next_free_boot_node;

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
                           PULONG old_access_protection) noexcept;

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
                        BOOL capture_dirty_bit) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmCalculatePageCommitment                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Counts active page mappings within a virtual address range         *
     ********************************************************************************/
    NO_DISCARD SIZE_T
    MmCalculatePageCommitment(PMM_ADDRESS_SPACE address_space,
                              QWORD starting_address,
                              QWORD ending_address) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmDeleteVirtualAddresses                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Iterates through a virtual range, frees frames, unmaps hardware    *
     ********************************************************************************/
    VOID
    MmDeleteVirtualAddresses(PMM_ADDRESS_SPACE address_space,
                             QWORD starting_va,
                             QWORD ending_va) noexcept;

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
                        BOOL write_to_target) noexcept;

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
                        SIZE_T buffer_size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmGetPageProtection                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Decodes PTE flag bits into standard protection flags               *
     ********************************************************************************/
    NO_DISCARD ULONG
    MmGetPageProtection(QWORD pte_value) noexcept;

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
                            ULONG protect) noexcept;

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
                        ULONG free_type) noexcept;

} // namespace cbk::mem