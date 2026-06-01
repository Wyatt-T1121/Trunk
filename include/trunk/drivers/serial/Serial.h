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
 *  FILE    : Serial.h                                                          *
 *  DATE    : 2026                                                              *
 *  PURPOSE : COM1 serial port driver declarations.                             *
 *            Talks to hardware via port I/O. No dependencies beyond Io.h.      *
 *            Hardcoded early driver — a proper version comes with the          *
 *            driver subsystem later.                                           *
 *                                                                              *
 * *****************************************************************************/

#pragma once

#include <trunk/Types.h>
#include <trunk/asi/Io.h>

namespace trunk
{

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  SECTION : COM1 port addresses                                               *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Base address and register offsets for COM1.                       *
     *            All access goes through Io.h outb/inb.                            *
     *                                                                              *
     * *****************************************************************************/
    inline constexpr u16 SERIAL_COM1_BASE = 0x3F8;

    inline constexpr u16 SERIAL_REG_DATA = SERIAL_COM1_BASE + 0;
    inline constexpr u16 SERIAL_REG_INT_ENABLE = SERIAL_COM1_BASE + 1;
    inline constexpr u16 SERIAL_REG_FIFO = SERIAL_COM1_BASE + 2;
    inline constexpr u16 SERIAL_REG_LINE_CTRL = SERIAL_COM1_BASE + 3;
    inline constexpr u16 SERIAL_REG_MODEM_CTRL = SERIAL_COM1_BASE + 4;
    inline constexpr u16 SERIAL_REG_LINE_STATUS = SERIAL_COM1_BASE + 5;
    inline constexpr u16 SERIAL_REG_MODEM_STATUS = SERIAL_COM1_BASE + 6;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  SECTION : Baud rate divisors                                                *
     *  DATE    : 2026                                                              *
     *  PURPOSE : DLAB must be set in LINE_CTRL before writing these.               *
     *            115200 baud is standard for QEMU serial output.                   *
     *                                                                              *
     * *****************************************************************************/
    inline constexpr u16 SERIAL_BAUD_115200_LO = 0x01;
    inline constexpr u16 SERIAL_BAUD_115200_HI = 0x00;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  SECTION : Line control flags                                                *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Bit flags for SERIAL_REG_LINE_CTRL.                               *
     *                                                                              *
     * *****************************************************************************/
    inline constexpr u8 SERIAL_LCR_8N1 = 0x03;
    inline constexpr u8 SERIAL_LCR_DLAB = 0x80;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  SECTION : FIFO control flags                                                *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Bit flags for SERIAL_REG_FIFO.                                    *
     *                                                                              *
     * *****************************************************************************/
    inline constexpr u8 SERIAL_FCR_ENABLE = 0x01;
    inline constexpr u8 SERIAL_FCR_CLEAR_RX = 0x02;
    inline constexpr u8 SERIAL_FCR_CLEAR_TX = 0x04;
    inline constexpr u8 SERIAL_FCR_TRIGGER_14 = 0xC0;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  SECTION : Line status flags                                                 *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Bit flags for SERIAL_REG_LINE_STATUS.                             *
     *                                                                              *
     * *****************************************************************************/
    inline constexpr u8 SERIAL_LSR_TX_EMPTY = 0x20;
    inline constexpr u8 SERIAL_LSR_DATA_READY = 0x01;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_init                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Initialise COM1 at 115200 baud, 8N1, FIFO enabled.                *
     *            Must be called before any serial output.                          *
     *                                                                              *
     * *****************************************************************************/
    void serial_init() noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_putchar                                                    *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write one character to COM1.                                      *
     *            Spins until the transmit buffer is ready.                         *
     *                                                                              *
     * *****************************************************************************/
    void serial_putchar(char c) noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_puts                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a null-terminated string to COM1.                           *
     *                                                                              *
     * *****************************************************************************/
    void serial_puts(const char *s) noexcept;

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_is_transmit_ready                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Return true if the transmit buffer is empty.                      *
     *            Used internally by serial_putchar.                                *
     *                                                                              *
     * *****************************************************************************/
    [[nodiscard]] bool serial_is_transmit_ready() noexcept;

} // namespace trunk