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
; *                                                                               *
; *  AUTHOR  : Trollycat                                                          *
; *  FILE    : Gdt.asm                                                            *
; *  DATE    : 2026                                                               *
; *  PURPOSE : Assembly code for the Global Descriptor Table.                     *
; *            Contains gdt_flush (called by c++)                                 *
; *                                                                               *
; *                                                                               *
; ********************************************************************************/

bits 64

section .text
global gdt_flush

; *********************************************************************************
; *  AUTHOR  : Trollycat                                                          *
; *  FUNC    : gdt_flush                                                          *
; *  DATE    : 2026                                                               *
; *  PURPOSE : Assembly function that flushes the global descriptor table         *
; ********************************************************************************/
gdt_flush:
    lgdt [rdi]

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    push 0x08
    lea rax, [.reload_segments]
    push rax
    retfq

; *********************************************************************************
; *  AUTHOR  : Trollycat                                                          *
; *  FUNC    : .reload_segments                                                   *
; *  DATE    : 2026                                                               *
; *  PURPOSE : Landing pad after far return                                       *
; ********************************************************************************/
.reload_segments:
    ret