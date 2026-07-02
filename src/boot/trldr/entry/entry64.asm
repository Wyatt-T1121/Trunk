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

extern KeSystemStartup

extern __bss_start
extern __bss_end

extern __stack_top

extern __init_array_start
extern __init_array_end

extern InMultiboot2MagicStore
extern InMultiboot2InfoStore

extern InEmergencyLockdown

global In64BitEntry

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : InLoadMultiboot2FromMemory                                       *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads MB2 values saved by entry32.asm before the mode switch.    *
; *******************************************************************************
InLoadMultiboot2FromMemory:
    mov r12d, dword [InMultiboot2MagicStore]
    mov r13d, dword [InMultiboot2InfoStore]
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : InLoad64BitDataSegments                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads 64-bit data segments                                       *
; *******************************************************************************
InLoad64BitDataSegments:
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : InZeroBss                                                        *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Zeros the BSS section                                            *
; *******************************************************************************
InZeroBss:
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    xor al,  al
    rep stosb
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : In64BitEntry                                                     *
; *  DATE    : 2026                                                             *
; *  PURPOSE : 64-bit entry level code, sets up the stack, calls C++ global con *
; *******************************************************************************
In64BitEntry:
    cli
    cld
    
    call InLoadMultiboot2FromMemory
    call InLoad64BitDataSegments

    mov rsp, __stack_top
    and rsp, ~0XF
    xor rbp, rbp

    call InZeroBss

    mov rbx, __init_array_start

; ***************************************************************************
; *  AUTHOR  : Trollycat                                                    *
; *  FUNC    : .InCtorLoop                                                  *
; *  DATE    : 2026                                                         *
; *  PURPOSE : Iterates __init_array_start to __init_array_end, calling     *
; *            each 8-byte function pointer in sequence.                    *
; ***************************************************************************
.InCtorLoop:
    cmp rbx, __init_array_end
    jge .InCtorFinish
    call qword [rbx]
    add rbx, 8
    jmp .InCtorLoop
.InCtorFinish:
    mov edi, r12d
    mov esi, r13d
    call KeSystemStartup

    ; If it fails to calL KeSystemStartup, halt the kernel ASAP...
    ; Should probably use KDCOM here to signal an error...
    ; Otherwise you kind of just sit there wondering what failed...
    jmp InEmergencyLockdown