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

extern InSetupPageTables
extern InEnableLongMode

global _InStartSystem

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : InSaveMultiboot2ToMemory                                         *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Stores MB2 values to memory in boot section so they can survive  *
; *******************************************************************************
InSaveMultiboot2ToMemory:
    mov dword [InMultiboot2MagicStore], eax
    mov dword [InMultiboot2InfoStore], ebx
    
    mov edi, eax
    mov esi, ebx
    
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : _InStartSystem                                                   *
; *  DATE    : 2026                                                             *
; *  PURPOSE : GRUB entry. The 32-64 bit switch.                                *
; *******************************************************************************
_InStartSystem:
    cli
    mov esp, 0x7C00

    call InSaveMultiboot2ToMemory
    call InSetupPageTables
    call InEnableLongMode

    lgdt [InGdt64Pointer]
    jmp 0x08:InTrampoline64

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : InTrampoline64                                                   *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Tiny 64-bit stub that lives in the physical boot region.         *
; *******************************************************************************
bits 64
InTrampoline64:
    mov rax, [InEntry64VirtualAddress]
    jmp rax
    jmp InEmergencyLockdown

align 16
InGdt64:
    dq 0
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    dq (1 << 41) | (1 << 44) | (1 << 47)
InGdt64Pointer:
    dw InGdt64Pointer - InGdt64 - 1
    dd InGdt64

extern In64BitEntry
align 8

InEntry64VirtualAddress:
    dq In64BitEntry

align 8

global InMultiboot2MagicStore
InMultiboot2MagicStore:
    dd 0

global InMultiboot2InfoStore
InMultiboot2InfoStore:
    dd 0

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : InEmergencyLockdown                                              *
; *  DATE    : 2026                                                             *
; *  PURPOSE : halt loop incase any assembly code fails                         *
; *******************************************************************************
global InEmergencyLockdown
InEmergencyLockdown:
    cli
.IniLockLoop:
    hlt
    pause
    jmp .IniLockLoop