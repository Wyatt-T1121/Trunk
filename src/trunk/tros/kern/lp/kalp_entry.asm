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
; *  PURPOSE : System startup landing pad. Called from entry64.asm with         *
; *            mb2_magic in edi and mb2_phys in esi. Responsibilities in        *
; *            order:                                                           *
; *              1. Harden CPU state: cli + cld                                 *
; *              2. Load 64-bit data segments                                   *
; *              3. Establish a 16-byte aligned stack per the System V          *
; *                 AMD64 ABI, required before any C++ call                     *
; *              4. Zero RBP to mark the end of the stack frame chain           *
; *              5. Call Trkload(mb2_magic, mb2_phys), never returns            *
; *                                                                             *
; *            EDI/ESI are untouched by steps 1 to 4 and pass through to        *
; *            Trkload unchanged, satisfying the System V AMD64 ABI.            *
; *                                                                             *
; *******************************************************************************

bits 64

extern Trkload          ; boot.cpp, extern "C"
extern __stack_top      ; linker script symbol, top of .stack section

global TrSystemStartup

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : TrHardenCPUState                                                 *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Hardens CPU state                                                *
; *******************************************************************************
TrHardenCPUState:
    cli
    cld
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : TrLoad64BitDataSegments                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads 64-bit data segments                                       *
; *******************************************************************************
TrLoad64BitDataSegments:
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : TrSetupKernelStack                                               *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Sets up the kernel stack                                         *
; *******************************************************************************
TrSetupKernelStack:
    mov rsp, __stack_top
    sub rsp, 8
    and rsp, ~0xF
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : TrSystemStartup                                                  *
; *  DATE    : 2026                                                             *
; *  PURPOSE : System startup landing pad                                       *
; *******************************************************************************
TrSystemStartup:
    call TrHardenCPUState
    call TrLoad64BitDataSegments

    mov gs, ax
    mov ss, ax

    call TrSetupKernelStack

    xor rbp, rbp

    call Trkload

.hang:
    cli
    hlt
    jmp .hang