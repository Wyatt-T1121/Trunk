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
 *  MODULE  : Core kernel                                                        *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Kernel entry point file, declares CbkStartup()                     *
 *                                                                               *
 ********************************************************************************/

#pragma once

#include <boot/trldr/mb2/boot.h>

#include <macros.h>

namespace cbk::kernel
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : CbkStartup()                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Main kernel function. Called by CbkLoad()                          *
     ********************************************************************************/
    extern "C" NO_RETURN VOID CbkStartup(CONST boot::BootInfo &info) noexcept;

} // namespace cbk::kernel