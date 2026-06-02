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
 *  FILE    : trunk.ld                                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Higher-half kernel linker script.                                  *
 *                                                                               *
 *            Boot region  (physical addresses, 32-bit safe):                    *
 *              .multiboot2   — MB2 Header blob                                  *
 *              .boot.text    — Entry32, Paging, 64-bit trampoline, GDT          *
 *              .boot.rodata  — any read-only boot data                          *
 *                                                                               *
 *            Kernel region (higher-half virtual, AT() sets physical LMA):       *
 *              .text / .rodata / .data / .bss / .stack                          *
 *                                                                               *
 ********************************************************************************/

OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)
ENTRY(_start)

KERNEL_VMA  = 0xFFFFFFFF80000000;
KERNEL_LMA  = 0x0000000000100000;

SECTIONS
{
    /* =========================================================================
       BOOT REGION — linked at physical address, safe for 32-bit code
       ========================================================================= */

    . = KERNEL_LMA;

    .multiboot2 : {
        KEEP(*(.multiboot2))
    }

    .boot.text : {
        *(.boot.text)
        *(.boot.text.*)
    }

    .boot.rodata : {
        *(.boot.rodata)
        *(.boot.rodata.*)
    }

    . = ALIGN(0x1000);
    BOOT_END_LMA = .;

    /* =========================================================================
       KERNEL REGION — higher-half virtual, loaded at physical via AT()
       ========================================================================= */

    . = KERNEL_VMA + BOOT_END_LMA;

    .text ALIGN(0x1000) : AT(ADDR(.text) - KERNEL_VMA) {
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - KERNEL_VMA) {
        *(.rodata)
        *(.rodata.*)
    }

    .init_array ALIGN(8) : AT(ADDR(.init_array) - KERNEL_VMA) {
        __init_array_start = .;
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        __init_array_end = .;
    }

    .fini_array ALIGN(8) : AT(ADDR(.fini_array) - KERNEL_VMA) {
        __fini_array_start = .;
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        __fini_array_end = .;
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - KERNEL_VMA) {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(0x1000) : AT(ADDR(.bss) - KERNEL_VMA) {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    }

    .stack ALIGN(0x1000) : AT(ADDR(.stack) - KERNEL_VMA) {
        __stack_bottom = .;
        . += 0x4000;
        __stack_top = .;
    }

    __kernel_phys_start = KERNEL_LMA;
    __kernel_phys_end   = LOADADDR(.bss) + SIZEOF(.bss);

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
        *(.note.GNU-stack)
    }
}