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
; *  MODULE  : System startup landing pad                                       *
; *  DATE    : 2026                                                             *
; *  PURPOSE : System startup landing pad. Called from entry64.asm              *
; *******************************************************************************
bits 64

extern InLoadKernel
extern InEmergencyLockdown

section .text

; *******************************************************************************
; *  AUTHOR  : Trollycat                                                        *
; *  FUNC    : KeSystemStartup                                                  *
; *  DATE    : 2026                                                             *
; *  PURPOSE : Kernel landing pad, ( assembly )                                 *
; *******************************************************************************
global KeSystemStartup
KeSystemStartup:
    call InLoadKernel
    jmp InEmergencyLockdown