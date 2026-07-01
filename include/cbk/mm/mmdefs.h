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
 *  MODULE  : Page definitions                                                   *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores definitions for page handling                               *
 ********************************************************************************/
#pragma once

#include <lddef.h>
#include <types.h>

#include <status.h>

#include <attributes.h>

#include <cbk/hal/io.h>

#define PAGE_ALIGN(addr) (((addr) + PAGE_SIZE - 1) & PAGE_MASK)

#define CONTAINING_RECORD(address, type, field)                                                    \
    ((type *)((PCHAR)(address) - (SIZE_T)(&((type *)0)->field)))

#define ASSERT_IS_CBK_PFN(pfn_num)                                                                 \
    ASSERT((pfn_num) != 0 && (pfn_num) <= mm_highest_physical_page, "Invalid PFN provided")

namespace cbk::mem
{
    using UnmapFn = CBKSTATUS (*)(QWORD);

    // Memory allocation flags... (tracking)
    constexpr ULONG MEM_COMMIT                  = 0x00001000;
    constexpr ULONG MEM_RESERVE                 = 0x00002000;
    constexpr ULONG MEM_REPLACE_PLACEHOLDER     = 0x00004000;
    constexpr ULONG MEM_DECOMMIT                = 0x00004000;
    constexpr ULONG MEM_RELEASE                 = 0x00008000;
    constexpr ULONG MEM_FREE                    = 0x00010000;
    constexpr ULONG MEM_RESET                   = 0x00080000;
    constexpr ULONG MEM_TOP_DOWN                = 0x00100000;
    constexpr ULONG MEM_WRITE_WATCH             = 0x00200000;
    constexpr ULONG MEM_PHYSICAL                = 0x00400000;
    constexpr ULONG MEM_ROTATE                  = 0x00800000;
    constexpr ULONG MEM_DIFFERENT_IMAGE_BASE_OK = 0x00800000;
    constexpr ULONG MEM_RESET_UNDO              = 0x01000000;
    constexpr ULONG MEM_LARGE_PAGES             = 0x20000000;
    constexpr ULONG MEM_4MB_PAGES               = 0x80000000;

    constexpr SIZE_T MEM_PFN_STATE_COUNT = 7;

    // Layout addresses...
    constexpr QWORD KERNEL_VMA   = 0xFFFFFFFF80000000ULL;
    constexpr QWORD PHYSMAP_BASE = 0xFFFF800000000000ULL;

    constexpr QWORD PHYS_ADDR_MAX        = 0xFFFFFFFFFFFFFFFFULL;
    constexpr QWORD PHYS_ADDR_48_BIT_MAX = 0x0000FFFFFFFFFFFFULL;
    constexpr QWORD PHYS_ADDR_52_BIT_MAX = 0x000FFFFFFFFFFFFFULL;
    constexpr QWORD PHYS_ADDR_16MB_MAX   = 0x01000000ULL;
    constexpr QWORD PHYS_ADDR_4GB_MAX    = 0x100000000ULL;

    // Metric units...
    constexpr QWORD KB = 1024ULL;
    constexpr QWORD MB = 1024ULL * KB;
    constexpr QWORD GB = 1024ULL * MB;

    // 4KB Page constants...
    constexpr QWORD PAGE_SIZE  = 4096;
    constexpr QWORD PAGE_MASK  = ~(PAGE_SIZE - 1);
    constexpr QWORD PAGE_SHIFT = 12;

    // 2MB Huge Page constants...
    constexpr QWORD HUGE_PAGE_SIZE = 2 * MB;
    constexpr QWORD HUGE_MASK      = ~(HUGE_PAGE_SIZE - 1);
    constexpr QWORD PAGE_HUGE      = QWORD{1} << 7;

    // Block Dimensions...
    constexpr QWORD PML4_BLOCK_SIZE = 512ULL * GB;
    constexpr QWORD PML4_BLOCK_MASK = ~(PML4_BLOCK_SIZE - 1);
    constexpr QWORD PDPT_BLOCK_SIZE = 1ULL * GB;
    constexpr QWORD PDPT_BLOCK_MASK = ~(PDPT_BLOCK_SIZE - 1);
    constexpr QWORD PD_BLOCK_SIZE   = 2ULL * MB;
    constexpr QWORD PD_BLOCK_MASK   = ~(PD_BLOCK_SIZE - 1);

