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
 *  MODULE  : Assembly Instructions                                              *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Assembly instruction C++ wrappers()                                *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

namespace trunk::hal
{

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : OutB                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a byte to an I/O port.                                       *
     ********************************************************************************/
    inline VOID OutB(WORD port, BYTE value) noexcept
    {
        asm volatile("outb %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : OutW                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a word (2 bytes) to an I/O port.                             *
     ********************************************************************************/
    inline VOID OutW(WORD port, WORD value) noexcept
    {
        asm volatile("outw %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : OutL                                                               *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a dword (4 bytes) to an I/O port.                            *
     ********************************************************************************/
    inline VOID OutL(WORD port, DWORD value) noexcept
    {
        asm volatile("outl %0, %1" : : "a"(value), "Nd"(port) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InB                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a byte from an I/O port.                                      *
     ********************************************************************************/
    NO_DISCARD
    inline BYTE InB(WORD port) noexcept
    {
        BYTE value;
        asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InW                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a word (2 bytes) from an I/O port.                            *
     ********************************************************************************/
    NO_DISCARD
    inline WORD InW(WORD port) noexcept
    {
        WORD value;
        asm volatile("inw %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InL                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a dword (4 bytes) from an I/O port.                           *
     ********************************************************************************/
    NO_DISCARD
    inline DWORD InL(WORD port) noexcept
    {
        DWORD value;
        asm volatile("inl %1, %0" : "=a"(value) : "Nd"(port) : "memory");
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IoWait                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Brief I/O delay by writing to an unused port.                      *
     ********************************************************************************/
    inline VOID IoWait() noexcept
    {
        OutB(0x80, 0x00);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RdMsr                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read a Model Specific Register.                                    *
     ********************************************************************************/
    NO_DISCARD
    inline QWORD RdMsr(DWORD msr) noexcept
    {
        DWORD low, high;
        asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
        return (static_cast<QWORD>(high) << 32) | low;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : WrMsr                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write a Model Specific Register.                                   *
     ********************************************************************************/
    inline VOID WrMsr(DWORD msr, QWORD value) noexcept
    {
        DWORD low  = static_cast<DWORD>(value);
        DWORD high = static_cast<DWORD>(value >> 32);
        asm volatile("wrmsr" : : "c"(msr), "a"(low), "d"(high));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ReadCr0                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR0.                                         *
     ********************************************************************************/
    NO_DISCARD
    inline QWORD ReadCr0() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr0, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : WriteCr0                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR0.                                        *
     ********************************************************************************/
    inline VOID WriteCr0(QWORD value) noexcept
    {
        asm volatile("mov %0, %%cr0" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ReadCr2                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR2.                                         *
     ********************************************************************************/
    NO_DISCARD
    inline QWORD ReadCr2() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr2, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ReadCr3                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR3.                                         *
     ********************************************************************************/
    NO_DISCARD
    inline QWORD ReadCr3() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr3, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : WriteCr3                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR3.                                        *
     ********************************************************************************/
    inline VOID WriteCr3(QWORD value) noexcept
    {
        asm volatile("mov %0, %%cr3" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ReadCr4                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read control register CR4.                                         *
     ********************************************************************************/
    NO_DISCARD
    inline QWORD ReadCr4() noexcept
    {
        QWORD value;
        asm volatile("mov %%cr4, %0" : "=r"(value));
        return value;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : WriteCr4                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Write control register CR4.                                        *
     ********************************************************************************/
    inline VOID WriteCr4(QWORD value) noexcept
    {
        asm volatile("mov %0, %%cr4" : : "r"(value) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InvLpg                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Invalidate a single TLB entry for the given virtual address.       *
     ********************************************************************************/
    inline VOID InvLpg(ULONG_PTR vaddr) noexcept
    {
        asm volatile("invlpg (%0)" : : "r"(vaddr) : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : Hlt                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Halt the CPU until the next interrupt.                             *
     ********************************************************************************/
    inline VOID Hlt() noexcept
    {
        asm volatile("hlt");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : Cli                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Disable hardware interrupts.                                       *
     ********************************************************************************/
    inline VOID Cli() noexcept
    {
        asm volatile("cli" : : : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : Sti                                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Enable hardware interrupts.                                        *
     ********************************************************************************/
    inline VOID Sti() noexcept
    {
        asm volatile("sti" : : : "memory");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : Pause                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Hint to the CPU that this is a spin-wait loop.                     *
     ********************************************************************************/
    inline VOID Pause() noexcept
    {
        asm volatile("pause");
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : Cpuid                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Execute Cpuid instruction.                                         *
     ********************************************************************************/
    inline VOID Cpuid(DWORD leaf, DWORD &eax, DWORD &ebx, DWORD &ecx, DWORD &edx) noexcept
    {
        asm volatile("cpuid" : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx) : "a"(leaf), "c"(0));
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RdtSc                                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Read the Time Stamp Counter.                                       *
     ********************************************************************************/
    NO_DISCARD
    inline QWORD RdtSc() noexcept
    {
        DWORD low, high;
        asm volatile("rdtsc" : "=a"(low), "=d"(high));
        return (static_cast<QWORD>(high) << 32) | low;
    }

} // namespace trunk::hal