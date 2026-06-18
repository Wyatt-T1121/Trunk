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
; *  PURPOSE : Saves MB2 magic + info ptr                                       *
; *******************************************************************************
bits 32
section .boot.text

extern SetupPageTables
extern EnableLongMode

global _Start

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : SaveMb2ToMemory                                                  *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Stores MB2 values to memory in boot section so they can survive  *
; *******************************************************************************
SaveMb2ToMemory:
    mov dword [Mb2MagicStore], eax
    mov dword [Mb2InfoStore], ebx
    
    mov edi, eax
    mov esi, ebx
    
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : _Start                                                           *
; *  DATE    : 2026                                                             *
; *  PURPOSE : GRUB entry. The 32-64 bit switch.                                *
; *******************************************************************************
_Start:
    cli
    
    call SaveMb2ToMemory

    mov esp, 0x7C00
    
    call SetupPageTables
    call EnableLongMode

    lgdt [Gdt64Ptr]

    jmp 0x08:Trampoline64

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : Trampoline64                                                     *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Tiny 64-bit stub that lives in the physical boot region.         *
; *******************************************************************************
bits 64
Trampoline64:
    mov rax, [Entry64Vaddr]
    jmp rax
    jmp CbkEarlyFaultLockdown

align 16
Gdt64:
    dq 0
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    dq (1 << 41) | (1 << 44) | (1 << 47)
Gdt64Ptr:
    dw Gdt64Ptr - Gdt64 - 1
    dd Gdt64
extern Entry64
align 8
Entry64Vaddr:
    dq Entry64

align 8
global Mb2MagicStore
global Mb2InfoStore

Mb2MagicStore:
    dd 0

Mb2InfoStore:
    dd 0

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : CbkEarlyFaultLockdown                                            *
; *  DATE    : 2026                                                             *
; *  PURPOSE : halt loop incase any assembly code fails                         *
; *******************************************************************************
global CbkEarlyFaultLockdown
CbkEarlyFaultLockdown:
    cli
.lock_loop:
    hlt
    pause
    jmp .lock_loop