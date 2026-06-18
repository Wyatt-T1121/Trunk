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
; *  AUTHOR  : Trollycat                                                        *
; *  MODULE  : Bootstrapping                                                    *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Saves MB2 magic + info ptr,                                      *
; *                  loads a minimal 64-bit GDT, jumps to entry64.              *
; *******************************************************************************

bits 32
section .boot.text

extern setup_page_tables
extern enable_long_mode

global _start

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : save_mb2_to_memory                                               *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Stores MB2 values to memory in boot section so they can survive  *
; *******************************************************************************
save_mb2_to_memory:
    mov dword [mb2_magic_store], eax
    mov dword [mb2_info_store], ebx
    
    mov edi, eax
    mov esi, ebx
    
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : _start                                                           *
; *  DATE    : 2026                                                             *
; *  PURPOSE : GRUB entry. The 32-64 bit switch.                                *
; *******************************************************************************
_start:
    cli
    
    call save_mb2_to_memory

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
; *******************************************************************************
bits 64
trampoline64:
    mov rax, [entry64_vaddr]
    jmp rax
    jmp tr_early_fault_lockdown

align 16
gdt64:
    dq 0
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    dq (1 << 41) | (1 << 44) | (1 << 47)
gdt64_ptr:
    dw gdt64_ptr - gdt64 - 1
    dd gdt64
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

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : tr_early_fault_lockdown                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : halt loop incase any assembly code fails                         *
; *******************************************************************************
global tr_early_fault_lockdown
tr_early_fault_lockdown:
    cli
.lock_loop:
    hlt
    pause
    jmp .lock_loop