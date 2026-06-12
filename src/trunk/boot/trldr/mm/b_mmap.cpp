/* ******************************************************************************
 *                                                                              *
 *  Copyright 2026 Trollycat                                                    *
 *                                                                              *
 *  Licensed under the Apache License, Version 2.0 (the "License");             *
 *  you may not use this file except in compliance with the License.            *
 *  You may obtain a copy of the License at                                     *
 *                                                                              *
 *      http://www.apache.org/licenses/LICENSE-2.0                              *
 *                                                                              *
 *  Unless required by applicable law or agreed to in writing, software         *
 *  distributed under the License is distributed on an "AS IS" BASIS,           *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
 *  See the License for the specific language governing permissions and         *
 *  limitations under the License.                                              *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  MODULE  : Bootstrapping                                                     *
 *  DATE    : 2026                                                              *
 *  PURPOSE : MB2 info struct parser for the boot stage.                        *
 *            Owns all raw Multiboot2 struct definitions. Extracts the memory   *
 *            map and bootloader name into BootInfo.                            *
 *                                                                              *
 * *****************************************************************************/

#include <trunk/boot/mm/b_mmap.h>

#include <tklib/string.h>

#include <macros.h>

namespace trunk::boot
{

    static constexpr u32 TAG_END = 0;
    static constexpr u32 TAG_BOOTLOADER = 2;
    static constexpr u32 TAG_MMAP = 6;
    static constexpr u32 MMAP_AVAILABLE = 1;
    static constexpr u32 MMAP_ACPI = 3;
    static constexpr u32 MMAP_NVS = 4;
    static constexpr u32 MMAP_BADRAM = 5;

    struct GNU_PACKED MB2Tag
    {
        u32 type;
        u32 size;
    };

    struct GNU_PACKED MB2MmapEntry
    {
        u64 base;
        u64 length;
        u32 type;
        u32 reserved;
    };

    struct GNU_PACKED MB2MmapTag
    {
        u32 type;
        u32 size;
        u32 entry_size;
        u32 entry_version;
        MB2MmapEntry entries[];
    };

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : next_tag                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Advance to the next MB2 tag. Tags are 8-byte aligned.             *
     *                                                                              *
     * *****************************************************************************/
    [[nodiscard]]
    static const MB2Tag *next_tag(const MB2Tag *tag) noexcept
    {
        uptr addr = reinterpret_cast<uptr>(tag) + tag->size;
        addr = (addr + 7) & ~uptr{7};
        return reinterpret_cast<const MB2Tag *>(addr);
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : parse_mmap                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Walk MB2 memory map entries and copy them into BootInfo.          *
     *                                                                              *
     * *****************************************************************************/
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

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : parse_mb2                                                         *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Walk all MB2 tags and populate BootInfo with the memory map       *
     *            and bootloader name.                                              *
     *                                                                              *
     * *****************************************************************************/
    void parse_mb2(uptr mb2_phys, BootInfo &info) noexcept
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
            {
                const char *src = reinterpret_cast<const char *>(
                    reinterpret_cast<uptr>(tag) + 8);
                tklib::strlcpy(info.bootloader_name, src,
                               BootInfo::BOOTLOADER_NAME_MAX);
                break;
            }

            default:
                break;
            }

            tag = next_tag(tag);
        }
    }

} // namespace trunk::boot