    // Caching control flags...
    constexpr QWORD PAGE_WRITE_THROUGH = 1ULL << 3;
    constexpr QWORD PAGE_CACHE_DISABLE = 1ULL << 4;
    constexpr QWORD PAGE_PAT           = 1ULL << 7;

    constexpr QWORD MM_IO_COHERENT_MEMORY = 0x00000001;
    constexpr QWORD MM_IO_NONCACHED       = 0x00000002;
    constexpr QWORD MM_IO_WRITE_COMBINED  = 0x00000004;

    // Page table entry access flags...
    constexpr QWORD PAGE_PRESENT  = (1ULL << 0);
    constexpr QWORD PAGE_WRITABLE = (1ULL << 1);
    constexpr QWORD PAGE_USER     = (1ULL << 2);
    constexpr QWORD PAGE_PWT      = (1ULL << 3);
    constexpr QWORD PAGE_PCD      = (1ULL << 4);
    constexpr QWORD PAGE_ACCESSED = (1ULL << 5);
    constexpr QWORD PAGE_DIRTY    = (1ULL << 6);
    constexpr QWORD PAGE_GLOBAL   = (1ULL << 8);
    constexpr QWORD PAGE_NX       = (1ULL << 63);

    // Tracking protection constants...
    constexpr ULONG PAGE_NOACCESS          = 0x00000001;
    constexpr ULONG PAGE_READONLY          = 0x00000002;
    constexpr ULONG PAGE_READWRITE         = 0x00000004;
    constexpr ULONG PAGE_WRITECOPY         = 0x00000008;
    constexpr ULONG PAGE_EXECUTE           = 0x00000010;
    constexpr ULONG PAGE_EXECUTE_READ      = 0x00000020;
    constexpr ULONG PAGE_EXECUTE_READWRITE = 0x00000040;
    constexpr ULONG PAGE_EXECUTE_WRITECOPY = 0x00000080;
    constexpr ULONG PAGE_GUARD             = 0x00000100;
    constexpr ULONG PAGE_NOCACHE           = 0x00000200;
    constexpr ULONG PAGE_WRITECOMBINE      = 0x00000400;

    constexpr ULONG IDX_BITSHIFT        = 0x1FF;
    constexpr ULONG VAD_STATE_COMMITTED = 0x02;

    // Section permissions... (linker)
    constexpr QWORD TEXT_SECTION_HW_FLAGS   = PAGE_PRESENT | PAGE_GLOBAL;
    constexpr QWORD RODATA_SECTION_HW_FLAGS = PAGE_PRESENT | PAGE_GLOBAL | PAGE_NX;
    constexpr QWORD BSS_SECTION_HW_FLAGS    = PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL | PAGE_NX;

    constexpr QWORD PTE_AVAIL          = 0xE00;
    constexpr QWORD PTE_USER           = PTE_AVAIL | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    constexpr QWORD PTE_FRAME_MASK     = 0x000FFFFFFFFFF000ULL;
    constexpr QWORD PTE_DIRTY_FLAG_BIT = 1ULL;
    constexpr QWORD NO_OF_PT_ENTRIES   = 512;

    constexpr QWORD ARR_BOOT_NODE_SIZE = 1024;

    // User mode, kernel mode definitions
    constexpr QWORD MM_USER_SPACE_START = 0x0000000000000000ULL;
    constexpr QWORD MM_USER_SPACE_END   = 0x00007FFFFFFFFFFFULL;

    constexpr QWORD MM_KERNEL_SPACE_START = PHYSMAP_BASE;
    constexpr QWORD MM_KERNEL_SPACE_END   = PHYS_ADDR_MAX;

#define MM_IS_USER_ADDRESS(addr) ((static_cast<QWORD>(addr)) <= MM_USER_SPACE_END)
#define MM_IS_KERNEL_ADDRESS(addr) ((static_cast<QWORD>(addr)) >= MM_KERNEL_SPACE_START)

    union PAGE_TABLE_ENTRY {
        QWORD val;

        struct
        {
            QWORD present : 1;
            QWORD writable : 1;
            QWORD user : 1;
            QWORD pwt : 1;
            QWORD pcd : 1;
            QWORD accessed : 1;
            QWORD dirty : 1;
            QWORD large_page : 1;
            QWORD global : 1;
            QWORD available : 3;
            QWORD page_frame : 40;
            QWORD reserved : 11;
            QWORD no_execute : 1;
        } Bits;
    };

