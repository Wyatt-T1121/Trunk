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
 *  MODULE  : Task State Segment                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initializes the Task State Segment                     *
 ********************************************************************************/
#pragma once

#include <cbk/desc/descriptor.h>
#include <macros.h>
#include <types.h>

namespace trunk::gdt
{
    inline constexpr DWORD IST_STACK_SIZE = 4096;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TssInit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the Task State Segment                                 *
     ********************************************************************************/
    VOID TssInit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TssSetRsp0                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Set the RSP0 field for ring mode                                   *
     ********************************************************************************/
    VOID TssSetRsp0(QWORD rsp) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : TssGet                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Get the current tss by reference                                   *
     ********************************************************************************/
    NO_DISCARD const Tss &TssGet() noexcept;
} // namespace trunk::gdt