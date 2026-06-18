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
 *  MODULE  : Programmable Interrupt Controller                                  *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Driver for the 8259 PIC chips                                      *
 ********************************************************************************/
#include <cbk/hal/io.h>
#include <drivers/hal/pic.h>

namespace trunk::drivers::pic
{
    namespace
    {
        static void get_pic_line_properties(u8 &irq, u16 &out_port) noexcept
        {
            if (irq < 8)
                out_port = PIC1_DATA;
            else {
                out_port  = PIC2_DATA;
                irq      -= 8;
            }
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the PIC driver                                          *
     ********************************************************************************/
    void pic_init() noexcept
    {
        hal::outb(PIC1_COMMAND, ICW1_INIT);
        hal::outb(PIC2_COMMAND, ICW1_INIT);

        hal::outb(PIC1_DATA, PIC1_OFFSET);
        hal::outb(PIC2_DATA, PIC2_OFFSET);

        hal::outb(PIC1_DATA, 0x04);
        hal::outb(PIC2_DATA, 0x02);

        hal::outb(PIC1_DATA, ICW4_8086);
        hal::outb(PIC2_DATA, ICW4_8086);

        hal::outb(PIC1_DATA, 0x00);
        hal::outb(PIC2_DATA, 0x00);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : irq_ack                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Signals that an interrupt is being processed                       *
     ********************************************************************************/
    void irq_ack(u8 irq) noexcept
    {
        if (irq >= 8)
            hal::outb(PIC2_COMMAND, PIC_EOI);
        hal::outb(PIC1_COMMAND, PIC_EOI);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_mask                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Mask an IRQ (interrupt request)                                    *
     ********************************************************************************/
    void pic_mask(u8 irq) noexcept
    {
        u16 port = 0;
        get_pic_line_properties(irq, port);
        u8 value = hal::inb(port) | (1 << irq);
        hal::outb(port, value);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_mask                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmask an IRQ (interrupt request)                                  *
     ********************************************************************************/
    void pic_unmask(u8 irq) noexcept
    {
        u16 port = 0;
        get_pic_line_properties(irq, port);
        u8 value = hal::inb(port) & ~(1 << irq);
        hal::outb(port, value);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_disable                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Disable the PIC driver                                             *
     ********************************************************************************/
    void pic_disable() noexcept
    {
        hal::outb(PIC1_DATA, 0xFF);
        hal::outb(PIC2_DATA, 0xFF);
    }

}; // namespace trunk::drivers::pic