    struct ArchAspace
    {
        QWORD *pml4_virt;
        QWORD pml4_phys;
        QWORD base;
        SIZE_T size;
    };

    struct ListEntry
    {
        ListEntry *flink;
        ListEntry *blink;
    };

    struct MmRmapEntry
    {
        LIST_ENTRY list_entry;
        ArchAspace *space;
        PVOID virtual_address;
    };

    struct PageTable
    {
        PAGE_TABLE_ENTRY entries[NO_OF_PT_ENTRIES];
    };

    struct PteContext
    {
        QWORD payload;
        QWORD extra;
    };

    struct PteInfo
    {
        PPAGE_TABLE_ENTRY entry;
        QWORD va;
        QWORD base_phys;
        SIZE_T step_size;
        SIZE_T chunk_size;
    };

    struct NCacheDescriptor
    {
        PVOID virt_address;
        SIZE_T size;
        QWORD phys_base;
        QWORD alloc_tag;
    };

    struct MmLevelTraits
    {
        SIZE_T bytes_spanned;
        UnmapFn unmap;
    };

    enum class MM_PFN_STATE : BYTE
    {
        ZEROED_PAGE_LIST   = 0,
        FREE_PAGE_LIST     = 1,
        RESERVED_PAGE_LIST = 2,
        BAD_PAGE_LIST      = 3,
        FIRMWARE_PAGE_LIST = 4,
        ACTIVE_AND_VALID   = 6
    };

    enum class MC_TYPE : ULONG
    {
        SYSTEM     = 1,
        USER       = 2,
        NPPOOL     = 3,
        PPOOL      = 4,
        CACHE      = 5,
        CONTIGUOUS = 6
    };

