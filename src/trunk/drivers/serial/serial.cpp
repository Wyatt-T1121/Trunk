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
#include <trunk/drivers/serial/serial.h>
#include <trunk/drivers/hal/pic.h>

#include <trunk/tros/interrupts/interrupts.h>

namespace trunk::drivers::serial
{
    namespace
    {
        /* ******************************************************************************
         *  AUTHOR  : Trollycat                                                         *
         *  FUNC    : serial_is_transmit_ready                                          *
         *  DATE    : 2026                                                              *
         *  PURPOSE : Return true if the transmit buffer is empty.                      *
         * *****************************************************************************/
        [[nodiscard]] bool serial_is_transmit_ready() noexcept
        {
            return (asi::inb(SERIAL_REG_LINE_STATUS) & SERIAL_LSR_TX_EMPTY) != 0;
        }
    } // namespace

    /* ******************************************************************************
     * AUTHOR  : Trollycat                                                          *
     * FUNC    : serial_interrupt_handler                                           *
     * DATE    : 2026                                                               *
     * PURPOSE : Handles incoming characters from the serial port asynchronously    *
     * *****************************************************************************/
    void serial_interrupt_handler([[maybe_unused]] interrupts::InterruptFrame *frame, [[maybe_unused]] void *context) noexcept
    {
        while (asi::inb(SERIAL_REG_LINE_STATUS) & 0x01)
        {
            u8 incoming_byte = asi::inb(SERIAL_REG_DATA);
            serial_putchar(static_cast<char>(incoming_byte));
        }
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_init                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Initialise COM1 at 115200 baud, 8N1, FIFO enabled                 *
     * *****************************************************************************/
    void serial_init() noexcept
    {
        asi::outb(SERIAL_REG_INT_ENABLE, 0x00);
        asi::outb(SERIAL_REG_LINE_CTRL, SERIAL_LCR_DLAB);
        asi::outb(SERIAL_REG_DATA, SERIAL_BAUD_115200_LO);
        asi::outb(SERIAL_REG_INT_ENABLE, SERIAL_BAUD_115200_HI);
        asi::outb(SERIAL_REG_LINE_CTRL, SERIAL_LCR_8N1);

        asi::outb(SERIAL_REG_FIFO,
                  SERIAL_FCR_ENABLE |
                      SERIAL_FCR_CLEAR_RX |
                      SERIAL_FCR_CLEAR_TX |
                      SERIAL_FCR_TRIGGER_14);

        asi::outb(SERIAL_REG_MODEM_CTRL, 0x0B);

        asi::outb(SERIAL_REG_INT_ENABLE, 0x01);

        interrupts::register_interrupt_handler(36, serial_interrupt_handler, nullptr);
        drivers::pic::pic_unmask(4);
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_putchar                                                    *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write one character to COM1.                                      *
     * *****************************************************************************/
    void serial_putchar(char c) noexcept
    {
        if (c == '\n')
            serial_putchar('\r');

        while (!serial_is_transmit_ready())
        {
        }

        // clang-format off
        #ifdef TRUNK_DEBUG
                asi::outb(SERIAL_REG_DATA, static_cast<u8>(c));
        #else
                (void)c;
        #endif
        // clang-format on
    }

    /* ******************************************************************************
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : serial_puts                                                       *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Write a null-terminated string to COM1.                           *
     * *****************************************************************************/
    void serial_puts(const char *s) noexcept
    {
        while (*s)
            serial_putchar(*s++);
    }

} // namespace trunk::drivers::serial