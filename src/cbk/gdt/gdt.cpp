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
 *  MODULE  : Global Descriptor Table                                            *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Defines and initalizes the Global Descriptor Table.                *
 ********************************************************************************/
#include <cbk/gdt/gdt.h>
#include <cbk/gdt/tss.h>

#include <tklib/string.h>

namespace trunk::gdt
{
    static GdtLayout gdt;
    static GdtPointer gdt_pointer;

    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : write_segment                                                      *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Utility to write a GDT segment                                     *
         ********************************************************************************/
        static VOID write_segment(GdtEntry *entry, WORD limit, DWORD base, BYTE access,
                                  BYTE flags) noexcept
        {
            entry->limit_low        = limit & 0xFFFF;
            entry->base_low         = base & 0xFFFF;
            entry->base_middle      = (base >> 16) & 0xFF;
            entry->access           = access;
            entry->flags_limit_high = ((flags & 0xF0)) | ((limit >> 16) & 0xF);
            entry->base_high        = (base >> 24) & 0xFF;
        }
    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_create_entries                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Creates standard GDT entries                                       *
     ********************************************************************************/
    VOID gdt_create_entries() noexcept
    {
        tklib::memset(&gdt, 0, sizeof(GdtLayout));

        write_segment(&gdt.kernel_code, 0, 0,
                      GDT_PRESENT | GDT_RING0 | GDT_SYSTEM | GDT_EXECUTABLE | GDT_READ_WRITE,
                      GDT_LONG_MODE);
        write_segment(&gdt.kernel_data, 0, 0, GDT_PRESENT | GDT_RING0 | GDT_SYSTEM | GDT_READ_WRITE,
                      0);
        write_segment(&gdt.user_code, 0, 0,
                      GDT_PRESENT | GDT_RING3 | GDT_SYSTEM | GDT_EXECUTABLE | GDT_READ_WRITE,
                      GDT_LONG_MODE);
        write_segment(&gdt.user_data, 0, 0, GDT_PRESENT | GDT_RING3 | GDT_SYSTEM | GDT_READ_WRITE,
                      0);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : GdtInstallTss                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Installs the TSS                                                   *
     ********************************************************************************/
    NO_DISCARD WORD GdtInstallTss(const Tss *tss_ptr) noexcept
    {
        QWORD base = reinterpret_cast<QWORD>(tss_ptr);
        WORD limit = sizeof(Tss) - 1;

        TssDescriptor *desc = &gdt.tss_desc;

        desc->limit_low   = limit & 0xFFFF;
        desc->base_low    = base & 0xFFFF;
        desc->base_middle = (base >> 16) & 0xFF;

        desc->type = 0x9;
        desc->zero = 0;
        desc->dpl  = 0;
        desc->p    = 1;

        desc->limit_high = (limit >> 16) & 0xF;
        desc->avl        = 0;
        desc->l          = 0;
        desc->db         = 0;
        desc->g          = 0;

        desc->base_high  = (base >> 24) & 0xFF;
        desc->base_upper = (base >> 32) & 0xFFFFFFFF;
        desc->reserved   = 0;

        return OFFSET_OF(GdtLayout, tss_desc);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : gdt_init                                                           *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes the global descriptor table subsystem                  *
     ********************************************************************************/
    VOID GdtInit() noexcept
    {
        gdt_create_entries();
        TssInit();

        WORD tss_selector = GdtInstallTss(&TssGet());

        gdt_pointer.limit = sizeof(GdtLayout) - 1;
        gdt_pointer.base  = reinterpret_cast<ULONG_PTR>(&gdt);

        GdtFlush(reinterpret_cast<ULONG_PTR>(&gdt_pointer));

        asm volatile("ltr %0" ::"r"(tss_selector));
    }

} // namespace trunk::gdt