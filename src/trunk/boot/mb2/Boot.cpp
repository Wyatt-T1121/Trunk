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
 *  FILE    : Boot.cpp                                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Multiboot2 bridge and ELF kernel loader.                           *
 *            Validates the bootloader handoff, parses the MB2 info struct       *
 *            into a clean BootInfo, locates the troskern.elf module that        *
 *            GRUB loaded into physical RAM, hands it to elf_load(), then        *
 *            jumps to the kernel virtual entry point.                           *
 *                                                                               *
 *            This is the only file in the boot stage that knows MB2 exists.     *
 *            No tklib dependency — boot stage is fully isolated.                *
 *                                                                               *
 ********************************************************************************/

#include <trunk/boot/Boot.h>
#include <trunk/boot/bu/ELoader.h>
#include <trunk/boot/bu/BString.h>

namespace trunk::boot
{

    // MB2 constants

    static constexpr u32 MB2_MAGIC = 0x36d76289;
    static constexpr u32 TAG_END = 0;
    static constexpr u32 TAG_MODULE = 3;
    static constexpr u32 TAG_MMAP = 6;
    static constexpr u32 TAG_BOOTLOADER = 2;
    static constexpr u32 MMAP_AVAILABLE = 1;
    static constexpr u32 MMAP_ACPI = 3;
    static constexpr u32 MMAP_NVS = 4;
    static constexpr u32 MMAP_BADRAM = 5;

    // Raw MB2 structs

    struct [[gnu::packed]] MB2Tag
    {
        u32 type;
        u32 size;
    };

    struct [[gnu::packed]] MB2ModuleTag
    {
        u32 type; // TAG_MODULE = 3
        u32 size;
        u32 mod_start;  // physical address of module start
        u32 mod_end;    // physical address of module end
        char cmdline[]; // null-terminated module command line string
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

    // Helpers

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
     *  PURPOSE : Copies MB2 memory map entries into BootInfo.                       *
     ********************************************************************************/
    static void parse_mmap(const MB2MmapTag *tag, BootInfo &info) noexcept
    {
        const uptr end = reinterpret_cast<uptr>(tag) + tag->size;
        const auto *entry = tag->entries;

        while (reinterpret_cast<uptr>(entry) < end &&
               info.mmap_count < BootInfo::MAX_MMAP_ENTRIES)
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
     *            Also returns the physical address of the first module found        *
     *            (troskern.elf), or 0 if no module tag is present.                  *
     ********************************************************************************/
    static uptr parse_mb2(uptr mb2_phys, BootInfo &info) noexcept
    {
        const uptr end = mb2_phys + *reinterpret_cast<const u32 *>(mb2_phys);
        const auto *tag = reinterpret_cast<const MB2Tag *>(mb2_phys + 8);

        uptr kernel_phys = 0;

        while (reinterpret_cast<uptr>(tag) < end && tag->type != TAG_END)
        {
            switch (tag->type)
            {
            case TAG_MMAP:
                parse_mmap(reinterpret_cast<const MB2MmapTag *>(tag), info);
                break;

            case TAG_BOOTLOADER:
                info.bootloader_name =
                    reinterpret_cast<const char *>(
                        reinterpret_cast<uptr>(tag) + 8);
                break;

            case TAG_MODULE:
            {
                // Only take the first module — that is troskern.elf.
                // Additional modules are ignored for now.
                if (kernel_phys == 0)
                {
                    const auto *mod =
                        reinterpret_cast<const MB2ModuleTag *>(tag);
                    kernel_phys = static_cast<uptr>(mod->mod_start);
                }
                break;
            }

            default:
                break;
            }

            tag = next_tag(tag);
        }

        return kernel_phys;
    }

    // Entry point

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : boot_entry                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Called from Entry64.asm. Validates MB2 magic, builds BootInfo,     *
     *            locates and loads troskern.elf via elf_load(), then jumps to the   *
     *            kernel virtual entry point. Never returns.                         *
     ********************************************************************************/
    extern "C" [[noreturn]]
    void boot_entry(u32 mb2_magic, u32 mb2_phys) noexcept
    {
        // Validate MB2 magic — if GRUB didn't boot us, halt immediately
        if (mb2_magic != MB2_MAGIC)
            for (;;)
            {
                asm volatile("cli; hlt");
            }

        // Parse MB2 info struct — fills BootInfo, returns kernel module address
        BootInfo info{};
        uptr kernel_phys = parse_mb2(static_cast<uptr>(mb2_phys), info);

        // No module found — troskern.elf was not loaded by GRUB
        if (kernel_phys == 0)
            for (;;)
            {
                asm volatile("cli; hlt");
            }

        // Load the kernel ELF from physical RAM
        ElfResult result = elf_load(kernel_phys);

        // ELF load failed — halt
        if (result.error != ElfError::None)
            for (;;)
            {
                asm volatile("cli; hlt");
            }

        // Cast the virtual entry point to a function pointer and jump.
        // The kernel expects a single const BootInfo& argument.
        // At this point paging is active — the higher-half is mapped.
        using KernelEntry = void (*)(const BootInfo &);
        auto kmain = reinterpret_cast<KernelEntry>(result.entry);
        kmain(info);

        // Unreachable — kmain is [[noreturn]]
        for (;;)
        {
            asm volatile("cli; hlt");
        }
    }

} // namespace trunk::boot