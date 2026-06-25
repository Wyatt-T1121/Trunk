; /* ******************************************************************************
; *                                                                               *
; *  Copyright 2026 Trollycat                                                     *
; *                                                                               *
; *  Licensed under the Apache License, Version 2.0 (the "License");              *
; *  you may not use this file except in compliance with the License.             *
; *  You may obtain a copy of the License at                                      *
; *                                                                               *
; *      http://www.apache.org/licenses/LICENSE-2.0                               *
; *                                                                               *
; *  Unless required by applicable law or agreed to in writing, software          *
; *  distributed under the License is distributed on an "AS IS" BASIS,            *
; *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     *
; *  See the License for the specific language governing permissions and          *
; *  limitations under the License.                                               *
; *                                                                               *
; *********************************************************************************
; *  AUTHOR  : Trollycat                                                          *
; *  MODULE  : Interrupt subsystem                                                *
; *  DATE    : 2026                                                               *
; *  PURPOSE : Traps interrupts, collects data, sends to dispatcher.              *
; ********************************************************************************/

[bits 64]

extern KInterruptDispatcher

section .text

%macro pushaq 0
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popaq 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

CommonInterruptHandler:
    pushaq

    mov rdi, rsp

    cld
    call KInterruptDispatcher

    popaq
    add rsp, 16
    iretq

%macro ISR_NOERRCODE 1
global VectorHandler_%1
VectorHandler_%1:
    push qword 0
    push qword %1
    jmp CommonInterruptHandler
%endmacro

%macro ISR_ERRCODE 1
global VectorHandler_%1
VectorHandler_%1:
    push qword %1
    jmp CommonInterruptHandler
%endmacro

ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE   8
ISR_NOERRCODE 9
ISR_ERRCODE   10
ISR_ERRCODE   11
ISR_ERRCODE   12
ISR_ERRCODE   13
ISR_ERRCODE   14
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE   17
ISR_NOERRCODE 18
ISR_NOERRCODE 19

%assign i 20
%rep 12
    ISR_NOERRCODE i
    %assign i i+1
%endrep

%assign i 32
%rep 224
    ISR_NOERRCODE i
    %assign i i+1
%endrep

section .data
global g_InterruptVectorTable
g_InterruptVectorTable:
%assign i 0
%rep 256
    dq VectorHandler_%+i
    %assign i i+1
%endrep