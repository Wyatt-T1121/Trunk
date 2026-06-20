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
 *  MODULE  : Test framework                                                     *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Kernel mocking file. This is needed to test kernel comps like      *
 *                                                          memblock             *
 ********************************************************************************/

#include <cstdio>
#include <cstdlib>

#include <gtest/gtest.h>

#include <types.h>

asm(".data\n"
    ".global __kernel_phys_start\n"
    ".global __kernel_phys_end\n"
    ".global __stack_bottom\n"
    ".global __stack_top\n"
    "__kernel_phys_start:\n"
    "    .space 1048576\n"
    "__kernel_phys_end:\n"
    "    .space 8\n"
    "__stack_bottom:\n"
    "    .space 4096\n"
    "__stack_top:\n"
    "    .space 8\n");

namespace trunk::kernel
{

    NO_RETURN VOID KAbort(const char *message) noexcept
    {
        std::printf("\n[ KERNEL PANIC ]: %s\n\n", message ? message : "No message provided");
        ADD_FAILURE() << "Kernel panicked: " << (message ? message : "");
        std::abort();
    }

} // namespace trunk::kernel