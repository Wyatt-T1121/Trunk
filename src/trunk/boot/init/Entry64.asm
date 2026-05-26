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
; *  FILE    : src/trunk/arch/x86_64/boot/Entry64.asm                           *
; *  DATE    : 2026                                                             *
; *  PURPOSE : 64-bit landing pad. Arrived at via far jump from Entry32.asm.    *
; *            Responsibilities (in order):                                     *
; *              1. Load 64-bit data segments                                   *
; *              2. Switch RSP to the real kernel stack (higher-half)           *
; *              3. Zero the BSS segment                                        *
; *              4. Run C++ global constructors (.init_array)                   *
; *              5. Call boot_entry(mb2_magic, mb2_phys) in boot.cpp            *
; *            edi/esi still hold MB2 magic and info ptr from Entry32.          *
; *                                                                             *
; *******************************************************************************

bits 64

extern boot_entry           ; boot.cpp  — C linkage
extern __bss_start          ; linker script symbol
extern __bss_end            ; linker script symbol
extern __stack_top          ; linker script symbol
extern __init_array_start   ; linker script symbol
extern __init_array_end     ; linker script symbol

global Entry64

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : Entry64                                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : First 64-bit code to execute. Sets up the environment the C++    *
; *            runtime expects, then calls boot_entry. Must not return.         *
; *******************************************************************************

Entry64:

    ; ── 1. Load 64-bit data segment into all data registers ──────────────────
    ; The far jump updated CS; now update the rest.
    ; gdt64.data offset = 16 (second descriptor after null+code)
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; ── 2. Switch to the kernel stack (higher-half virtual address) ───────────
    ; The linker script defines __stack_top at the top of the .stack section.
    mov rsp, __stack_top
    xor rbp, rbp                    ; mark end of stack frames for debuggers

    ; ── 3. Zero BSS ───────────────────────────────────────────────────────────
    ; The C++ runtime requires BSS to be zeroed before any static constructors
    ; or kernel code runs. The bootloader does NOT do this for us.

    ; ***************************************************************************
    ; *  AUTHOR  : Trollycat                                                    *
    ; *  FUNC    : bss_zero (inline)                                            *
    ; *  DATE    : 2026                                                         *
    ; *  PURPOSE : Writes zero bytes from __bss_start to __bss_end using        *
    ; *            rep stosb. Must run before any C/C++ code.                   *
    ; ***************************************************************************
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    xor al,  al
    rep stosb

    ; ── 4. Call C++ global constructors ──────────────────────────────────────
    ; The linker collects constructor function pointers into .init_array.
    ; We call each one in order. Required for any static C++ objects.

    ; ***************************************************************************
    ; *  AUTHOR  : Trollycat                                                    *
    ; *  FUNC    : ctor_loop (inline)                                           *
    ; *  DATE    : 2026                                                         *
    ; *  PURPOSE : Iterates __init_array_start .. __init_array_end, calling     *
    ; *            each 8-byte function pointer in sequence.                    *
    ; ***************************************************************************
    mov rbx, __init_array_start
.ctor_loop:
    cmp rbx, __init_array_end
    jge .ctor_done
    call qword [rbx]
    add rbx, 8
    jmp .ctor_loop
.ctor_done:

    ; ── 5. Call boot_entry(mb2_magic, mb2_phys) ──────────────────────────────
    ; System V AMD64 ABI: first arg = rdi, second arg = rsi.
    ; edi and esi were set by Entry32.asm and survived the mode switch.
    ; Zero-extend to full 64-bit registers to be clean.
    mov edi, edi                    ; zero-extend edi -> rdi  (MB2 magic)
    mov esi, esi                    ; zero-extend esi -> rsi  (MB2 phys ptr)
    call boot_entry                 ; [[noreturn]] — should never come back

    ; ── Safety net ────────────────────────────────────────────────────────────
.hang:
    cli
    hlt
    jmp .hang