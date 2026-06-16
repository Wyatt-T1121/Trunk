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
; *  PURPOSE : Builds the early boot page tables and enables 64-bit long mode.  *
; *******************************************************************************

bits 32

PML4_ADDR       equ 0x1000
PDPT_ID         equ 0x2000
PDPT_HH         equ 0x3000
PD_ADDR         equ 0x4000

PTE_PRESENT     equ (1 << 0)
PTE_WRITABLE    equ (1 << 1)
PTE_HUGE        equ (1 << 7)
PTE_FLAGS       equ PTE_PRESENT | PTE_WRITABLE
PTE_HUGE_FLAGS  equ PTE_PRESENT | PTE_WRITABLE | PTE_HUGE

HH_PDPT_INDEX   equ 510

section .boot.text


; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : zero_page_tables                                                 *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Zeroes the four page-table pages                                 *
; *******************************************************************************
zero_page_tables:
    mov edi, PML4_ADDR
    xor eax, eax
    mov ecx, (0x4000 / 4)
    rep stosd
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : populate_page_tables                                             *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Populates the page tables with appropriate mappings              *
; *******************************************************************************
populate_page_tables:
    mov dword [PML4_ADDR],                   PDPT_ID | PTE_FLAGS
    mov dword [PML4_ADDR + 511 * 8],         PDPT_HH | PTE_FLAGS
    mov dword [PDPT_ID], PD_ADDR |           PTE_FLAGS
    mov dword [PDPT_HH + HH_PDPT_INDEX * 8], PD_ADDR | PTE_FLAGS
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : setup_page_tables                                                *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Setup the page tables                                            *
; *******************************************************************************
global setup_page_tables
setup_page_tables:
    call zero_page_tables
    call populate_page_tables

    mov edi, PD_ADDR
    mov eax, PTE_HUGE_FLAGS
    mov ecx, 512

.fill_pd:
    mov dword [edi], eax
    add eax, 0x200000
    add edi, 8
    loop .fill_pd
    ret

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : enable_long_mode                                                 *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Loads CR3, enables PAE in CR4, sets EFER.LME via MSR, then       *
; *            enables paging in CR0 which activates 64-bit long mode.          *
; *******************************************************************************
global enable_long_mode
enable_long_mode:
    mov eax, PML4_ADDR
    mov cr3, eax

    mov eax, cr4
    or  eax, (1 << 5)
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or  eax, (1 << 8)
    wrmsr

    mov eax, cr0
    or  eax, (1 << 31)
    mov cr0, eax

    ret