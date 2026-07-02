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
 *  AUTHOR  : Trollycat                                                         *
 *  MODULE  : Bootstrapping                                                     *
 *  DATE    : 2026                                                              *
 *  PURPOSE : MB2 info struct parser for the boot stage.                        *
 * *****************************************************************************/
#include <boot/trldr/mem/mmap.h>

#include <tklib/string.h>

#include <attributes.h>

namespace cbk::boot
{
    struct GNU_PACKED MB2Tag
    {
        DWORD type;
        DWORD size;
    };

    struct GNU_PACKED MB2MmapEntry
    {
        QWORD base;
        QWORD length;
        DWORD type;
        DWORD reserved;
    };

    struct GNU_PACKED MB2MmapTag
    {
        DWORD type;
        DWORD size;
        DWORD entry_size;
        DWORD entry_version;
        MB2MmapEntry entries[1];
    };

    namespace
    {
        /* ******************************************************************************
         *  AUTHOR  : Trollycat                                                         *
         *  FUNC    : IniAdvanceToNextMultiboot2Tag                                     *
         *  DATE    : 2026                                                              *
         *  PURPOSE : Advance to the next MB2 tag                                       *
         * *****************************************************************************/
        NO_DISCARD INLINE const MB2Tag *
        IniAdvanceToNextMultiboot2Tag(const MB2Tag *tag) noexcept
        {
            ULONG_PTR addr = reinterpret_cast<ULONG_PTR>(tag) + tag->size;
            addr           = (addr + 7) & ~ULONG_PTR{7};
            return reinterpret_cast<const MB2Tag *>(addr);
        }

        /* ******************************************************************************
         *  AUTHOR  : Trollycat                                                         *
         *  FUNC    : IniParseMemoryMap                                                 *
         *  DATE    : 2026                                                              *
         *  PURPOSE : Walk MB2 memory map entries and copy them into BootInfo.          *
         * *****************************************************************************/
        INLINE
        VOID
        IniParseMemoryMap(const MB2MmapTag *tag, BootInfo &info) noexcept
        {
            const ULONG_PTR end = reinterpret_cast<ULONG_PTR>(tag) + tag->size;
            const auto *entry   = tag->entries;

            while (reinterpret_cast<ULONG_PTR>(entry) < end &&
                   info.mmap_count < BootInfo::MAX_MMAP_ENTRIES) {
                auto &out  = info.mmap[info.mmap_count++];
                out.base   = entry->base;
                out.length = entry->length;

                switch (entry->type) {
                case MMAP_AVAILABLE:
                    out.type = MEMORY_TYPE::Available;
                    break;
                case MMAP_ACPI:
                    out.type = MEMORY_TYPE::AcpiReclaimable;
                    break;
                case MMAP_NVS:
                    out.type = MEMORY_TYPE::AcpiNvs;
                    break;
                case MMAP_BADRAM:
                    out.type = MEMORY_TYPE::BadRam;
                    break;
                default:
                    out.type = MEMORY_TYPE::Reserved;
                    break;
                }

                entry = reinterpret_cast<const MB2MmapEntry *>(reinterpret_cast<ULONG_PTR>(entry) +
                                                               tag->entry_size);
            }
        }
    } // namespace

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : InParseMultiboot2                                                 *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Walk all MB2 tags and populate BootInfo with the memory map       *
     * *****************************************************************************/
    VOID
    InParseMultiboot2(ULONG_PTR mb2_phys, BootInfo &info) noexcept
    {
        info.mmap_count     = 0;
        const ULONG_PTR end = mb2_phys + *reinterpret_cast<const DWORD *>(mb2_phys);
        const auto *tag     = reinterpret_cast<const MB2Tag *>(mb2_phys + 8);

        while (reinterpret_cast<ULONG_PTR>(tag) < end && tag->type != TAG_END) {
            switch (tag->type) {
            case TAG_MMAP:
                IniParseMemoryMap(reinterpret_cast<const MB2MmapTag *>(tag), info);
                break;

            case TAG_BOOTLOADER: {
                PCSTR src = reinterpret_cast<PCSTR>(reinterpret_cast<ULONG_PTR>(tag) + 8);
                tklib::strlcpy(info.bootloader_name, src, BootInfo::BOOTLOADER_NAME_MAX);
                break;
            }

            default:
                break;
            }

            tag = IniAdvanceToNextMultiboot2Tag(tag);
        }
    }

} // namespace cbk::boot