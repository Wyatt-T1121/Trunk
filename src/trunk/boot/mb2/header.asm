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
; *******************************************************************************

bits 32

MB2_MAGIC       equ 0xE85250D6
MB2_ARCH        equ 0
HEADER_LEN      equ (mb2_end - mb2_start)
MB2_CHECKSUM    equ -(MB2_MAGIC + MB2_ARCH + HEADER_LEN)

section .multiboot2
align 8


; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : mb2_start                                                        *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Multiboot2 header start                                          *
; *******************************************************************************
mb2_start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd HEADER_LEN
    dd MB2_CHECKSUM

    align 8
    dw 5            ; type   = framebuffer
    dw 1            ; flags  = optional
    dd 20           ; size   = 20 bytes
    dd 0            ; width  — no preference
    dd 0            ; height — no preference
    dd 0            ; depth  — no preference

    align 8
    dw 0            ; type  = end
    dw 0            ; flags
    dd 8            ; size
mb2_end: