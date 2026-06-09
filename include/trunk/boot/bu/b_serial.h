/* ******************************************************************************
 *                                                                              *
 *  Copyright 2026 Trollycat                                                    *
 *                                                                              *
 *  Licensed under the Apache License, Version 2.0 (the "License");             *
 *  you may not use this file except in compliance with the License.            *
 *  You may obtain a copy of the License at                                     *
 *                                                                              *
 *      http://www.apache.org/licenses/LICENSE-2.0                              *
 *                                                                              *
 *  Unless required by applicable law or agreed to in writing, software         *
 *  distributed under the License is distributed on an "AS IS" BASIS,           *
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.    *
 *  See the License for the specific language governing permissions and         *
 *  limitations under the License.                                              *
 *                                                                              *
 ********************************************************************************
 *                                                                              *
 *  AUTHOR  : Trollycat                                                         *
 *  MODULE  : Bootstrapping                                                     *
 *  DATE    : 2026                                                              *
 *  PURPOSE : Raw UART output for the boot stage.                               *
 *            Not a driver — directly pokes COM1 I/O ports with no              *
 *            abstraction layer. Used for boot diagnostics and boot_panic.      *
 *            No tklib dependency. Boot stage only.                             *
 *                                                                              *
 * *****************************************************************************/

#pragma once

#include <types.h>

namespace trunk::boot
{

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_init                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Initialise COM1 at 115200 baud, 8N1.                              *
     *            Must be called once before any b_serial_put* call.                *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_init() noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_putc                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a single character to COM1. Spins until the                 *
     *            transmit-holding-register-empty bit is set.                       *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_putc(char c) noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_puts                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a null-terminated string to COM1.                           *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_puts(const char *s) noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_put_hex                                                  *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a 64-bit value to COM1 as a zero-padded hex string.         *
     *            Format: 0x0000000000000000. Useful for printing addresses.        *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_put_hex(u64 val) noexcept;

} // namespace trunk::boot