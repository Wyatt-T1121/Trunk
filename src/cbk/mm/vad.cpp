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

#define NODE_HEIGHT(n) ((n) ? (n)->height : 0)
#define MAX_HEIGHT(a, b) ((a) > (b) ? (a) : (b))

namespace cbk::mem
{
    namespace
    {
        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MiVadUpdateHeight                                                   *
         * DATE    : 2026                                                                *
         * PURPOSE : Re-calculates and caches the max depth of a node                    *
         ********************************************************************************/
        INLINE VOID
        MiVadUpdateHeight(PMMVAD node) noexcept
        {
            if (node != nullptr)
                node->height =
                    MAX_HEIGHT(NODE_HEIGHT(node->left_child), NODE_HEIGHT(node->right_child)) + 1;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MiVadGetBalance                                                     *
         * DATE    : 2026                                                                *
         * PURPOSE : Computes balance difference (left height - right height)            *
         ********************************************************************************/
        NO_DISCARD INLINE LONG
        MiVadGetBalance(PMMVAD node) noexcept
        {
            return node ? (NODE_HEIGHT(node->left_child) - NODE_HEIGHT(node->right_child)) : 0;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MiVadRotateLeft                                                     *
         * DATE    : 2026                                                                *
         * PURPOSE : Single left rotation for right-heavy tree                           *
         ********************************************************************************/
        NO_DISCARD INLINE PMMVAD
        MiVadRotateLeft(PMMVAD x) noexcept
        {
            PMMVAD y       = x->right_child;
            x->right_child = y->left_child;

            if (y->left_child != nullptr)
                y->left_child->parent = x;

            y->parent     = x->parent;
            y->left_child = x;
            x->parent     = y;

            MiVadUpdateHeight(x);
            MiVadUpdateHeight(y);
            return y;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MiVadRotateRight                                                    *
         * DATE    : 2026                                                                *
         * PURPOSE : Single right rotation for left-heavy tree                           *
         ********************************************************************************/
        NO_DISCARD INLINE PMMVAD
        MiVadRotateRight(PMMVAD y) noexcept
        {
            PMMVAD x      = y->left_child;
            y->left_child = x->right_child;

            if (x->right_child != nullptr)
                x->right_child->parent = y;

            x->parent      = y->parent;
            x->right_child = y;
            y->parent      = x;

            MiVadUpdateHeight(y);
            MiVadUpdateHeight(x);
            return x;
        }

        /* *******************************************************************************
         * AUTHOR  : Trollycat                                                           *
         * FUNC    : MiVadRebalanceTree                                                  *
         * DATE    : 2026                                                                *
         * PURPOSE : Upward chains to correct AVL balance                                *
         ********************************************************************************/
        VOID
        MiVadRebalanceTree(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept
        {
            while (node != nullptr) {
                MiVadUpdateHeight(node);
                LONG balance = MiVadGetBalance(node);

                if (balance > 1) {
                    if (MiVadGetBalance(node->left_child) < 0)
                        node->left_child = MiVadRotateLeft(node->left_child);

                    PMMVAD parent       = node->parent;
                    PMMVAD balanced_sub = MiVadRotateRight(node);

                    if (parent == nullptr)
                        space->vad_root = balanced_sub;
                    else if (parent->left_child == node)
                        parent->left_child = balanced_sub;
                    else
                        parent->right_child = balanced_sub;

                    node = parent;
                    continue;
                }

                if (balance < -1) {
                    if (MiVadGetBalance(node->right_child) > 0)
                        node->right_child = MiVadRotateRight(node->right_child);

                    PMMVAD parent       = node->parent;
                    PMMVAD balanced_sub = MiVadRotateLeft(node);

                    if (parent == nullptr)
                        space->vad_root = balanced_sub;
                    else if (parent->left_child == node)
                        parent->left_child = balanced_sub;
                    else
                        parent->right_child = balanced_sub;

                    node = parent;
                    continue;
                }

                node = node->parent;
            }
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiVadFindMinimum                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Finds the VAD node with the lowest address in a sub-tree           *
         ********************************************************************************/
        NO_DISCARD PMMVAD
        MiVadFindMinimum(PMMVAD node) noexcept
        {
            if (node == nullptr)
                return nullptr;

            while (node->left_child != nullptr)
                node = node->left_child;

            return node;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiVadGetNextNode                                                   *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Finds the next node in memory order                                *
         ********************************************************************************/
        NO_DISCARD PMMVAD
        MiVadGetNextNode(PMMVAD node) noexcept
        {
            if (node == nullptr)
                return nullptr;
            if (node->right_child != nullptr)
                return MiVadFindMinimum(node->right_child);

            PMMVAD p = node->parent;

            while (p != nullptr && node == p->right_child) {
                node = p;
                p    = p->parent;
            }

            return p;
        }

        /* *******************************************************************************
         *  AUTHOR  : Trollycat                                                          *
         *  FUNC    : MiVadLinkChildNode                                                 *
         *  DATE    : 2026                                                               *
         *  PURPOSE : Link a child leaf to its parent                                    *
         ********************************************************************************/
        VOID
        MiVadLinkChildNode(PMMVAD parent, PMMVAD *child_slot, PMMVAD new_node) noexcept
        {
            *child_slot           = new_node;
            new_node->parent      = parent;
            new_node->left_child  = nullptr;
            new_node->right_child = nullptr;
        }

    } // namespace

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
                        ULONG protect) noexcept
    {
        if (blank_node == nullptr)
            return nullptr;

        blank_node->starting_vpn   = starting_vpn;
        blank_node->ending_vpn     = starting_vpn + page_count - 1;
        blank_node->left_child     = nullptr;
        blank_node->right_child    = nullptr;
        blank_node->parent         = nullptr;
        blank_node->balance        = 0;
        blank_node->backing_object = nullptr;

        blank_node->u.long_flags       = 0;
        blank_node->u.flags.protection = protect & 0x1F;
        blank_node->u.flags.vad_type   = VAD_PRIVATE;
        blank_node->u.flags.is_private = TRUE;

        return blank_node;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmVadFindNode                                                      *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree to see if a vpn exists inside a range               *
     ********************************************************************************/
    NO_DISCARD PMMVAD
    MmVadFindNode(PMM_ADDRESS_SPACE space, QWORD vpn) noexcept
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
     *  FUNC    : MmVadInsertNode                                                    *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Places a new block into the binary tree                            *
     ********************************************************************************/
    NO_DISCARD CBKSTATUS
    MmVadInsertNode(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept
    {
        if (space == nullptr || node == nullptr)
            return STATUS_INVALID_PARAMETER;

        if (space->vad_root == nullptr) {
            MiVadLinkChildNode(nullptr, &space->vad_root, node);
            return STATUS_SUCCESS;
        }

        PMMVAD current = space->vad_root;
        while (true) {

            if (node->ending_vpn < current->starting_vpn) {

                if (current->left_child == nullptr) {
                    MiVadLinkChildNode(current, &current->left_child, node);
                    break;
                }

                current = current->left_child;

            } else if (node->starting_vpn > current->ending_vpn) {

                if (current->right_child == nullptr) {
                    MiVadLinkChildNode(current, &current->right_child, node);
                    break;
                }

                current = current->right_child;

            } else
                return STATUS_CONFLICTING_ADDRESSES;
        }

        MiVadRebalanceTree(space, node->parent);
        return STATUS_SUCCESS;
    }

    /* *******************************************************************************
     * AUTHOR  : Trollycat                                                           *
     * FUNC    : MmVadDeleteNode                                                     *
     * DATE    : 2026                                                                *
     * PURPOSE : Removes a node from the  tree and rebalances                        *
     ********************************************************************************/
    VOID
    MmVadDeleteNode(PMM_ADDRESS_SPACE space, PMMVAD node) noexcept
    {
        if (space == nullptr || node == nullptr)
            return;

        PMMVAD rebalance_start = nullptr;

        if (node->left_child == nullptr || node->right_child == nullptr) {
            PMMVAD child    = (node->left_child != nullptr) ? node->left_child : node->right_child;
            rebalance_start = node->parent;

            if (child != nullptr)
                child->parent = node->parent;

            if (node->parent == nullptr)
                space->vad_root = child;
            else if (node->parent->left_child == node)
                node->parent->left_child = child;
            else
                node->parent->right_child = child;
        } else {
            PMMVAD successor = MiVadFindMinimum(node->right_child);
            rebalance_start  = successor->parent;

            if (successor->parent->left_child == successor)
                successor->parent->left_child = successor->right_child;
            else
                successor->parent->right_child = successor->right_child;

            if (successor->right_child != nullptr)
                successor->right_child->parent = successor->parent;

            if (rebalance_start == node)
                rebalance_start = successor;

            successor->parent      = node->parent;
            successor->left_child  = node->left_child;
            successor->right_child = node->right_child;
            successor->height      = node->height;

            if (node->left_child != nullptr)
                node->left_child->parent = successor;
            if (node->right_child != nullptr)
                node->right_child->parent = successor;

            if (node->parent == nullptr)
                space->vad_root = successor;
            else if (node->parent->left_child == node)
                node->parent->left_child = successor;
            else
                node->parent->right_child = successor;
        }

        if (rebalance_start != nullptr)
            MiVadRebalanceTree(space, rebalance_start);

        node->left_child  = nullptr;
        node->right_child = nullptr;
        node->parent      = nullptr;
    }

    /* *******************************************************************************
     *  AUTHOR  : Trollycat                                                          *
     *  FUNC    : MmVadFindFreeGap                                                   *
     *  DATE    : 2026                                                               *
     *  PURPOSE : Walks the tree looking for an empty hole between nodes             *
     ********************************************************************************/
    NO_DISCARD QWORD
    MmVadFindFreeGap(PMM_ADDRESS_SPACE space, SIZE_T page_cnt, BOOL top_down) noexcept
    {
        if (space == nullptr || space->vad_root == nullptr)
            return space->lowest_addr >> 12;

        QWORD current_vpn_frontier = space->lowest_addr >> 12;
        PMMVAD current             = MiVadFindMinimum(space->vad_root);

        while (current != nullptr) {

            if (current->starting_vpn > current_vpn_frontier) {
                SIZE_T gap_size = current->starting_vpn - current_vpn_frontier;
                if (gap_size >= page_cnt)
                    return current_vpn_frontier;
            }

            if (current->ending_vpn == 0xFFFFFFFFFFFFFFFF)
                return 0;

            current_vpn_frontier = current->ending_vpn + 1;
            current              = MiVadGetNextNode(current);
        }

        QWORD highest_vpn_boundary = space->highest_addr >> 12;
        if (highest_vpn_boundary > current_vpn_frontier)
            if ((highest_vpn_boundary - current_vpn_frontier) >= page_cnt)
                return current_vpn_frontier;

        return 0;
    }

} // namespace cbk::mem