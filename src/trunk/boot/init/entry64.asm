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
; *  PURPOSE : 64-bit landing pad. Arrived at via far jump from Entry32.asm.    *
; *            Responsibilities (in order):                                     *
; *              1. Load 64-bit data segments                                   *
; *              2. Switch RSP to the real kernel stack (higher-half)           *
; *              3. Zero the BSS segment                                        *
; *              4. Run C++ global constructors (.init_array)                   *
; *              5. Call boot_entry(mb2_magic, mb2_phys) in Boot.cpp            *
; *            edi/esi still hold MB2 magic and info ptr from Entry32.          *
; *                                                                             *
; *******************************************************************************

bits 64

extern boot_entry           ; Boot.cpp  — C linkage
extern __bss_start          ; linker script symbol
extern __bss_end            ; linker script symbol
extern __boot_stack_top     ; linker script symbol
extern __init_array_start   ; linker script symbol
extern __init_array_end     ; linker script symbol

extern mb2_magic_store      ; Entry32.asm — MB2 magic value
extern mb2_info_store       ; Entry32.asm — MB2 info pointer

global entry64

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : Entry64                                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : First 64-bit code to execute. Sets up the environment the C++    *
; *            runtime expects, then calls boot_entry. Must not return.         *
; *******************************************************************************

entry64:
    ; ── LOAD MB2 VALUES FROM BOOT MEMORY ───────────────────────────────────
    ; These were stored in Entry32.asm before the mode switch
    ; They're in the .boot.text section at physical addresses
    mov r12d, dword [mb2_magic_store]    ; load MB2 magic from memory
    mov r13d, dword [mb2_info_store]     ; load MB2 info ptr from memory

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
    ; The linker script defines __boot_stack_top at the top of the .stack section.
    mov rsp, __boot_stack_top
    xor rbp, rbp                    ; mark end of stack frames for debuggers

    ; ── 3. Zero BSS ───────────────────────────────────────────────────────────
    ; The C++ runtime requires BSS to be zeroed before any static constructors
    ; or kernel code runs. The bootloader does NOT do this for us.

    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    xor al,  al
    rep stosb

    ; ── 4. Call C++ global constructors ──────────────────────────────────────
    ; The linker collects constructor function pointers into .init_array.
    ; We call each one in order. Required for any static C++ objects.

    mov rbx, __init_array_start
    
    ; ***************************************************************************
    ; *  AUTHOR  : Trollycat                                                    *
    ; *  FUNC    : ctor_loop (inline)                                           *
    ; *  DATE    : 2026                                                         *
    ; *  PURPOSE : Iterates __init_array_start .. __init_array_end, calling     *
    ; *            each 8-byte function pointer in sequence.                    *
    ; ***************************************************************************
.ctor_loop:
    cmp rbx, __init_array_end
    jge .ctor_done
    call qword [rbx]
    add rbx, 8
    jmp .ctor_loop
.ctor_done:

    ; ── 5. Call boot_entry(mb2_magic, mb2_phys) ──────────────────────────────
    ; System V AMD64 ABI: first arg = rdi, second arg = rsi.
    ; Restore MB2 values from r12d/r13d (callee-saved across BSS zero and ctors)
    mov edi, r12d                   ; restore MB2 magic to rdi (32-bit load zero-extends)
    mov esi, r13d                   ; restore MB2 phys ptr to rsi (32-bit load zero-extends)
    call boot_entry                 ; [[noreturn]] — should never come back

    ; ── Safety net ────────────────────────────────────────────────────────────
.hang:
    cli
    hlt
    jmp .hang