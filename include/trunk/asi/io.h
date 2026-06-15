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
 *  MODULE  : Assembly Instructions                                              *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Assembly instruction C++ wrappers()                                *
 ********************************************************************************/

#pragma once

#include <types.h>

namespace trunk::asi
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : outb                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a byte to an I/O port.                                       *
     ********************************************************************************/
    inline void outb(u16 port, u8 value) noexcept
    {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : outw                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a word (2 bytes) to an I/O port.                             *
     ********************************************************************************/
    inline void outw(u16 port, u16 value) noexcept
    {
        asm volatile("outw %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : outl                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a dword (4 bytes) to an I/O port.                            *
     ********************************************************************************/
    inline void outl(u16 port, u32 value) noexcept
    {
        asm volatile("outl %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : inb                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a byte from an I/O port.                                      *
     ********************************************************************************/
    [[nodiscard]]
    inline u8 inb(u16 port) noexcept
    {
        u8 value;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : inw                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a word (2 bytes) from an I/O port.                            *
     ********************************************************************************/
    [[nodiscard]]
    inline u16 inw(u16 port) noexcept
    {
        u16 value;
        asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : inl                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a dword (4 bytes) from an I/O port.                           *
     ********************************************************************************/
    [[nodiscard]]
    inline u32 inl(u16 port) noexcept
    {
        u32 value;
        asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : io_wait                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Brief I/O delay by writing to an unused port. Some old hardware    *
     *            needs time between port accesses to process a command.             *
     *            Port 0x80 is the POST code port — safe to write to, ignored.       *
     ********************************************************************************/
    inline void io_wait() noexcept
    {
        outb(0x80, 0x00);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : rdmsr                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a Model Specific Register. Returns the full 64-bit value.     *
     *            Used for EFER, STAR, LSTAR (syscalls), APIC base, etc.             *
     ********************************************************************************/
    [[nodiscard]]
    inline u64 rdmsr(u32 msr) noexcept
    {
        u32 low, high;
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
        return (static_cast<u64>(high) << 32) | low;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : wrmsr                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a Model Specific Register.                                   *
     *            Used for EFER, STAR, LSTAR (syscalls), APIC base, etc.             *
     ********************************************************************************/
    inline void wrmsr(u32 msr, u64 value) noexcept
    {
        u32 low = static_cast<u32>(value);
        u32 high = static_cast<u32>(value >> 32);
        asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : read_cr0                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR0.                                         *
     *            Contains: protected mode, paging, write protect, cache flags.      *
     ********************************************************************************/
    [[nodiscard]]
    inline u64 read_cr0() noexcept
    {
        u64 value;
        asm volatile("mov %%cr0, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : write_cr0                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR0.                                        *
     ********************************************************************************/
    inline void write_cr0(u64 value) noexcept
    {
        asm volatile("mov %0, %%cr0" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : read_cr2                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR2.                                         *
     *            Contains the faulting virtual address on a page fault.             *
     ********************************************************************************/
    [[nodiscard]]
    inline u64 read_cr2() noexcept
    {
        u64 value;
        asm volatile("mov %%cr2, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : read_cr3                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR3.                                         *
     *            Contains the physical address of the PML4 (page table root).       *
     ********************************************************************************/
    [[nodiscard]]
    inline u64 read_cr3() noexcept
    {
        u64 value;
        asm volatile("mov %%cr3, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : write_cr3                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR3. Flushes the entire TLB.                *
     *            Use to switch page tables (process context switch).                *
     ********************************************************************************/
    inline void write_cr3(u64 value) noexcept
    {
        asm volatile("mov %0, %%cr3" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : read_cr4                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR4.                                         *
     *            Contains: PAE, PSE, OSFXSR, OSXSAVE and other feature flags.       *
     ********************************************************************************/
    [[nodiscard]]
    inline u64 read_cr4() noexcept
    {
        u64 value;
        asm volatile("mov %%cr4, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : write_cr4                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR4.                                        *
     ********************************************************************************/
    inline void write_cr4(u64 value) noexcept
    {
        asm volatile("mov %0, %%cr4" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : invlpg                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Invalidate a single TLB entry for the given virtual address.       *
     *            Much cheaper than a full CR3 reload when mapping a single page.    *
     ********************************************************************************/
    inline void invlpg(uptr vaddr) noexcept
    {
        asm volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : hlt                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Halt the CPU until the next interrupt.                             *
     ********************************************************************************/
    inline void hlt() noexcept
    {
        asm volatile("hlt");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : cli                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Disable hardware interrupts.                                       *
     ********************************************************************************/
    inline void cli() noexcept
    {
        asm volatile("cli" : : : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : sti                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Enable hardware interrupts.                                        *
     ********************************************************************************/
    inline void sti() noexcept
    {
        asm volatile("sti" : : : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pause                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Hint to the CPU that this is a spin-wait loop.                     *
     *            Improves performance and power consumption in spinlocks.           *
     ********************************************************************************/
    inline void pause() noexcept
    {
        asm volatile("pause");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : cpuid                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Execute CPUID instruction. Returns eax/ebx/ecx/edx via             *
     *            output parameters. Used for CPU feature detection.                 *
     ********************************************************************************/
    inline void cpuid(u32 leaf, u32 &eax, u32 &ebx, u32 &ecx, u32 &edx) noexcept
    {
        asm volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(leaf), "c"(0));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : rdtsc                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read the Time Stamp Counter. Returns CPU cycles since reset.       *
     *            Useful for basic timing and entropy.                               *
     ********************************************************************************/
    [[nodiscard]]
    inline u64 rdtsc() noexcept
    {
        u32 low, high;
        asm volatile("rdtsc" : "=a"(low), "=d"(high));
        return (static_cast<u64>(high) << 32) | low;
    }

} // namespace trunk::asi