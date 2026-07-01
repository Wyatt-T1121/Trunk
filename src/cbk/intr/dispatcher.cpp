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
#include <cbk/intr/dispatcher.h>
#include <cbk/intr/interrupts.h>

#include <drivers/hal/pic.h>
#include <drivers/serial/serial.h>

namespace cbk::interrupts
{
    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : KeInterruptDispatchHandler                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Takes the interrupt from trap and dispatches It                    *
     ********************************************************************************/
    EXTERN_C VOID
    KeInterruptDispatchHandler(InterruptFrame *frame) noexcept
    {
        const BYTE vector = static_cast<BYTE>(frame->vector_number);

        if (vector >= drivers::pic::PIC1_OFFSET && vector < (drivers::pic::PIC1_OFFSET + 16)) {
            const BYTE irq = vector - drivers::pic::PIC1_OFFSET;
            drivers::pic::IrqAck(irq);
        }

        KeExecuteInterruptHandler(vector, frame);
    }
} // namespace cbk::interrupts