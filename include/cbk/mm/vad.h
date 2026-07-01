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
 *  MODULE  : Virtual address descriptor                                         *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Binary search tree kept by the kernel to manage fake vmem          *
 ********************************************************************************/
#pragma once

#include <attributes.h>
#include <status.h>
#include <types.h>

#include <cbk/mm/mmdefs.h>

namespace cbk::mem
{
    enum VAD_TYPE : ULONG
    {
        VAD_NONE,
        VAD_DEVICE_PHYSICAL_MEMORY,
        VAD_IMAGE_MAP,
        VAD_PRIVATE,
    };

    struct MmVadFlags
    {
        ULONG protection : 5;
        ULONG vad_type : 3;
        ULONG is_private : 1;
        ULONG spare : 23;
    };

    struct MmVad
    {
        QWORD starting_vpn;
        QWORD ending_vpn;

        MmVad *left_child;
        MmVad *right_child;
        MmVad *parent;

        LONG height;

        union {
            ULONG long_flags;
            MM_VAD_FLAGS flags;
        } u;

        LONG balance;
        PVOID backing_object;
    };

    struct MmAddressSpace
    {
        PARCH_ASPACE hardware_map;
        PMMVAD vad_root;
        QWORD lowest_addr;
        QWORD highest_addr;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmVadInitializeNode                                                *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Creates a new VAD node                                             *
     ********************************************************************************/
    NO_DISCARD PMMVAD
    MmVadInitializeNode(PMMVAD blank_node,
                        QWORD starting_vpn,
                        SIZE_T page_count,
                        ULONG protect) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmVadFindNode                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree to see if a vpn exists inside a range               *
     ********************************************************************************/
    NO_DISCARD PMMVAD
    MmVadFindNode(PMM_ADDRESS_SPACE space, QWORD vpn) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmVadInsertNode                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Places a new block into the binary tree                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmVadInsertNode(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept;

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmVadDeleteNode                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Removes a node from the tree and rebalances                         *
     ********************************************************************************/
    VOID
    MmVadDeleteNode(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept;

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmVadFindFreeGap                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree looking for an empty hole between nodes             *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmVadFindFreeGap(PMM_ADDRESS_SPACE space, SIZE_T page_cnt, BOOL top_down) noexcept;

} // namespace cbk::mem