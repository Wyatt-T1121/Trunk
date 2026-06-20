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
#include <cbk/mem/virt/virtual.h>

#include <cbk/mem/alloc/freelist.h>
#include <cbk/mem/alloc/memblock.h>
#include <cbk/mem/arch/mmarch.h>
#include <cbk/mem/pfn/pfn.h>
#include <cbk/mem/util/list.h>
#include <cbk/mem/virt/mmu.h>

#include <assert.h>
#include <macros.h>

#include <tklib/math.h>

namespace trunk::mem
{

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : ProtectionToMmuFlags                                               *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Convert a PAGE_* constant to MMU page flag bits                    *
         ********************************************************************************/
        NO_DISCARD QWORD ProtectionToMmuFlags(ULONG protect) noexcept
        {
            QWORD flags = PAGE_PRESENT | PAGE_WRITABLE;

            switch (protect) {
            case PAGE_NOACCESS:
                return 0;
            case PAGE_READONLY:
                flags = PAGE_PRESENT | PAGE_NX;
                break;
            case PAGE_READWRITE:
                flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX;
                break;
            case PAGE_EXECUTE:
                flags = PAGE_PRESENT;
                break;
            case PAGE_EXECUTE_READWRITE:
                flags = PAGE_PRESENT | PAGE_WRITABLE;
                break;
            default:
                flags = PAGE_PRESENT | PAGE_WRITABLE | PAGE_NX;
                break;
            }

            return flags;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : VadOverlaps                                                        *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Returns true if [base, base+size) overlaps the given VAD           *
         ********************************************************************************/
        NO_DISCARD BOOL VadOverlaps(PMMVAD vad, QWORD base, SIZE_T size) noexcept
        {
            if (tklib::math::add_would_overflow(base, size)) UNLIKELY
                return true;

            QWORD vad_end   = vad->starting_address + vad->size;
            QWORD range_end = base + size;
            return base < vad_end && range_end > vad->starting_address;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : UnmapAndFreeRange                                                  *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Safely unmaps hardware entries and dereferences underlying frames  *
         ********************************************************************************/
        VOID UnmapAndFreeRange(ArchAspace *space, QWORD start_va, SIZE_T size) noexcept
        {
            QWORD va  = start_va;
            QWORD end = start_va + size;

            while (va < end) {
                QWORD phys = MmuTranslate(space, va);
                if (phys != 0) {
                    MmDereferencePage(AddrToPfn(phys));
                    ASSERT(MmuUnmapPage(space, va),
                           "UnmapAndFreeRange: Failed to tear down hardware mapping");
                }
                va += PAGE_SIZE;
            }
        }

    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadFind                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Search the VAD list of space for the entry covering address        *
     ********************************************************************************/
    NO_DISCARD PMMVAD VadFind(ArchAspace *space, QWORD address) noexcept
    {
        ASSERT(space != nullptr, "VadFind: space is null");

        PLIST_ENTRY entry = space->vad_list_head.flink;

        while (entry != &space->vad_list_head) {
            PMMVAD vad = CONTAINING_RECORD(entry, MmVad, entry);

            if (address >= vad->starting_address && address < (vad->starting_address + vad->size))
                return vad;
            if (address < vad->starting_address)
                break;

            entry = entry->flink;
        }

        return nullptr;
    }
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadInsert                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Insert a new VAD into the address space VAD list                   *
     ********************************************************************************/
    VOID VadInsert(ArchAspace *space, PMMVAD vad) noexcept
    {
        ASSERT(space != nullptr, "VadInsert: space is null");
        ASSERT(vad != nullptr, "VadInsert: vad is null");

        PLIST_ENTRY current = space->vad_list_head.flink;

        while (current != &space->vad_list_head) {
            PMMVAD existing = CONTAINING_RECORD(current, MmVad, entry);
            if (existing->starting_address > vad->starting_address)
                break;
            current = current->flink;
        }

        vad->entry.flink      = current;
        vad->entry.blink      = current->blink;
        current->blink->flink = &vad->entry;
        current->blink        = &vad->entry;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadRemove                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Remove a VAD from the address space VAD list                       *
     ********************************************************************************/
    VOID VadRemove(ArchAspace *space, PMMVAD vad) noexcept
    {
        ASSERT(space != nullptr, "VadRemove: space is null");
        ASSERT(vad != nullptr, "VadRemove: vad is null");

        RemoveEntryList(&vad->entry);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : AllocateVirtualMemory                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Reserve and commit a virtual memory region in space                *
     ********************************************************************************/
    NO_DISCARD BOOL AllocateVirtualMemory(ArchAspace *space, PVOID *base_address,
                                          PSIZE_T region_size, ULONG allocation_type,
                                          ULONG protect) noexcept
    {
        ASSERT(space != nullptr, "AllocateVirtualMemory: space is null");
        ASSERT(base_address != nullptr, "AllocateVirtualMemory: base_address is null");
        ASSERT(region_size != nullptr, "AllocateVirtualMemory: region_size is null");
        ASSERT(*region_size > 0, "AllocateVirtualMemory: region_size is zero");

        if (tklib::math::add_would_overflow(*region_size, PAGE_SIZE - 1)) UNLIKELY
            return false;

        SIZE_T size = (*region_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
        QWORD base  = reinterpret_cast<QWORD>(*base_address) & ~(PAGE_SIZE - 1);

        if (tklib::math::add_would_overflow(base, size)) UNLIKELY
            return false;

        PLIST_ENTRY entry = space->vad_list_head.flink;
        while (entry != &space->vad_list_head) {
            PMMVAD existing = CONTAINING_RECORD(entry, MmVad, entry);
            if (VadOverlaps(existing, base, size))
                return false;
            if (existing->starting_address >= (base + size))
                break;
            entry = entry->flink;
        }

        QWORD mmu_flags = ProtectionToMmuFlags(protect);
        QWORD curr_va   = base;
        SIZE_T mapped   = 0;

        while (mapped < size) {

            PFN_NUM pfn = MmAllocPage(static_cast<ULONG>(MC_TYPE::USER));

            if (pfn == 0) {
                UnmapAndFreeRange(space, base, mapped);
                return false;
            }

            QWORD phys = PfnToAddr(pfn);
            if (!MmuMapPage(space, curr_va, phys, mmu_flags)) {
                MmDereferencePage(pfn);
                UnmapAndFreeRange(space, base, mapped);
                return false;
            }

            curr_va += PAGE_SIZE;
            mapped  += PAGE_SIZE;
        }

        QWORD vad_phys = MemblockAlloc(sizeof(MmVad), alignof(MmVad));
        if (!vad_phys) {
            UnmapAndFreeRange(space, base, size);
            return false;
        }

        PMMVAD vad = reinterpret_cast<PMMVAD>(PaddrToKvaddr(vad_phys));

        vad->starting_address = base;
        vad->size             = size;
        vad->state            = VAD_STATE_COMMITTED;
        vad->protect          = protect;

        VadInsert(space, vad);

        *base_address = reinterpret_cast<PVOID>(base);
        *region_size  = size;

        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : FreeVirtualMemory                                                  *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmap and release a previously allocated virtual memory region     *
     ********************************************************************************/
    NO_DISCARD BOOL FreeVirtualMemory(ArchAspace *space, PVOID *base_address,
                                      PSIZE_T region_size) noexcept
    {
        ASSERT(space != nullptr, "FreeVirtualMemory: space is null");
        ASSERT(base_address != nullptr, "FreeVirtualMemory: base_address is null");
        ASSERT(region_size != nullptr, "FreeVirtualMemory: region_size is null");

        QWORD base  = reinterpret_cast<QWORD>(*base_address) & ~(PAGE_SIZE - 1);
        SIZE_T size = (*region_size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        if (size == 0)
            return false;

        PMMVAD vad = VadFind(space, base);
        if (!vad)
            return false;

        QWORD requested_end = base + size;
        QWORD vad_end       = vad->starting_address + vad->size;
        if (base < vad->starting_address || requested_end > vad_end)
            return false;

        if (base == vad->starting_address && size == vad->size) {
            UnmapAndFreeRange(space, base, size);
            VadRemove(space, vad);

            PVOID free_paddr = KvaddrToPaddr(reinterpret_cast<QWORD>(vad));
            ASSERT(free_paddr != nullptr, "FreeVirtualMemory: KvaddrToPaddr mapping returned null");

            QWORD paddr = reinterpret_cast<QWORD>(free_paddr);
            MemblockFree(paddr, sizeof(MmVad));

        } else if (base == vad->starting_address) {
            UnmapAndFreeRange(space, base, size);
            vad->starting_address += size;
            vad->size             -= size;
        } else if (requested_end == vad_end) {
            UnmapAndFreeRange(space, base, size);
            vad->size -= size;
        } else {
            QWORD right_base  = requested_end;
            SIZE_T right_size = vad_end - requested_end;

            QWORD new_vad_phys = MemblockAlloc(sizeof(MmVad), alignof(MmVad));
            if (!new_vad_phys) {
                ASSERT(false,
                       "FreeVirtualMemory: Out of memory while trying to split VAD descriptor");
                return false;
            }

            UnmapAndFreeRange(space, base, size);

            vad->size = base - vad->starting_address;

            PMMVAD new_vad            = reinterpret_cast<PMMVAD>(PaddrToKvaddr(new_vad_phys));
            new_vad->starting_address = right_base;
            new_vad->size             = right_size;
            new_vad->state            = vad->state;
            new_vad->protect          = vad->protect;

            VadInsert(space, new_vad);
        }

        *base_address = reinterpret_cast<PVOID>(base);
        *region_size  = size;

        return true;
    }
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ProtectVirtualMemory                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Change the protection flags on a mapped virtual memory region      *
     ********************************************************************************/
    NO_DISCARD BOOL ProtectVirtualMemory(ArchAspace *space, PVOID *base_address,
                                         PSIZE_T number_of_bytes, ULONG new_protect,
                                         PULONG old_protect) noexcept
    {
        ASSERT(space != nullptr, "ProtectVirtualMemory: space is null");
        ASSERT(base_address != nullptr, "ProtectVirtualMemory: base_address is null");
        ASSERT(number_of_bytes != nullptr, "ProtectVirtualMemory: number_of_bytes is null");
        ASSERT(*number_of_bytes > 0, "ProtectVirtualMemory: number_of_bytes is zero");
        ASSERT(old_protect != nullptr, "ProtectVirtualMemory: old_protect is null");

        if (tklib::math::add_would_overflow(*number_of_bytes, PAGE_SIZE - 1)) UNLIKELY
            return false;

        QWORD base  = reinterpret_cast<QWORD>(*base_address) & ~(PAGE_SIZE - 1);
        SIZE_T size = (*number_of_bytes + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);

        PMMVAD vad = VadFind(space, base);
        if (!vad)
            return false;

        if ((base + size) > (vad->starting_address + vad->size))
            return false;

        if (old_protect)
            *old_protect = vad->protect;

        QWORD mmu_flags = ProtectionToMmuFlags(new_protect);
        QWORD va        = base;
        QWORD end       = base + size;

        while (va < end) {
            if (MmuIsMapped(space, va))
                ASSERT(MmuProtect(space, va, mmu_flags), "ProtectVirtualMemory: MmuProtect failed");
            va += PAGE_SIZE;
        }

        vad->protect = new_protect;

        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : QueryVirtualMemory                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Return information about the VAD covering address in space         *
     ********************************************************************************/
    NO_DISCARD BOOL QueryVirtualMemory(ArchAspace *space, QWORD address, PVOID *base_address,
                                       PSIZE_T region_size, PULONG state, PULONG protect) noexcept
    {
        ASSERT(space != nullptr, "QueryVirtualMemory: space is null");

        PMMVAD vad = VadFind(space, address);
        if (!vad)
            return false;

        if (base_address)
            *base_address = reinterpret_cast<PVOID>(vad->starting_address);
        if (region_size)
            *region_size = vad->size;
        if (state)
            *state = vad->state;
        if (protect)
            *protect = vad->protect;

        return true;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CalculatePageCommitment                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Count committed pages in [starting_address, ending_address)        *
     ********************************************************************************/
    NO_DISCARD ULONG CalculatePageCommitment(QWORD starting_address, QWORD ending_address,
                                             PMMVAD vad, ArchAspace *space) noexcept
    {
        ASSERT(space != nullptr, "CalculatePageCommitment: space is null");
        ASSERT(vad != nullptr, "CalculatePageCommitment: vad is null");
        ASSERT(ending_address > starting_address,
               "CalculatePageCommitment: ending_address must be greater than starting_address");

        ULONG committed = 0;
        QWORD va        = starting_address;

        while (va < ending_address) {
            if (MmuIsMapped(space, va))
                committed++;
            va += PAGE_SIZE;
        }

        return committed;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : DeleteVirtualAddresses                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmap all pages in [va, ending_address) and remove their VADs      *
     ********************************************************************************/
    VOID DeleteVirtualAddresses(QWORD va, QWORD ending_address, ArchAspace *space) noexcept
    {
        ASSERT(space != nullptr, "DeleteVirtualAddresses: space is null");
        ASSERT(ending_address > va, "DeleteVirtualAddresses: empty range");

        while (va < ending_address) {
            PMMVAD vad = VadFind(space, va);

            if (vad) {
                UnmapAndFreeRange(space, vad->starting_address, vad->size);
                QWORD next_va = vad->starting_address + vad->size;
                VadRemove(space, vad);

                PVOID free_paddr = KvaddrToPaddr(reinterpret_cast<QWORD>(vad));
                ASSERT(free_paddr != nullptr,
                       "DeleteVirtualAddresses: KvaddrToPaddr returned null");

                QWORD paddr = reinterpret_cast<QWORD>(free_paddr);
                MemblockFree(paddr, sizeof(MmVad));

                va = next_va;
            } else {
                va += PAGE_SIZE;
            }
        }
    }
} // namespace trunk::mem