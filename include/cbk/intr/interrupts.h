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
 *  PURPOSE : Implements registration array logic and exception translation      *
 ********************************************************************************/
#pragma once

#include <types.h>

#include <cbk/intr/dispatcher.h>

namespace cbk::interrupts
{
    using InterruptHandler = VOID (*)(InterruptFrame *frame, PVOID ctx);

    struct RegisteredHandler
    {
        InterruptHandler handler = nullptr;
        PVOID context            = nullptr;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RegisterInterruptHandler                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Assigns a custom C++ driver function to an IDT slot                *
     ********************************************************************************/
    VOID RegisterInterruptHandler(BYTE vector, InterruptHandler handler,
                                  PVOID context = nullptr) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : ExecuteInterruptHandler                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Invoked to route traffic or detect unhandled traps                 *
     ********************************************************************************/
    VOID ExecuteInterruptHandler(BYTE vector, InterruptFrame *frame) noexcept;

} // namespace cbk::interrupts