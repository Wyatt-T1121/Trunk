; *********************************************************************************
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
; *  MODULE  : Architecture Initialization                                        *
; *  DATE    : 2026                                                               *
; *  PURPOSE : Global descriptor table (assembly) code                            *
; ********************************************************************************/

bits 64

section .text

; *********************************************************************************
; *  AUTHOR  : Trollycat                                                          *
; *  FUNC    : gdt_flush                                                          *
; *  DATE    : 2026                                                               *
; *  PURPOSE : Assembly function that flushes the global descriptor table         *
; ********************************************************************************/
global gdt_flush
gdt_flush:
    lgdt [rdi]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, [rsp]          

    mov qword [rsp], 0x08
    push rax
    retfq