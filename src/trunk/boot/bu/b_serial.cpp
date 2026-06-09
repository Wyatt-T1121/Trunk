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
 *            Directly pokes COM1 I/O ports. Not a driver. No tklib.            *
 *                                                                              *
 * *****************************************************************************/

#include <trunk/boot/bu/b_serial.h>
#include <trunk/asi/io.h>

using namespace trunk::asi;

namespace trunk::boot
{

    static constexpr u16 COM1_PORT = 0x3F8;
    static constexpr u16 COM1_DATA = COM1_PORT + 0;
    static constexpr u16 COM1_IER = COM1_PORT + 1;
    static constexpr u16 COM1_FCR = COM1_PORT + 2;
    static constexpr u16 COM1_LCR = COM1_PORT + 3;
    static constexpr u16 COM1_MCR = COM1_PORT + 4;
    static constexpr u16 COM1_LSR = COM1_PORT + 5;
    static constexpr u8 LCR_DLAB = 0x80;
    static constexpr u8 LCR_8N1 = 0x03;
    static constexpr u8 LSR_THRE = 0x20;
    static constexpr u8 BAUD_DIV_LO = 0x01; // 115200 baud divisor low
    static constexpr u8 BAUD_DIV_HI = 0x00; // 115200 baud divisor high

    static const char HEX_CHARS[] = "0123456789abcdef";

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_init                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Initialise COM1 at 115200 baud, 8N1. Sets DLAB to program         *
     *            the baud rate divisor, then clears DLAB and sets line params.     *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_init() noexcept
    {
        outb(COM1_IER, 0x00);         // Disable all interrupts
        outb(COM1_LCR, LCR_DLAB);     // Enable DLAB to set baud rate
        outb(COM1_DATA, BAUD_DIV_LO); // Baud rate divisor low byte
        outb(COM1_IER, BAUD_DIV_HI);  // Baud rate divisor high byte
        outb(COM1_LCR, LCR_8N1);      // 8 bits, no parity, one stop bit
        outb(COM1_FCR, 0xC7);         // Enable FIFO, clear, 14-byte threshold
        outb(COM1_MCR, 0x03);         // RTS + DTR asserted
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_putc                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Spin until THRE is set then transmit one character.               *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_putc(char c) noexcept
    {
        while ((inb(COM1_LSR) & LSR_THRE) == 0)
            ;
        outb(COM1_DATA, static_cast<u8>(c));
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_puts                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a null-terminated string to COM1 one character at a time.   *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_puts(const char *s) noexcept
    {
        if (!s)
            return;
        while (*s)
            b_serial_putc(*s++);
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : b_serial_put_hex                                                  *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Print a 64-bit value as 0x followed by 16 hex digits to COM1.     *
     *                                                                              *
     * *****************************************************************************/
    void b_serial_put_hex(u64 val) noexcept
    {
        b_serial_puts("0x");
        for (i32 shift = 60; shift >= 0; shift -= 4)
            b_serial_putc(HEX_CHARS[(val >> shift) & 0xF]);
    }

} // namespace trunk::boot