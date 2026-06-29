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
 *  MODULE  : Global definitions                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Linker script definitions pulled to c++                            *
 ********************************************************************************/
#pragma once

#include <types.h>

EXTERN_C
{
    extern CHAR __kernel_phys_start[];
    extern CHAR __kernel_phys_end[];

    extern CHAR __text_start[];
    extern CHAR __text_end[];

    extern CHAR __rodata_start[];
    extern CHAR __rodata_end[];

    extern CHAR __data_start[];
    extern CHAR __data_end[];

    extern CHAR __bss_start[];
    extern CHAR __bss_end[];

    extern BYTE __stack_bottom[];
    extern BYTE __stack_top[];
}