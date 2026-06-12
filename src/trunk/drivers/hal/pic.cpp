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
 *  PURPOSE : Implements the functionality for the Dual 8259 PIC chips           *
 ********************************************************************************/
#include <trunk/drivers/hal/pic.h>
#include <trunk/asi/io.h>

namespace trunk::drivers::pic
{

    static void get_pic_line_properties(u8 &irq, u16 &out_port) noexcept
    {
        if (irq < 8)
        {
            out_port = PIC1_DATA;
        }
        else
        {
            out_port = PIC2_DATA;
            irq -= 8;
        }
    }

    void pic_init() noexcept
    {
        u8 mask1 = asi::inb(PIC1_DATA);
        u8 mask2 = asi::inb(PIC2_DATA);

        // ICW1
        asi::outb(PIC1_COMMAND, ICW1_INIT);
        asi::outb(PIC2_COMMAND, ICW1_INIT);

        // ICW2
        asi::outb(PIC1_DATA, PIC1_OFFSET);
        asi::outb(PIC2_DATA, PIC2_OFFSET);

        // ICW3
        asi::outb(PIC1_DATA, 0x04);
        asi::outb(PIC2_DATA, 0x02);

        // ICW4
        asi::outb(PIC1_DATA, ICW4_8086);
        asi::outb(PIC2_DATA, ICW4_8086);

        // clear mask register
        asi::outb(PIC1_DATA, 0x00);
        asi::outb(PIC2_DATA, 0x00);
    }

    void irq_ack(u8 irq) noexcept
    {
        if (irq >= 8)
            asi::outb(PIC2_COMMAND, PIC_EOI);
        asi::outb(PIC1_COMMAND, PIC_EOI);
    }

    void pic_mask(u8 irq) noexcept
    {
        u16 port = 0;
        get_pic_line_properties(irq, port);
        u8 value = asi::inb(port) | (1 << irq);
        asi::outb(port, value);
    }

    void pic_unmask(u8 irq) noexcept
    {
        u16 port = 0;
        get_pic_line_properties(irq, port);
        u8 value = asi::inb(port) & ~(1 << irq);
        asi::outb(port, value);
    }

    void pic_disable() noexcept
    {
        asi::outb(PIC1_DATA, 0xFF);
        asi::outb(PIC2_DATA, 0xFF);
    }

}; // namespace trunk::drivers::pic