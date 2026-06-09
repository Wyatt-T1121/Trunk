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
; *  MODULE  : Kernel Landing Pad                                               *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Kernel assembly entry point. ENTRY(_start_kern) in trunk.ld.     *
; *            points here. Responsibilities in order:                          *
; *              1. Harden CPU state: cli + cld                                 *
; *              2. Load 64-bit data segments                                   *
; *              3. Establish a 16-byte aligned stack per the System V          *
; *                 AMD64 ABI, required before any C++ call                     *
; *              4. Zero RBP to mark the end of the stack frame chain           *
; *              5. Call kmain, never returns                                   *
; *                                                                             *
; *            RDI still holds the BootInfo pointer passed by boot_entry        *
; *            via the KernelEntry function pointer cast. We preserve it        *
; *            untouched so kmain receives it as its first argument.            *
; *                                                                             *
; *******************************************************************************

bits 64

extern kmain            ; trunk::kernel::kmain, extern "C" in kernel.cpp
extern __stack_top      ; linker script symbol, top of .stack section

global _start_kern

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : _start_kern                                                      *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Kernel landing pad. Hardens CPU state, aligns the stack to       *
; *            16 bytes per the System V AMD64 ABI, then calls kmain.           *
; *            Must not return.                                                 *
; *******************************************************************************

_start_kern:

    ; Harden CPU state immediately on entry.
    ; cli: disable interrupts — we cannot trust the bootloader left them off.
    ; cld: clear direction flag — string ops must increment, not decrement.
    cli
    cld

    ; Load 64-bit data segments.
    ; Selector offset 16 = third GDT entry (null, code, data).
    ; CS was already set by the far jump in boot_entry.
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Establish the kernel stack.
    ; Point RSP at __stack_top, subtract 8 to create a safe buffer before
    ; masking, then AND to 16-byte boundary per the System V AMD64 ABI.
    mov rsp, __stack_top
    sub rsp, 8
    and rsp, ~0xF

    ; Zero RBP so stack unwinders see a clean frame chain termination.
    xor rbp, rbp

    ; Call kmain. RDI is untouched and still holds the BootInfo pointer
    ; passed by boot_entry, satisfying the first argument slot per the ABI.
    call kmain

    ; kmain is [[noreturn]]. Halt permanently if it ever returns.
.hang:
    cli
    hlt
    jmp .hang