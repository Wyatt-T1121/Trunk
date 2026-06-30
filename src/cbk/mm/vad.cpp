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
#include <cbk/mm/vad.h>

namespace cbk::mem
{
    namespace
    {
        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : VadFindMinimum                                                     *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Finds the VAD node with the lowest address in a sub-tree           *
         ********************************************************************************/
        NO_DISCARD PMMVAD VadFindMinimum(PMMVAD node) noexcept
        {
            if (node == nullptr)
                return nullptr;

            while (node->left_child != nullptr)
                node = node->left_child;

            return node;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : VadGetNextSuccessor                                                *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Finds the next node in memory order                                *
         ********************************************************************************/
        NO_DISCARD PMMVAD VadGetNextSuccessor(PMMVAD node) noexcept
        {
            if (node == nullptr)
                return nullptr;
            if (node->right_child != nullptr)
                return VadFindMinimum(node->right_child);

            PMMVAD p = node->parent;

            while (p != nullptr && node == p->right_child) {
                node = p;
                p    = p->parent;
            }

            return p;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : VadLinkChildNode                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Link a child leaf to its parent                                    *
         ********************************************************************************/
        VOID VadLinkChildNode(PMMVAD parent, PMMVAD *child_slot, PMMVAD new_node) noexcept
        {
            *child_slot           = new_node;
            new_node->parent      = parent;
            new_node->left_child  = nullptr;
            new_node->right_child = nullptr;
        }

    } // namespace

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadFindNode                                                        *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree to see if a vpn exists inside a range               *
     ********************************************************************************/
    NO_DISCARD PMMVAD VadFindNode(PMM_ADDRESS_SPACE space, QWORD vpn) noexcept
    {
        if (space == nullptr)
            return nullptr;

        PMMVAD current = space->vad_root;

        while (current != nullptr) {

            if (vpn >= current->starting_vpn && vpn <= current->ending_vpn)
                return current;

            if (vpn < current->starting_vpn)
                current = current->left_child;
            else
                current = current->right_child;
        }

        return nullptr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadInsertNode                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Places a new block into the binary tree                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS VadInsertNode(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept
    {
        if (space == nullptr || node == nullptr)
            return STATUS_INVALID_PARAMETER;

        if (space->vad_root == nullptr) {
            VadLinkChildNode(nullptr, &space->vad_root, node);
            return STATUS_SUCCESS;
        }

        PMMVAD current = space->vad_root;
        while (true) {
            if (node->starting_vpn < current->starting_vpn) {

                if (current->left_child == nullptr) {
                    VadLinkChildNode(current, &current->left_child, node);
                    break;
                }

                current = current->left_child;

            } else {

                if (current->right_child == nullptr) {
                    VadLinkChildNode(current, &current->right_child, node);
                    break;
                }

                current = current->right_child;
            }
        }

        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : VadFindFreeGap                                                     *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree looking for an empty hole between nodes             *
     ********************************************************************************/
    NO_DISCARD QWORD VadFindFreeGap(PMM_ADDRESS_SPACE space, SIZE_T page_cnt,
                                    BOOL top_down) noexcept
    {
        if (space == nullptr || space->vad_root == nullptr)
            return space->lowest_addr >> 12;

        QWORD current_vpn_frontier = space->lowest_addr >> 12;
        PMMVAD current             = VadFindMinimum(space->vad_root);

        while (current != nullptr) {

            if (current->starting_vpn > current_vpn_frontier) {
                SIZE_T gap_size = current->starting_vpn - current_vpn_frontier;
                if (gap_size >= page_cnt)
                    return current_vpn_frontier;
            }

            if (current->ending_vpn == 0xFFFFFFFFFFFFFFFF)
                return 0;

            current_vpn_frontier = current->ending_vpn + 1;
            current              = VadGetNextSuccessor(current);
        }

        QWORD highest_vpn_boundary = space->highest_addr >> 12;
        if (highest_vpn_boundary > current_vpn_frontier)
            if ((highest_vpn_boundary - current_vpn_frontier) >= page_cnt)
                return current_vpn_frontier;

        return 0;
    }

} // namespace cbk::mem