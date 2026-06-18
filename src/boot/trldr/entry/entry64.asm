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
; *  PURPOSE : 64-bit landing pad. Arrived from far jump in entry32.asm         *
; *******************************************************************************

bits 64

extern CbkSystemStartup

extern __bss_start
extern __bss_end

extern __stack_top

extern __init_array_start
extern __init_array_end

extern Mb2MagicStore
extern Mb2InfoStore

extern CbkEarlyFaultLockdown

global Entry64

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : LoadMb2FromMemory                                                *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads MB2 values saved by entry32.asm before the mode switch.    *
; *******************************************************************************
LoadMb2FromMemory:
    mov r12d, dword [Mb2MagicStore]
    mov r13d, dword [Mb2InfoStore]
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : Load64bDataSegments                                              *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads 64-bit data segments                                       *
; *******************************************************************************
Load64bDataSegments:
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : ZeroBss                                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Zeros the BSS section                                            *
; *******************************************************************************
ZeroBss:
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    xor al,  al
    rep stosb
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : Entry64                                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : 64-bit entry level code, sets up the stack, calls C++ global con *
; *******************************************************************************
Entry64:
    cli
    cld
    
    call LoadMb2FromMemory
    call Load64bDataSegments

    mov rsp, __stack_top
    and rsp, ~0XF
    xor rbp, rbp

    call ZeroBss

    mov rbx, __init_array_start

; ***************************************************************************
; *  AUTHOR  : Trollycat                                                    *
; *  FUNC    : .CtorLoop                                                    *
; *  DATE    : 2026                                                         *
; *  PURPOSE : Iterates __init_array_start to __init_array_end, calling     *
; *            each 8-byte function pointer in sequence.                    *
; ***************************************************************************
.CtorLoop:
    cmp rbx, __init_array_end
    jge .CtorDone
    call qword [rbx]
    add rbx, 8
    jmp .CtorLoop
.CtorDone:
    mov edi, r12d
    mov esi, r13d
    call CbkSystemStartup

    jmp CbkEarlyFaultLockdown