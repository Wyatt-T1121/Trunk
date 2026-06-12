/* *******************************************************************************
 *                                                                               *
 *  Copyright 2026 Trollycat                                                     *
 *                                                                               *
 *  Licensed under the Apache License, Version 2.0 (the "License");              *
 *  you may not use this file except in compliance with the License.             *
 *  You may obtain a copy of the License at                                      *
 *                                                                               *
 *      http://www.apache.org/licenses/LICENSE-2.0                               *
 *                                                                               *
 *  Unless required by applicable law or agreed to in writing, software          *
 *  distributed under the License is distributed on an "AS IS" BASIS,            *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.     *
 *  See the License for the specific language governing permissions and          *
 *  limitations under the License.                                               *
 *                                                                               *
 *********************************************************************************
 *                                                                               *
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Interrupt subsystem                                                *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores all 19 status codes for the RSOD                            *
 ********************************************************************************/
#pragma once

#include <types.h>

namespace trunk::interrupts
{
    // clang-format off
    enum STATUS : u32
    {
        STATUS_SUCCESS                  = 0x00000000, // Safe operational baseline
        STATUS_BREAKPOINT               = 0x80000003, // Vector 3: Breakpoint (#BP)
        STATUS_INTEGER_DIVIDE_BY_ZERO   = 0xC0000094, // Vector 0: Divide-by-Zero (#DE)
        STATUS_SINGLE_STEP_DEBUG        = 0xC0000091, // Vector 1: Debug Exception (#DB)
        STATUS_NMI_HARDWARE_FAILURE     = 0xC0000096, // Vector 2: Non-Maskable Interrupt (NMI)
        STATUS_INTEGER_OVERFLOW         = 0xC0000095, // Vector 4: Overflow Detect (#OF)
        STATUS_ARRAY_BOUNDS_EXCEEDED    = 0xC000008C, // Vector 5: BOUND Range Exceeded (#BR)
        STATUS_ILLEGAL_INSTRUCTION      = 0xC000001D, // Vector 6: Invalid Opcode (#UD)
        STATUS_FLOAT_INVALID_OPERATION  = 0xC0000090, // Vector 7: Device Not Available (#NM)
        STATUS_DOUBLE_FAULT             = 0xC000007C, // Vector 8: Double Fault (#DF)
        STATUS_INVALID_DISPOSITION      = 0xC0000026, // Vector 10: Invalid TSS (#TS)
        STATUS_IN_PAGE_ERROR            = 0xC0000006, // Vector 11: Segment Not Present (#NP)
        STATUS_STACK_OVERFLOW           = 0xC00000FD, // Vector 12: Stack-Segment Fault (#SS)
        STATUS_GENERAL_PROTECTION_FAULT = 0xC000001E, // Vector 13: General Protection Fault (#GP)
        STATUS_ACCESS_VIOLATION         = 0xC0000005, // Vector 14: Page Fault (#PF)
        STATUS_FLOAT_STACK_CHECK        = 0xC0000092, // Vector 16: x87 FPU Math Exception (#MF)
        STATUS_DATATYPE_MISALIGNMENT    = 0xC00002C5, // Vector 17: Alignment Check Fault (#AC)
        STATUS_MACHINE_CHECK            = 0xC0000097, // Vector 18: Machine Check (#MC)
        STATUS_SIMD_FAULT               = 0xC0000093, // Vector 19: SIMD Floating-Point (#XM)
        STATUS_UNSUCCESSFUL             = 0xC0000001  // Generic software fallback failure
    };
    // clang-format on

} // namespace trunk::interrupts