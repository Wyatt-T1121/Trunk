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
 *  AUTHOR  : Trollycat                                                         *
 *  MODULE  : Serial communication driver                                       *
 *  DATE    : 2026                                                              *
 *  PURPOSE : COM1 serial port driver                                           *
 * *****************************************************************************/
#pragma once

#include <cbk/hal/io.h>
#include <types.h>

namespace trunk::drivers::serial
{
    inline constexpr WORD SERIAL_COM1_BASE        = 0x3F8;
    inline constexpr WORD SERIAL_REG_DATA         = SERIAL_COM1_BASE + 0;
    inline constexpr WORD SERIAL_REG_INT_ENABLE   = SERIAL_COM1_BASE + 1;
    inline constexpr WORD SERIAL_REG_FIFO         = SERIAL_COM1_BASE + 2;
    inline constexpr WORD SERIAL_REG_LINE_CTRL    = SERIAL_COM1_BASE + 3;
    inline constexpr WORD SERIAL_REG_MODEM_CTRL   = SERIAL_COM1_BASE + 4;
    inline constexpr WORD SERIAL_REG_LINE_STATUS  = SERIAL_COM1_BASE + 5;
    inline constexpr WORD SERIAL_REG_MODEM_STATUS = SERIAL_COM1_BASE + 6;

    inline constexpr WORD SERIAL_BAUD_115200_LO = 0x01;
    inline constexpr WORD SERIAL_BAUD_115200_HI = 0x00;

    inline constexpr BYTE SERIAL_LCR_8N1  = 0x03;
    inline constexpr BYTE SERIAL_LCR_DLAB = 0x80;

    inline constexpr BYTE SERIAL_FCR_ENABLE     = 0x01;
    inline constexpr BYTE SERIAL_FCR_CLEAR_RX   = 0x02;
    inline constexpr BYTE SERIAL_FCR_CLEAR_TX   = 0x04;
    inline constexpr BYTE SERIAL_FCR_TRIGGER_14 = 0xC0;

    inline constexpr BYTE SERIAL_LSR_TX_EMPTY   = 0x20;
    inline constexpr BYTE SERIAL_LSR_DATA_READY = 0x01;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : SerialInit                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Initialise COM1 at 115200 baud, 8N1, FIFO enabled                 *
     * *****************************************************************************/
    VOID SerialInit() noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : SerialPutChar                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write one character to COM1.                                      *
     * *****************************************************************************/
    VOID SerialPutChar(char c) noexcept;

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : SerialPuts                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a null-terminated string to COM1.                           *
     * *****************************************************************************/
    VOID SerialPuts(const char *s) noexcept;

} // namespace trunk::drivers::serial