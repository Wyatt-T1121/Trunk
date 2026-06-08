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
 *  PURPOSE : Declares kmain — the only symbol Boot.cpp needs from the kernel.   *
 *                                                                               *
 ********************************************************************************/

#pragma once

#include <trunk/boot/boot.h>

/* *******************************************************************************
 *  AUTHOR  : Trollycat                                                          *
 *  FUNC    : kmain                                                              *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Kernel entry point. Called once by boot_entry() after the machine  *
 *            is in a known-good 64-bit state. Never returns.                    *
 ********************************************************************************/
extern "C" [[noreturn]]
void kmain(const trunk::boot::BootInfo &info) noexcept;