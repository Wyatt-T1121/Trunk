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
; *  FILE    : Entry32.asm                                                      *
; *  DATE    : 2026                                                             *
; *  PURPOSE : 32-bit protected mode entry point (_start). GRUB jumps here.     *
; *            Saves MB2 magic + info ptr, builds page tables, enables long     *
; *            mode, loads a minimal 64-bit GDT, jumps to Entry64.              *
; *                                                                             *
; *            The GDT pointer and far-jump target must be 32-bit physical      *
; *            addresses. We avoid asking the linker to relocate them at all    *
; *            by encoding both inline as immediate constants.                  *
; *                                                                             *
; *******************************************************************************

bits 32
section .boot.text

extern setup_page_tables
extern enable_long_mode

global _start

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : _start                                                           *
; *  DATE    : 2026                                                             *
; *  PURPOSE : GRUB entry. Orchestrates the 32→64 bit switch.                   *
; *******************************************************************************
_start:
    cli
    
    ; Store MB2 values to memory in boot section so they survive mode switch
    mov dword [mb2_magic_store], eax      ; save MB2 magic
    mov dword [mb2_info_store], ebx       ; save MB2 info ptr
    
    mov edi, eax            ; also keep in registers for now
    mov esi, ebx
    mov esp, 0x7C00         ; temp stack

    call setup_page_tables
    call enable_long_mode

    ; Load our minimal 64-bit GDT.
    ; The GDT is defined right below in this same section, so its address
    ; is a physical address that fits in 32 bits — no linker relocation needed.
    lgdt [gdt64_ptr]

    ; Far jump to flush CS and enter 64-bit mode.
    ; We encode the target as an absolute 64-bit value loaded into rax,
    ; then do an indirect far jump — this way the linker never needs to
    ; encode a 32-bit relocation against the higher-half Entry64 symbol.
    ; After the far jump the CPU is in full 64-bit mode.
    jmp 0x08:trampoline64

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : trampoline64                                                     *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Tiny 64-bit stub that lives in the physical boot region.         *
; *            Loads the higher-half address of Entry64 into rax and jumps.     *
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

; ── GDT (physical, inline) ────────────────────────────────────────────────────
; Defined in the same section so addresses are physical.
; gdt64_ptr.base is a 4-byte field — filled with the physical address of gdt64.

align 16
gdt64:
    dq 0                                                    ; null
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)      ; 64-bit code ring 0
    dq (1 << 41) | (1 << 44) | (1 << 47)                   ; 64-bit data ring 0
gdt64_ptr:
    dw gdt64_ptr - gdt64 - 1   ; limit
    dd gdt64                    ; base (32-bit physical — assembler resolves this,
                                ;       no linker relocation involved)

; ── Higher-half address of Entry64 as a 64-bit constant ──────────────────────
; We store it as a 64-bit immediate in .boot.text so the linker emits a
; full 64-bit relocation (R_X86_64_64) which is fine, rather than a
; truncated 32-bit one.
extern entry64
align 8
entry64_vaddr:
    dq entry64

; ── MB2 Values Storage ───────────────────────────────────────────────────────
; Stored in boot section to survive the mode switch
align 8
global mb2_magic_store
global mb2_info_store

mb2_magic_store:
    dd 0

mb2_info_store:
    dd 0