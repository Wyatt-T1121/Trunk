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
 *  AUTHOR  : Trollycat                                                          *
 *  MODULE  : Central processing unit flags                                      *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores hardware bitmask flags for the CPU                          *
 ********************************************************************************/
#pragma once

#include <types.h>

namespace trunk::cpu
{
    constexpr u64 CR0_PE = 0x00000001;
    constexpr u64 CR0_MP = 0x00000002;
    constexpr u64 CR0_EM = 0x00000004;
    constexpr u64 CR0_TS = 0x00000008;
    constexpr u64 CR0_ET = 0x00000010;
    constexpr u64 CR0_NE = 0x00000020;
    constexpr u64 CR0_WP = 0x00010000;
    constexpr u64 CR0_AM = 0x00040000;
    constexpr u64 CR0_NW = 0x20000000;
    constexpr u64 CR0_CD = 0x40000000;
    constexpr u64 CR0_PG = 0x80000000;

    constexpr u64 CR4_PCE = 0x00000100;
    constexpr u64 CR4_MCE = 0x00000040;
    constexpr u64 CR4_PSE = 0x00000010;
    constexpr u64 CR4_DE  = 0x00000008;
    constexpr u64 CR4_TSD = 0x00000004;
    constexpr u64 CR4_PVI = 0x00000002;
    constexpr u64 CR4_VME = 0x00000001;
    constexpr u64 CR4_PGE = 0x80;

    constexpr u64 FL_CF        = 0x00000001;
    constexpr u64 FL_PF        = 0x00000004;
    constexpr u64 FL_AF        = 0x00000010;
    constexpr u64 FL_ZF        = 0x00000040;
    constexpr u64 FL_SF        = 0x00000080;
    constexpr u64 FL_TF        = 0x00000100;
    constexpr u64 FL_IF        = 0x00000200;
    constexpr u64 FL_DF        = 0x00000400;
    constexpr u64 FL_OF        = 0x00000800;
    constexpr u64 FL_IOPL_MASK = 0x00003000;
    constexpr u64 FL_IOPL_0    = 0x00000000;
    constexpr u64 FL_IOPL_1    = 0x00001000;
    constexpr u64 FL_IOPL_2    = 0x00002000;
    constexpr u64 FL_IOPL_3    = 0x00003000;
    constexpr u64 FL_NT        = 0x00004000;
    constexpr u64 FL_RF        = 0x00010000;
    constexpr u64 FL_VM        = 0x00020000;
    constexpr u64 FL_AC        = 0x00040000;
    constexpr u64 FL_VIF       = 0x00080000;
    constexpr u64 FL_VIP       = 0x00100000;
    constexpr u64 FL_ID        = 0x00200000;

    constexpr u64 FEC_PR = 0x1;
    constexpr u64 FEC_WR = 0x2;
    constexpr u64 FEC_U  = 0x4;

    constexpr u64 EFER_NXE = 11;
} // namespace trunk::cpu