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
 *  MODULE  : Programmable Interrupt Controller                                  *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Driver for the 8259 PIC chips                                      *
 ********************************************************************************/
#pragma once

#include <types.h>

namespace trunk::drivers::pic
{

    // clang-format off
    inline constexpr static BYTE PIC1         = 0x20;
    inline constexpr static BYTE PIC2         = 0xA0;

    inline constexpr static BYTE PIC1_COMMAND = PIC1;
    inline constexpr static BYTE PIC1_DATA    = PIC1 + 1;
    inline constexpr static BYTE PIC2_COMMAND = PIC2;
    inline constexpr static BYTE PIC2_DATA    = PIC2 + 1;
    
    inline constexpr static BYTE PIC_EOI      = 0x20;

    inline constexpr static BYTE PIC1_OFFSET  = 0x20;
    inline constexpr static BYTE PIC2_OFFSET  = 0x28;

    inline constexpr static BYTE ICW1_INIT    = 0x11;
    inline constexpr static BYTE ICW4_8086    = 0x01;

    // clang-format on

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PicInit                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the PIC driver                                          *
     ********************************************************************************/
    VOID PicInit() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IrqAck                                                             *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Signals that an interrupt is being processed                       *
     ********************************************************************************/
    VOID IrqAck(BYTE irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PicMask                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Mask an IRQ (interrupt request)                                    *
     ********************************************************************************/
    VOID PicMask(BYTE irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PicUnmask                                                          *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmask an IRQ (interrupt request)                                  *
     ********************************************************************************/
    VOID PicUnmask(BYTE irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : PicDisable                                                         *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Disable the PIC driver                                             *
     ********************************************************************************/
    VOID PicDisable() noexcept;
}; // namespace trunk::drivers::pic