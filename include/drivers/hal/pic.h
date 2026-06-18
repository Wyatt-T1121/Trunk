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
    inline constexpr static u8 PIC1         = 0x20;
    inline constexpr static u8 PIC2         = 0xA0;

    inline constexpr static u8 PIC1_COMMAND = PIC1;
    inline constexpr static u8 PIC1_DATA    = PIC1 + 1;
    inline constexpr static u8 PIC2_COMMAND = PIC2;
    inline constexpr static u8 PIC2_DATA    = PIC2 + 1;
    
    inline constexpr static u8 PIC_EOI      = 0x20;

    inline constexpr static u8 PIC1_OFFSET  = 0x20;
    inline constexpr static u8 PIC2_OFFSET  = 0x28;

    inline constexpr static u8 ICW1_INIT    = 0x11;
    inline constexpr static u8 ICW4_8086    = 0x01;

    // clang-format on

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initialize the PIC driver                                          *
     ********************************************************************************/
    void pic_init() noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : irq_ack                                                            *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Signals that an interrupt is being processed                       *
     ********************************************************************************/
    void irq_ack(u8 irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_mask                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Mask an IRQ (interrupt request)                                    *
     ********************************************************************************/
    void pic_mask(u8 irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_mask                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Unmask an IRQ (interrupt request)                                  *
     ********************************************************************************/
    void pic_unmask(u8 irq) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : pic_disable                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Disable the PIC driver                                             *
     ********************************************************************************/
    void pic_disable() noexcept;
}; // namespace trunk::drivers::pic