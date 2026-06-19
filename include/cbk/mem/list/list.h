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
 *  MODULE  : ListEntry                                                          *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores the ListEntry structure for linked-lists                    *
 ********************************************************************************/
#pragma once

#include <cbk/mem/types/mmtypes.h>
#include <types.h>

#ifndef CONTAINING_RECORD
#define CONTAINING_RECORD(address, type, field)                                                    \
    ((type *)((CHAR *)(address) - (SIZE_T)(&((type *)0)->field)))
#endif

namespace trunk::mem
{
    struct ListEntry
    {
        ListEntry *flink;
        ListEntry *blink;
    };

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InitializeListHead                                                 *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Initializes a list head to point to itself                         *
     ********************************************************************************/
    inline VOID InitializeListHead(PLIST_ENTRY list_head) noexcept
    {
        list_head->flink = list_head;
        list_head->blink = list_head;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : IsListEmpty                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Checks If a doubly-linked list is empty                            *
     ********************************************************************************/
    NO_DISCARD inline BOOL IsListEmpty(PLIST_ENTRY list_head) noexcept
    {
        return (BOOL)(list_head->flink == list_head);
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InsertHeadList                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Inserts an entry at the beginning of the list                      *
     ********************************************************************************/
    inline VOID InsertHeadList(PLIST_ENTRY list_head, PLIST_ENTRY entry) noexcept
    {
        PLIST_ENTRY flink = list_head->flink;
        entry->flink      = flink;
        entry->blink      = list_head;
        flink->blink      = entry;
        list_head->flink  = entry;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : InsertTailList                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Inserts an entry at the beginning of the list                      *
     ********************************************************************************/
    inline VOID InsertTailList(PLIST_ENTRY list_head, PLIST_ENTRY entry) noexcept
    {
        PLIST_ENTRY blink = list_head->blink;
        entry->flink      = list_head;
        entry->blink      = blink;
        blink->flink      = entry;
        list_head->blink  = entry;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : RemoveEntryList                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Removes an entry from anywhere in the list in 0(1) time            *
     ********************************************************************************/
    inline VOID RemoveEntryList(PLIST_ENTRY entry) noexcept
    {
        PLIST_ENTRY flink = entry->flink;
        PLIST_ENTRY blink = entry->blink;
        blink->flink      = flink;
        flink->blink      = blink;
    }

} // namespace trunk::mem