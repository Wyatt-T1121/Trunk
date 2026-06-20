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
 *  PURPOSE : Virtual memory management                                          *
 ********************************************************************************/
#pragma once

// KERNEL DOES NOT SUPPPORT PROCESSES OR SCHEDULING RIGHT NOW
// THIS VMM IS VERY SIMPLE, NO PROCESS SUPPORT
// LATER WILL BE IMPROVED

// TODO: ADD PROCESS SUPPORT AND AREA MAPPINGS

#include <cbk/mem/arch/mmarch.h>
#include <cbk/mem/virt/aspace.h>

#include <macros.h>
#include <types.h>

namespace trunk::mem
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadFind                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Search the VAD list of space for the entry covering address        *
     ********************************************************************************/
    NO_DISCARD PMMVAD VadFind(ArchAspace *space, QWORD address) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadInsert                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Insert a new VAD into the address space VAD list                   *
     ********************************************************************************/
    VOID VadInsert(ArchAspace *space, PMMVAD vad) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadRemove                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Remove a VAD from the address space VAD list                       *
     ********************************************************************************/
    VOID VadRemove(ArchAspace *space, PMMVAD vad) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : AllocateVirtualMemory                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reserve and commit a virtual memory region in space                *
     ********************************************************************************/
    NO_DISCARD BOOL AllocateVirtualMemory(ArchAspace *space, PVOID *base_address,
                                          PSIZE_T region_size, ULONG allocation_type,
                                          ULONG protect) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : FreeVirtualMemory                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmap and release a previously allocated virtual memory region     *
     ********************************************************************************/
    NO_DISCARD BOOL FreeVirtualMemory(ArchAspace *space, PVOID *base_address,
                                      PSIZE_T region_size) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ProtectVirtualMemory                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Change the protection flags on a mapped virtual memory region      *
     ********************************************************************************/
    NO_DISCARD BOOL ProtectVirtualMemory(ArchAspace *space, PVOID *base_address,
                                         PSIZE_T number_of_bytes, ULONG new_protect,
                                         PULONG old_protect) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : QueryVirtualMemory                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return information about the VAD covering address in space         *
     ********************************************************************************/
    NO_DISCARD BOOL QueryVirtualMemory(ArchAspace *space, QWORD address, PVOID *base_address,
                                       PSIZE_T region_size, PULONG state, PULONG protect) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CalculatePageCommitment                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Count committed pages in [starting_address, ending_address)        *
     ********************************************************************************/
    NO_DISCARD ULONG CalculatePageCommitment(QWORD starting_address, QWORD ending_address,
                                             PMMVAD vad, ArchAspace *space) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : DeleteVirtualAddresses                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmap all pages in [va, ending_address) and remove their VADs      *
     ********************************************************************************/
    VOID DeleteVirtualAddresses(QWORD va, QWORD ending_address, ArchAspace *space) noexcept;

} // namespace trunk::mem