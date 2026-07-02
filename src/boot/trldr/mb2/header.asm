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
; *  PURPOSE : Multiboot2 magic header blob.                                    *
; *******************************************************************************
bits 32

MB2_MAGIC       equ 0xE85250D6
MB2_ARCH        equ 0
HEADER_LEN      equ (InMultiboot2End - InMb2Start)
MB2_CHECKSUM    equ -(MB2_MAGIC + MB2_ARCH + HEADER_LEN)

section .multiboot2
align 8

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : InMb2Start                                                       *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Multiboot2 header start                                          *
; *******************************************************************************
InMb2Start:
    dd MB2_MAGIC
    dd MB2_ARCH
    dd HEADER_LEN
    dd MB2_CHECKSUM

    align 8
    dw 5
    dw 1
    dd 20
    dd 0
    dd 0
    dd 0

    align 8
    dw 0
    dw 0
    dd 8
InMultiboot2End: