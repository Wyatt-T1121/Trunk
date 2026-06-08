; *******************************************************************************
; *                                                                             *
; *  Copyright 2026 Trollycat                                                   *
; *                                                                             *
; *  Licensed under the Apache License, Version 2.0 (the "License");            *
; *  you may not use this file except in compliance with the License.           *
; *  You may obtain a copy of the License at                                    *
; *                                                                             *
; *      http://www.apache.org/licenses/LICENSE-2.0                             *
; *                                                                             *
; *  Unless required by applicable law or agreed to in writing, software        *
; *  distributed under the License is distributed on an "AS IS" BASIS,          *
; *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
; *  See the License for the specific language governing permissions and        *
; *  limitations under the License.                                             *
; *                                                                             *
; *******************************************************************************
; *                                                                             *
; *  AUTHOR  : Trollycat                                                        *
; *  MODULE  : Bootstrapping                                                    *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Multiboot2 magic header blob. Must be within the first 32KB      *
; *            of the final binary. Isolated here so the linker script can      *
; *            guarantee placement with KEEP(*(.multiboot2)) and nothing        *
; *            else can accidentally push it out of range.                      *
; *                                                                             *
; *            Tags present:                                                    *
; *              - Framebuffer  (type 5, optional) — request text mode          *
; *              - Module align (type 6, optional) — 4KB-align module loads     *
; *              - End          (type 0, required) — terminates tag list        *
; *                                                                             *
; *            GRUB is told to load troskern.elf as a module via grub.cfg       *
; *            (multiboot2 module /boot/troskern.elf). The module align tag     *
; *            ensures GRUB places it on a 4KB boundary so elf_load() can       *
; *            safely walk its program headers without alignment faults.        *
; *                                                                             *
; *******************************************************************************

bits 32

; Multiboot2 constants

MB2_MAGIC       equ 0xE85250D6
MB2_ARCH        equ 0                           ; i386 protected mode
HEADER_LEN      equ (mb2_end - mb2_start)
MB2_CHECKSUM    equ -(MB2_MAGIC + MB2_ARCH + HEADER_LEN)

; Header

section .multiboot2
align 8

mb2_start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd HEADER_LEN
    dd MB2_CHECKSUM

    ; Tag: framebuffer (type 5, optional)
    ; Request text mode. flags=1 means optional — GRUB will not fail if the
    ; firmware cannot satisfy it. width/height/depth = 0 = no preference.
    align 8
    dw 5            ; type   = framebuffer
    dw 1            ; flags  = optional
    dd 20           ; size   = 20 bytes
    dd 0            ; width  — no preference
    dd 0            ; height — no preference
    dd 0            ; depth  — no preference

    ; Tag: module alignment (type 6, optional)
    ; Instructs GRUB to load all modules on page-aligned (4KB) boundaries.
    ; This is required for elf_load() — it copies PT_LOAD segments via p_paddr
    ; and walks headers with pointer arithmetic that assumes natural alignment.
    ; Without this, GRUB may place the module at an arbitrary address.
    align 8
    dw 6            ; type  = module alignment
    dw 0            ; flags = required (0 = must honour)
    dd 8            ; size  = 8 bytes (no payload beyond the fixed fields)

    ; Tag: end (type 0, required)
    align 8
    dw 0            ; type  = end
    dw 0            ; flags
    dd 8            ; size
mb2_end: