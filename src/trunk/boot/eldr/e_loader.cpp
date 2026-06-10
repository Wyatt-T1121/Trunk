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
 *  PURPOSE : ELF64 loader implementation for the boot stage.                   *
 *            Validates ELF magic/class/arch, walks PT_LOAD program headers,    *
 *            copies segments to their physical load addresses, zeroes BSS,     *
 *            and returns the kernel virtual entry point.                       *
 *                                                                              *
 * *****************************************************************************/

#include <trunk/boot/eldr/e_loader.h>
#include <trunk/boot/bu/b_string.h>

namespace trunk::boot
{

    // ELF64 structure definitions

    static constexpr u8 ELFMAG0 = 0x7F;
    static constexpr u8 ELFMAG1 = 'E';
    static constexpr u8 ELFMAG2 = 'L';
    static constexpr u8 ELFMAG3 = 'F';
    static constexpr u8 ELFCLASS64 = 2;  // 64-bit
    static constexpr u16 EM_X86_64 = 62; // x86_64 architecture
    static constexpr u16 ET_EXEC = 2;    // Executable file
    static constexpr u32 PT_LOAD = 1;    // Loadable segment

    struct [[gnu::packed]] Elf64_Ehdr
    {
        u8 e_ident[16];  // Magic, class, data, version, OS/ABI, padding
        u16 e_type;      // Object file type (ET_EXEC = 2)
        u16 e_machine;   // Architecture (EM_X86_64 = 62)
        u32 e_version;   // ELF version
        u64 e_entry;     // Virtual entry point address
        u64 e_phoff;     // Program header table offset (bytes from file start)
        u64 e_shoff;     // Section header table offset (unused here)
        u32 e_flags;     // Processor-specific flags
        u16 e_ehsize;    // ELF header size in bytes
        u16 e_phentsize; // Size of one program header entry
        u16 e_phnum;     // Number of program header entries
        u16 e_shentsize; // Size of one section header entry (unused)
        u16 e_shnum;     // Number of section header entries (unused)
        u16 e_shstrndx;  // Section name string table index (unused)
    };

    struct [[gnu::packed]] Elf64_Phdr
    {
        u32 p_type;   // Segment type (PT_LOAD = 1)
        u32 p_flags;  // Segment flags (R/W/X)
        u64 p_offset; // Offset of segment data within the ELF file
        u64 p_vaddr;  // Virtual address in memory
        u64 p_paddr;  // Physical address — where we copy data to
        u64 p_filesz; // Size of segment in file (may be < p_memsz)
        u64 p_memsz;  // Size of segment in memory (p_memsz - p_filesz = BSS)
        u64 p_align;  // Alignment constraint
    };

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : validate_elf                                                      *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Validate ELF magic, class, architecture, and type.                *
     *            Returns ElfError::None on success, appropriate error otherwise.   *
     *                                                                              *
     * *****************************************************************************/
    static ElfError validate_elf(const Elf64_Ehdr *hdr) noexcept
    {
        // Check ELF magic bytes
        if (hdr->e_ident[0] != ELFMAG0 ||
            hdr->e_ident[1] != ELFMAG1 ||
            hdr->e_ident[2] != ELFMAG2 ||
            hdr->e_ident[3] != ELFMAG3)
            return ElfError::BadMagic;

        // Must be 64-bit ELF
        if (hdr->e_ident[4] != ELFCLASS64)
            return ElfError::BadClass;

        // Must target x86_64
        if (hdr->e_machine != EM_X86_64)
            return ElfError::BadArch;

        // Must be an executable
        if (hdr->e_type != ET_EXEC)
            return ElfError::BadType;

        return ElfError::None;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : elf_load                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Load an ELF64 executable from a physical RAM address.             *
     *            Validates the header, walks PT_LOAD segments, copies file data    *
     *            to p_paddr, zeroes p_memsz - p_filesz (BSS), returns e_entry.     *
     *            The ELF image must already be fully present in physical RAM.      *
     *            GRUB places module data in RAM before jumping to trboot.elf.      *
     *                                                                              *
     * *****************************************************************************/
    ElfResult elf_load(uptr elf_phys_addr) noexcept
    {
        // Interpret the physical address as a pointer to the ELF header
        const auto *hdr = reinterpret_cast<const Elf64_Ehdr *>(elf_phys_addr);

        // Validate before touching anything else
        ElfError err = validate_elf(hdr);
        if (err != ElfError::None)
            return ElfResult{err, 0};

        // Walk program headers
        bool loaded_any = false;

        for (u16 i = 0; i < hdr->e_phnum; ++i)
        {
            // Each program header is at: elf_base + e_phoff + i * e_phentsize
            const auto *phdr = reinterpret_cast<const Elf64_Phdr *>(
                elf_phys_addr + hdr->e_phoff +
                static_cast<u64>(i) * hdr->e_phentsize);

            // Only process loadable segments
            if (phdr->p_type != PT_LOAD)
                continue;

            // Source: segment data inside the ELF image in RAM
            const void *src = reinterpret_cast<const void *>(
                elf_phys_addr + phdr->p_offset);

            // Destination: physical load address from the program header
            void *dst = reinterpret_cast<void *>(phdr->p_paddr);

            // Copy file data (code, rodata, initialised data)
            if (phdr->p_filesz > 0)
                memcpy(dst, src, phdr->p_filesz);

            // Zero BSS region (p_memsz > p_filesz means there is a BSS tail)
            if (phdr->p_memsz > phdr->p_filesz)
            {
                void *bss_start = reinterpret_cast<void *>(
                    phdr->p_paddr + phdr->p_filesz);
                usize bss_size = phdr->p_memsz - phdr->p_filesz;
                memset(bss_start, 0, bss_size);
            }

            loaded_any = true;
        }

        if (!loaded_any)
            return ElfResult{ElfError::NoLoadSeg, 0};

        // Return the virtual entry point — Boot.cpp will jump here
        return ElfResult{ElfError::None, hdr->e_entry};
    }

} // namespace trunk::boot