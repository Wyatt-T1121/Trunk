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
 *  MODULE  : Serial communication driver                                       *
 *  DATE    : 2026                                                              *
 *  PURPOSE : COM1 serial port driver implementation.                           *
 *            Initialises the UART chip and provides byte-level output.         *
 *            QEMU forwards COM1 to stdout via -serial stdio.                   *
 *                                                                              *
 * *****************************************************************************/

#include <trunk/drivers/serial/serial.h>

namespace trunk::drivers::serial
{

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_is_transmit_ready                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if the transmit buffer is empty.                      *
     *            Reads bit 5 (THRE) of the line status register.                   *
     *                                                                              *
     * *****************************************************************************/
    bool serial_is_transmit_ready() noexcept
    {
        return (asi::inb(SERIAL_REG_LINE_STATUS) & SERIAL_LSR_TX_EMPTY) != 0;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_init                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Initialise COM1 at 115200 baud, 8N1, FIFO enabled.                *
     *            Sequence: disable interrupts, set baud rate via DLAB,             *
     *            configure 8N1 line control, enable and clear FIFO,                *
     *            enable modem control.                                             *
     *                                                                              *
     * *****************************************************************************/
    void serial_init() noexcept
    {
        // Disable all interrupts
        asi::outb(SERIAL_REG_INT_ENABLE, 0x00);

        // Enable DLAB to set baud rate divisor
        asi::outb(SERIAL_REG_LINE_CTRL, SERIAL_LCR_DLAB);

        // Set divisor low and high bytes (115200 baud)
        asi::outb(SERIAL_REG_DATA, SERIAL_BAUD_115200_LO);
        asi::outb(SERIAL_REG_INT_ENABLE, SERIAL_BAUD_115200_HI);

        // Disable DLAB, set 8 data bits, no parity, 1 stop bit (8N1)
        asi::outb(SERIAL_REG_LINE_CTRL, SERIAL_LCR_8N1);

        // Enable FIFO, clear TX and RX queues, 14-byte threshold
        asi::outb(SERIAL_REG_FIFO,
                  SERIAL_FCR_ENABLE |
                      SERIAL_FCR_CLEAR_RX |
                      SERIAL_FCR_CLEAR_TX |
                      SERIAL_FCR_TRIGGER_14);

        // Enable modem: DTR + RTS + OUT2 (required for interrupts later)
        asi::outb(SERIAL_REG_MODEM_CTRL, 0x0B);
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_putchar                                                    *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write one character to COM1.                                      *
     *            Spins until the transmit buffer is empty before writing.          *
     *            Converts bare LF to CRLF so terminals display correctly.          *
     *                                                                              *
     * *****************************************************************************/
    void serial_putchar(char c) noexcept
    {
        // Convert \n to \r\n for terminal compatibility
        if (c == '\n')
            serial_putchar('\r');

        while (!serial_is_transmit_ready())
        {
        }

#ifdef TRUNK_DEBUG
        asi::outb(SERIAL_REG_DATA, static_cast<u8>(c));
#else
        (void)c; // Suppress unused parameter warning in release builds
#endif
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_puts                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a null-terminated string to COM1.                           *
     *            Calls serial_putchar for each character.                          *
     *                                                                              *
     * *****************************************************************************/
    void serial_puts(const char *s) noexcept
    {
        while (*s)
            serial_putchar(*s++);
    }

} // namespace trunk::drivers::serial