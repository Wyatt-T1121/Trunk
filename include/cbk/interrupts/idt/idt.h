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
 *  PURPOSE :  Populates the 256 gates and executes physical lidt instruction    *
 ********************************************************************************/
#pragma once

#include <assert.h>
#include <macros.h>
#include <types.h>

#include <cbk/desc/descriptor.h>

namespace trunk::interrupts
{
    STATIC_ASSERT(sizeof(IdtDescriptor) == 16, "IdtDescriptor must be exactly 16 bytes!");
    STATIC_ASSERT(sizeof(IdtrPointer) == 10, "IdtrPointer must be exactly 10 bytes!");

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : idt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the interrupt descriptor table                          *
     ********************************************************************************/
    void idt_init() noexcept;

} // namespace trunk::interrupts
