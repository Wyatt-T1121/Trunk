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
#include <drivers/hal/pic.h>
#include <drivers/serial/serial.h>

#include <cbk/interrupts/interrupts.h>

#include <macros.h>

namespace trunk::drivers::serial
{
    namespace
    {
        /* ******************************************************************************
         *  AUTHOR  : Trollycat                                                         *
         *  FUNC    : SerialIsTransmitReady                                             *
         *  DATE    : 2026                                                              *
         *  PURPOSE : Return true if the transmit buffer is empty.                      *
         * *****************************************************************************/
        NO_DISCARD BOOL SerialIsTransmitReady() noexcept
        {
            return (hal::InB(SERIAL_REG_LINE_STATUS) & SERIAL_LSR_TX_EMPTY) != 0;
        }

        /* ******************************************************************************
         * AUTHOR  : Trollycat                                                          *
         * FUNC    : SerialInterruptHandler                                             *
         * DATE    : 2026                                                               *
         * PURPOSE : Handles incoming characters from the serial port asynchronously    *
         * *****************************************************************************/
        void SerialInterruptHandler(MAYBE_UNUSED interrupts::InterruptFrame *frame,
                                    MAYBE_UNUSED void *context) noexcept
        {
            while (hal::InB(SERIAL_REG_LINE_STATUS) & 0x01) {
                BYTE incoming_byte = hal::InB(SERIAL_REG_DATA);
                SerialPutChar(static_cast<char>(incoming_byte));
            }
        }
    } // namespace

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : SerialInit                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Initialise COM1 at 115200 baud, 8N1, FIFO enabled                 *
     * *****************************************************************************/
    void SerialInit() noexcept
    {
        hal::OutB(SERIAL_REG_INT_ENABLE, 0x00);
        hal::OutB(SERIAL_REG_LINE_CTRL, SERIAL_LCR_DLAB);
        hal::OutB(SERIAL_REG_DATA, SERIAL_BAUD_115200_LO);
        hal::OutB(SERIAL_REG_INT_ENABLE, SERIAL_BAUD_115200_HI);
        hal::OutB(SERIAL_REG_LINE_CTRL, SERIAL_LCR_8N1);

        hal::OutB(SERIAL_REG_FIFO, SERIAL_FCR_ENABLE | SERIAL_FCR_CLEAR_RX | SERIAL_FCR_CLEAR_TX |
                                       SERIAL_FCR_TRIGGER_14);

        hal::OutB(SERIAL_REG_MODEM_CTRL, 0x0B);

        hal::OutB(SERIAL_REG_INT_ENABLE, 0x01);

        interrupts::RegisterInterruptHandler(36, SerialInterruptHandler, nullptr);
        drivers::pic::PicMask(4);
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : SerialPutChar                                                     *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write one character to COM1.                                      *
     * *****************************************************************************/
    void SerialPutChar(char c) noexcept
    {
        if (c == '\n')
            SerialPutChar('\r');

        while (!SerialIsTransmitReady()) {
        }

        // clang-format off
        #ifdef TRUNK_DEBUG
                hal::OutB(SERIAL_REG_DATA, static_cast<BYTE>(c));
        #else
                (void)c;
        #endif
        // clang-format on
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : SerialPuts                                                        *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a null-terminated string to COM1.                           *
     * *****************************************************************************/
    void SerialPuts(const char *s) noexcept
    {
        while (*s)
            SerialPutChar(*s++);
    }

} // namespace trunk::drivers::serial