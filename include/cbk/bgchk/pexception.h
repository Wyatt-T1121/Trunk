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
 *  MODULE  : Exception pointer                                                  *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores ExceptionRecord                                             *
 ********************************************************************************/
#pragma once

#include <types.h>

#include <cbk/intr/trap_frame.h>

namespace cbk::kernel
{
    struct ExceptionRecord
    {
        DWORD exception_code;
        DWORD exception_flags;
        ExceptionRecord *exception_record;
        PVOID exception_address;
        DWORD number_parameters;
        ULONG_PTR exception_information[15];
    };

    struct ExceptionPointers
    {
        ExceptionRecord *exception_record;
        interrupts::TrapFrame *context_record;
    };
} // namespace cbk::kernel