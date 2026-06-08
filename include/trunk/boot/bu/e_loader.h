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
 *  FILE    : ELoader.h                                                         *
 *  DATE    : 2026                                                              *
 *  PURPOSE : ELF64 loader for the boot stage.                                  *
 *            Parses troskern.elf from its physical RAM location, copies each   *
 *            PT_LOAD segment to its physical load address, zeroes BSS regions, *
 *            and returns the virtual entry point for the kernel jump.          *
 *            No tklib dependency — boot stage is fully self-contained.         *
 *                                                                              *
 * *****************************************************************************/

#pragma once

#include <types.h>

namespace trunk::boot
{

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  SECTION : ELF64 result type                                                 *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Returned by elf_load(). On success contains the virtual entry     *
     *            point address to jump to. On failure contains an error code.      *
     *                                                                              *
     * *****************************************************************************/
    enum class ElfError : u32
    {
        None = 0,
        BadMagic = 1,  // Not an ELF file
        BadClass = 2,  // Not ELF64
        BadArch = 3,   // Not x86_64
        BadType = 4,   // Not ET_EXEC
        NoLoadSeg = 5, // No PT_LOAD segments found
    };

    struct ElfResult
    {
        ElfError error;
        u64 entry; // Virtual entry point (e_entry from ELF header)

        [[nodiscard]] constexpr bool ok() const noexcept
        {
            return error == ElfError::None;
        }
    };

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : elf_load                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Load an ELF64 executable from a physical RAM address.             *
     *            Walks PT_LOAD segments, copies file data to p_paddr,              *
     *            zeroes p_memsz - p_filesz (BSS), returns e_entry.                 *
     *            Returns ElfResult with error set if validation fails.             *
     *                                                                              *
     * *****************************************************************************/
    [[nodiscard]]
    ElfResult elf_load(uptr elf_phys_addr) noexcept;

} // namespace trunk::boot