    enum class PAGING_LEVEL : ULONG
    {
        PML4 = 39,
        PDPT = 30,
        PD   = 21,
        PT   = 12
    };

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PML4X                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PML4 table index                                       *
     ********************************************************************************/
    NO_DISCARD ALWAYS_INLINE constexpr QWORD
    PML4X(QWORD la) noexcept
    {
        return (la >> static_cast<ULONG>(PAGING_LEVEL::PML4)) & IDX_BITSHIFT;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDPTX                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the PDP table index                                        *
     ********************************************************************************/
    NO_DISCARD ALWAYS_INLINE constexpr QWORD
    PDPTX(QWORD la) noexcept
    {
        return (la >> static_cast<ULONG>(PAGING_LEVEL::PDPT)) & IDX_BITSHIFT;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PDX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page directory index                                   *
     ********************************************************************************/
    NO_DISCARD ALWAYS_INLINE constexpr QWORD
    PDX(QWORD la) noexcept
    {
        return (la >> static_cast<ULONG>(PAGING_LEVEL::PD)) & IDX_BITSHIFT;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTX                                                                 *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the page table index                                       *
     ********************************************************************************/
    NO_DISCARD ALWAYS_INLINE constexpr QWORD
    PTX(QWORD la) noexcept
    {
        return (la >> static_cast<ULONG>(PAGING_LEVEL::PT)) & IDX_BITSHIFT;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGOFF                                                               *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the byte offset within a page                              *
     ********************************************************************************/
    NO_DISCARD ALWAYS_INLINE constexpr QWORD
    PGOFF(QWORD la) noexcept
    {
        return la & (PAGE_SIZE - 1);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PGADDR                                                              *
     * DATE    : 2026                                                                *
     * PURPOSE : Constructs a virtual address from indexes                           *
     ********************************************************************************/
    NO_DISCARD ALWAYS_INLINE constexpr QWORD
    PGADDR(QWORD m, QWORD d, QWORD p, QWORD t, QWORD o) noexcept
    {
        return (m << static_cast<ULONG>(PAGING_LEVEL::PML4)) |
               (d << static_cast<ULONG>(PAGING_LEVEL::PDPT)) |
               (p << static_cast<ULONG>(PAGING_LEVEL::PD)) |
               (t << static_cast<ULONG>(PAGING_LEVEL::PT)) | (o & (PAGE_SIZE - 1));
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PTE_ADDR                                                            *
     * DATE    : 2026                                                                *
     * PURPOSE : Extracts the physical address from an entry                         *
     ********************************************************************************/
    NO_DISCARD constexpr ULONG_PTR
    PTE_ADDR(ULONG_PTR pte) noexcept
    {
        return pte & PTE_FRAME_MASK;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PageAlignDown                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address down to page boundary                             *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    PageAlignDown(QWORD addr) noexcept
    {
        return addr & PAGE_MASK;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PageAlignUp                                                         *
     * DATE    : 2026                                                                *
     * PURPOSE : Aligns an address up to page boundary                               *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    PageAlignUp(QWORD addr) noexcept
    {
        return (addr + PAGE_SIZE - 1) & PAGE_MASK;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsPageAligned                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Returns TRUE if addr is 4KB aligned                                *
     ********************************************************************************/
    NO_DISCARD INLINE BOOL
    IsPageAligned(QWORD addr) noexcept
    {
        return (addr & (PAGE_SIZE - 1)) == 0;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : PaddrToKvaddr                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a physical address to a virtual address                    *
     ********************************************************************************/
    NO_DISCARD INLINE PVOID
    PaddrToKvaddr(QWORD paddr) noexcept
    {
        if (paddr >= PHYS_ADDR_48_BIT_MAX)
            return nullptr;
        return reinterpret_cast<PVOID>(paddr + PHYSMAP_BASE);
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : KvaddrToPaddr                                                       *
     * DATE    : 2026                                                                *
     * PURPOSE : Converts a virtual address to a physical adddress                   *
     ********************************************************************************/
    NO_DISCARD INLINE QWORD
    KvaddrToPaddr(QWORD kvaddr) noexcept
    {
        if (kvaddr >= KERNEL_VMA) {
            QWORD k_phys_start = reinterpret_cast<QWORD>(__kernel_phys_start);
            return (kvaddr - KERNEL_VMA) + k_phys_start;
        }

        if (kvaddr < PHYSMAP_BASE)
            return PHYS_ADDR_MAX;

        return kvaddr - PHYSMAP_BASE;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InitializeListHead                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes a list head to point to itself                         *
     ********************************************************************************/
    INLINE VOID
    InitializeListHead(PLIST_ENTRY list_head) noexcept
    {
        list_head->flink = list_head;
        list_head->blink = list_head;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsListEmpty                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks If a doubly-linked list is empty                            *
     ********************************************************************************/
    NO_DISCARD INLINE BOOL
    IsListEmpty(PLIST_ENTRY list_head) noexcept
    {
        return (BOOL)(list_head->flink == list_head);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InsertHeadList                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Inserts an entry at the beginning of the list                      *
     ********************************************************************************/
    INLINE VOID
    InsertHeadList(PLIST_ENTRY list_head, PLIST_ENTRY entry) noexcept
    {
        PLIST_ENTRY flink = list_head->flink;
        entry->flink      = flink;
        entry->blink      = list_head;
        flink->blink      = entry;
        list_head->flink  = entry;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InsertTailList                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Inserts an entry at the beginning of the list                      *
     ********************************************************************************/
    INLINE VOID
    InsertTailList(PLIST_ENTRY list_head, PLIST_ENTRY entry) noexcept
    {
        PLIST_ENTRY blink = list_head->blink;
        entry->flink      = list_head;
        entry->blink      = blink;
        blink->flink      = entry;
        list_head->blink  = entry;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RemoveEntryList                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Removes an entry from anywhere in the list in 0(1) time            *
     ********************************************************************************/
    INLINE VOID
    RemoveEntryList(PLIST_ENTRY entry) noexcept
    {
        PLIST_ENTRY flink = entry->flink;
        PLIST_ENTRY blink = entry->blink;
        blink->flink      = flink;
        flink->blink      = blink;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TlbFlushPage                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the TLB entry for a single virtual address on this CPU       *
     ********************************************************************************/
    INLINE VOID
    TlbFlushPage(QWORD va) noexcept
    {
        hal::InvLpg(va);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TlbFlushAll                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Flush the entire TLB by reloading CR3                              *
     ********************************************************************************/
    INLINE VOID
    TlbFlushAll() noexcept
    {
        QWORD cr3 = hal::ReadCr3();
        hal::WriteCr3(cr3);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MemoryFence                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Execute the mfence instruction (wrapper)                           *
     ********************************************************************************/
    INLINE VOID
    MemoryFence() noexcept
    {
        hal::MFence();
    }

} // namespace cbk::mem
