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
 *                                                                               *
 *  AUTHOR  : Trollycat                                                          *
 *  FILE    : src/trunk/arch/x86_64/boot/Boot.cpp                                *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Multiboot2 bridge. Validates the bootloader handoff, parses        *
 *            the MB2 info struct into a clean BootInfo, then calls kmain.       *
 *            This is the only file in the kernel that knows MB2 exists.         *
 *                                                                               *
 ********************************************************************************/

#include <trunk/boot/Boot.h>
#include <trunk/kernel/Kernel.h>

namespace trunk
{

    // ── MB2 constants ─────────────────────────────────────────────────────────────

    static constexpr u32 MB2_MAGIC = 0x36d76289;
    static constexpr u32 TAG_END = 0;
    static constexpr u32 TAG_MMAP = 6;
    static constexpr u32 TAG_BOOTLOADER = 2;
    static constexpr u32 MMAP_AVAILABLE = 1;
    static constexpr u32 MMAP_ACPI = 3;
    static constexpr u32 MMAP_NVS = 4;
    static constexpr u32 MMAP_BADRAM = 5;

    // ── Raw MB2 structs (private to this file) ────────────────────────────────────

    struct [[gnu::packed]] MB2Tag
    {
        u32 type;
        u32 size;
    };

    struct [[gnu::packed]] MB2MmapEntry
    {
        u64 base;
        u64 length;
        u32 type;
        u32 reserved;
    };

    struct [[gnu::packed]] MB2MmapTag
    {
        u32 type;
        u32 size;
        u32 entry_size;
        u32 entry_version;
        MB2MmapEntry entries[];
    };

    // ── Helpers ───────────────────────────────────────────────────────────────────

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : next_tag                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Advances to the next MB2 tag. Tags are 8-byte aligned.             *
     ********************************************************************************/
    [[nodiscard]]
    static const MB2Tag *next_tag(const MB2Tag *tag) noexcept
    {
        uptr addr = reinterpret_cast<uptr>(tag) + tag->size;
        addr = (addr + 7) & ~uptr{7};
        return reinterpret_cast<const MB2Tag *>(addr);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : parse_mmap                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Copies MB2 memory map entries into BootInfo using our own types.   *
     ********************************************************************************/
    static void parse_mmap(const MB2MmapTag *tag, BootInfo &info) noexcept
    {
        const uptr end = reinterpret_cast<uptr>(tag) + tag->size;
        const auto *entry = tag->entries;

        while (reinterpret_cast<uptr>(entry) < end && info.mmap_count < BootInfo::MAX_MMAP_ENTRIES)
        {
            auto &out = info.mmap[info.mmap_count++];
            out.base = entry->base;
            out.length = entry->length;

            switch (entry->type)
            {
            case MMAP_AVAILABLE:
                out.type = MemoryType::Available;
                break;
            case MMAP_ACPI:
                out.type = MemoryType::AcpiReclaimable;
                break;
            case MMAP_NVS:
                out.type = MemoryType::AcpiNvs;
                break;
            case MMAP_BADRAM:
                out.type = MemoryType::BadRam;
                break;
            default:
                out.type = MemoryType::Reserved;
                break;
            }

            entry = reinterpret_cast<const MB2MmapEntry *>(
                reinterpret_cast<uptr>(entry) + tag->entry_size);
        }
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : parse_mb2                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks all MB2 tags and fills a BootInfo struct.                    *
     ********************************************************************************/
    static void parse_mb2(uptr mb2_phys, BootInfo &info) noexcept
    {
        const uptr end = mb2_phys + *reinterpret_cast<const u32 *>(mb2_phys);
        const auto *tag = reinterpret_cast<const MB2Tag *>(mb2_phys + 8);

        while (reinterpret_cast<uptr>(tag) < end && tag->type != TAG_END)
        {
            switch (tag->type)
            {
            case TAG_MMAP:
                parse_mmap(reinterpret_cast<const MB2MmapTag *>(tag), info);
                break;
            case TAG_BOOTLOADER:
                info.bootloader_name =
                    reinterpret_cast<const char *>(reinterpret_cast<uptr>(tag) + 8);
                break;
            default:
                break;
            }
            tag = next_tag(tag);
        }
    }

    // ── Entry point ───────────────────────────────────────────────────────────────

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : boot_entry                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Called from Entry64.asm. Validates MB2 magic, builds BootInfo,     *
     *            calls kmain. Never returns.                                        *
     ********************************************************************************/
    extern "C" [[noreturn]]
    void boot_entry(u32 mb2_magic, u32 mb2_phys) noexcept
    {
        if (mb2_magic != MB2_MAGIC)
            for (;;)
            {
                asm volatile("cli; hlt");
            }

        BootInfo info{};
        parse_mb2(static_cast<uptr>(mb2_phys), info);

        trunk::kmain(info);

        for (;;)
        {
            asm volatile("cli; hlt");
        }
    }

} // namespace trunk::boot