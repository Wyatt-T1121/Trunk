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
 *  MODULE  : User welcome                                                       *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Welcome information for the user                                   *
 ********************************************************************************/
#include <cbk/kern/welcome.h>
#include <drivers/serial/serial.h>

#include <version.h>

namespace serial = trunk::drivers::serial;

namespace trunk::kernel
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : welcome_user                                                       *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Welcomes the user to Trunk                                         *
     ********************************************************************************/
    void welcome_user() noexcept
    {
        serial::serial_puts("Welcome to Trunk!\n");
        serial::serial_puts("The Hobby C++ operating system.\n");
        serial::serial_puts("Copyright (c) ALL CONTRIBUTERS TO TRUNK.\n");
        serial::serial_puts("You are likely a developer, as you have built Trunk In DEBUG mode!\n");

        serial::serial_puts("VERSION: ");
        serial::serial_puts(get_version().build_string);
    }
} // namespace trunk::kernel