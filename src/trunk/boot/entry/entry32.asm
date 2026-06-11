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
; *  PURPOSE : 32-bit protected mode entry point (_start). GRUB jumps here.     *
; *            Saves MB2 magic + info ptr, builds page tables, enables long     *
; *            mode, loads a minimal 64-bit GDT, jumps to entry64.              *
; *                                                                             *
; *******************************************************************************

bits 32
section .boot.text

extern setup_page_tables
extern enable_long_mode

global _start

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : _save_mb2_to_memory                                              *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Stores MB2 values to memory in boot section so they can survive  *
; *******************************************************************************
_save_mb2_to_memory:
    mov dword [mb2_magic_store], eax
    mov dword [mb2_info_store], ebx
    
    mov edi, eax
    mov esi, ebx
    
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : _start                                                           *
; *  DATE    : 2026                                                             *
; *  PURPOSE : GRUB entry. Orchestrates the 32→64 bit switch.                   *
; *******************************************************************************
_start:
    cli
    
    call _save_mb2_to_memory

    mov esp, 0x7C00
    
    call setup_page_tables
    call enable_long_mode

    lgdt [gdt64_ptr]

    jmp 0x08:trampoline64

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : trampoline64                                                     *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Tiny 64-bit stub that lives in the physical boot region.         *
; *            Loads the higher-half address of entry64 into rax and jumps.     *
; *            Needed because a direct far-jmp to a 64-bit address is not       *
; *            encodeable from 32-bit mode.                                     *
; *******************************************************************************
bits 64
trampoline64:
    mov rax, [entry64_vaddr]  ; 64-bit immediate — no truncation issue
    jmp rax

.hang:
    cli
    hlt
    jmp .hang


align 16
gdt64:
    dq 0                                                    ; null
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)      ; 64-bit code ring 0
    dq (1 << 41) | (1 << 44) | (1 << 47)                   ; 64-bit data ring 0
gdt64_ptr:
    dw gdt64_ptr - gdt64 - 1   ; limit
    dd gdt64                    ; base (32-bit physical — assembler resolves this,
                                ;       no linker relocation involved)

extern entry64
align 8
entry64_vaddr:
    dq entry64

align 8
global mb2_magic_store
global mb2_info_store

mb2_magic_store:
    dd 0

mb2_info_store:
    dd 0