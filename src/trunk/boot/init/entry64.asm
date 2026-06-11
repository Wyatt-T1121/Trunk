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
; *  PURPOSE : 64-bit landing pad. Arrived at via far jump from entry32.asm.    *
; *            Responsibilities in order:                                       *
; *              1. Load 64-bit data segments                                   *
; *              2. Switch RSP to the kernel stack defined in trunk.ld          *
; *              3. Zero the BSS segment                                        *
; *              4. Run C++ global constructors (.init_array)                   *
; *              5. Call TrSystemStartup(mb2_magic, mb2_phys) in kalp_entry.asm *
; *            MB2 magic and info ptr are restored from boot memory storage     *
; *            written by entry32.asm before the mode switch.                   *
; *                                                                             *
; *******************************************************************************

bits 64

extern TrSystemStartup      ; kalp_entry.asm, C linkage
extern __bss_start          ; linker script symbol
extern __bss_end            ; linker script symbol
extern __stack_top          ; linker script symbol
extern __init_array_start   ; linker script symbol
extern __init_array_end     ; linker script symbol

extern mb2_magic_store      ; entry32.asm, MB2 magic value
extern mb2_info_store       ; entry32.asm, MB2 info pointer

global entry64

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : entry64                                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : First 64-bit code to execute. Sets up the environment the C++    *
; *            runtime expects, then calls boot_entry. Must not return.         *
; *******************************************************************************

entry64:
    ; Load MB2 values saved by entry32.asm before the mode switch.
    ; They live in .boot.text at physical addresses.
    mov r12d, dword [mb2_magic_store]
    mov r13d, dword [mb2_info_store]

    ; 1. Load 64-bit data segments.
    ; The far jump updated CS. Update the rest to match.
    ; GDT data descriptor offset = 16 (null, code, data).
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; 2. Switch to the kernel stack.
    ; __stack_top is defined in trunk.ld at the top of the .stack section.
    mov rsp, __stack_top
    xor rbp, rbp

    ; 3. Zero BSS.
    ; The C++ runtime requires BSS zeroed before constructors or kernel code.
    ; GRUB does not do this for us.
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    xor al,  al
    rep stosb

    ; 4. Call C++ global constructors.
    ; The linker collects constructor pointers into .init_array.
    ; Each entry is an 8-byte function pointer called in order.
    mov rbx, __init_array_start

    ; ***************************************************************************
    ; *  AUTHOR  : Trollycat                                                    *
    ; *  FUNC    : ctor_loop (inline)                                           *
    ; *  DATE    : 2026                                                         *
    ; *  PURPOSE : Iterates __init_array_start to __init_array_end, calling     *
    ; *            each 8-byte function pointer in sequence.                    *
    ; ***************************************************************************
.ctor_loop:
    cmp rbx, __init_array_end
    jge .ctor_done
    call qword [rbx]
    add rbx, 8
    jmp .ctor_loop
.ctor_done:

    ; 5. Call TrSystemStartup(mb2_magic, mb2_phys).
    ; System V AMD64 ABI: first arg = rdi, second arg = rsi.
    ; r12d/r13d are callee-saved so they survived BSS zero and ctors.
    mov edi, r12d
    mov esi, r13d
    call TrSystemStartup

.hang:
    cli
    hlt
    jmp .hang