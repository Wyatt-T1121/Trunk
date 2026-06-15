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
 *  MODULE  : Interrupt subsystem                                                *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Interrupt dispatcher                                               *
 ********************************************************************************/
#include <trunk/tros/interrupts/dispatcher.h>
#include <trunk/tros/interrupts/interrupts.h>

#include <trunk/drivers/serial/serial.h>
#include <trunk/drivers/hal/pic.h>

namespace trunk::interrupts
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : kinterrupt_dispatcher                                              *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Takes the interrupt from trap and dispatches It                    *
     ********************************************************************************/
    extern "C" void kinterrupt_dispatcher(InterruptFrame *frame) noexcept
    {
        const u8 vector = static_cast<u8>(frame->vector_number);

        execute_interrupt_handler(vector, frame);

        if (vector >= drivers::pic::PIC1_OFFSET && vector < (drivers::pic::PIC1_OFFSET + 16))
        {
            const u8 irq = vector - drivers::pic::PIC1_OFFSET;
            drivers::pic::irq_ack(irq);
        }
    }
} // namespace trunk::interrupts