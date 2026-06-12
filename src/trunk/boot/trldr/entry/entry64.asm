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
; *******************************************************************************

bits 64

extern TrSystemStartup

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
; *  FUNC    : load_mb2_from_memory                                             *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads MB2 values saved by entry32.asm before the mode switch.    *
; *******************************************************************************
load_mb2_from_memory:
    mov r12d, dword [mb2_magic_store]
    mov r13d, dword [mb2_info_store]
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : load_64b_data_segments                                           *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads 64-bit data segments                                       *
; *******************************************************************************
load_64b_data_segments:
    mov ax, 16
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : zero_bss                                                         *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Zeros the BSS section                                            *
; *******************************************************************************
zero_bss:
    mov rdi, __bss_start
    mov rcx, __bss_end
    sub rcx, rdi
    xor al,  al
    rep stosb
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : entry64                                                          *
; *  DATE    : 2026                                                             *
; *  PURPOSE : 64-bit entry level code, sets up the stack, calls C++ global con *
; *******************************************************************************
entry64:
    cli
    cld
    
    call load_mb2_from_memory
    call load_64b_data_segments

    mov rsp, __stack_top
    and rsp, ~0XF
    xor rbp, rbp

    call zero_bss

    mov rbx, __init_array_start

; ***************************************************************************
; *  AUTHOR  : Trollycat                                                    *
; *  FUNC    : ctor_loop                                                    *
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
    mov edi, r12d
    mov esi, r13d
    call TrSystemStartup
.hang:
    cli
    hlt
    jmp .hang