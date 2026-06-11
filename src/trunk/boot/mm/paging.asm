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
; *  PURPOSE : Builds the early boot page tables and enables 64-bit long mode.  *
; *            Two mappings are created over the same first 1GB of physical     *
; *            RAM: an identity map (virt == phys) so execution continues       *
; *            after paging is switched on, and a higher-half map at            *
; *            0xFFFFFFFF80000000 where the kernel actually lives.              *
; *            Uses 2MB huge pages, no PT level needed, simpler and faster.     *
; *                                                                             *
; *******************************************************************************

bits 32

; Physical addresses for the temporary page tables.
; These sit in low memory below the kernel load address (1MB).
; Each table is one 4KB page = 512 eight-byte entries.

PML4_ADDR       equ 0x1000      ; Page Map Level 4
PDPT_ID         equ 0x2000      ; PDPT for identity map (PML4 entry 0)
PDPT_HH         equ 0x3000      ; PDPT for higher-half  (PML4 entry 511)
PD_ADDR         equ 0x4000      ; Page Directory shared by both PDPTs

; Page table flag bits.

PTE_PRESENT     equ (1 << 0)
PTE_WRITABLE    equ (1 << 1)
PTE_HUGE        equ (1 << 7)
PTE_FLAGS       equ PTE_PRESENT | PTE_WRITABLE
PTE_HUGE_FLAGS  equ PTE_PRESENT | PTE_WRITABLE | PTE_HUGE

; PDPT index for 0xFFFFFFFF80000000.
; Virtual address breakdown:
;   PML4 index 511
;   PDPT index 510
;   PD   index 0    (first 2MB page)

HH_PDPT_INDEX   equ 510

section .boot.text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : setup_page_tables                                                *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Zeroes the four page-table pages then populates them:            *
; *              PML4[0]        -> PDPT_ID (identity map)                       *
; *              PML4[511]      -> PDPT_HH (higher-half)                        *
; *              PDPT_ID[0]     -> PD_ADDR                                      *
; *              PDPT_HH[510]   -> PD_ADDR                                      *
; *              PD[0..511]     -> 512 x 2MB huge pages covering 0 to 1GB       *
; *            Called from entry32.asm before long mode is enabled.             *
; *******************************************************************************

global setup_page_tables
setup_page_tables:

    ; Zero all four tables (4 pages = 0x4000 bytes).
    mov edi, PML4_ADDR
    xor eax, eax
    mov ecx, (0x4000 / 4)
    rep stosd

    ; PML4 entry 0 -> PDPT_ID (identity map: virtual 0x0 to 0x7FFFFFFFFFFF)
    mov dword [PML4_ADDR],           PDPT_ID | PTE_FLAGS

    ; PML4 entry 511 -> PDPT_HH (higher-half: virtual 0xFFFF800000000000+)
    mov dword [PML4_ADDR + 511 * 8], PDPT_HH | PTE_FLAGS

    ; Identity PDPT entry 0 -> PD (covers virtual 0x0 to 0x3FFFFFFF = 1GB)
    mov dword [PDPT_ID], PD_ADDR | PTE_FLAGS

    ; Higher-half PDPT entry 510 -> PD (virtual 0xFFFFFFFF80000000)
    mov dword [PDPT_HH + HH_PDPT_INDEX * 8], PD_ADDR | PTE_FLAGS

    ; Page Directory: 512 x 2MB huge pages covering 0 to 1GB physical.
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
; *            The CPU enters compatibility mode after this returns. A far      *
; *            jump to a 64-bit code descriptor in entry32.asm completes the    *
; *            switch. Must be called after setup_page_tables.                  *
; *******************************************************************************

global enable_long_mode
enable_long_mode:

    ; 1. Point CR3 at the PML4.
    mov eax, PML4_ADDR
    mov cr3, eax

    ; 2. Enable PAE (Physical Address Extension) — required for 64-bit paging.
    mov eax, cr4
    or  eax, (1 << 5)
    mov cr4, eax

    ; 3. Set EFER.LME (Long Mode Enable) — signals intent, not yet active.
    mov ecx, 0xC0000080
    rdmsr
    or  eax, (1 << 8)
    wrmsr

    ; 4. Enable paging (CR0.PG) — long mode activates here.
    ;    CR0.PE is already set — GRUB guarantees protected mode on entry.
    mov eax, cr0
    or  eax, (1 << 31)
    mov cr0, eax

    ret