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
 *  MODULE  : Standard library utility                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Replacement for <utility> header (std)                             *
 ********************************************************************************/
#pragma once

#include <types.h>

namespace trunk
{
    template <typename T>
    struct remove_reference
    {
        using type = T;
    };
    template <typename T>
    struct remove_reference<T &>
    {
        using type = T;
    };
    template <typename T>
    struct remove_reference<T &&>
    {
        using type = T;
    };

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : forward                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Perfect forward t as T.                                           *
     *            Replaces std::forward - no <utility> needed.                      *
     *                                                                              *
     * *****************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr T &&
    forward(typename remove_reference<T>::type &t) noexcept
    {
        return static_cast<T &&>(t);
    }

    template <typename T>
    [[nodiscard]] constexpr T &&
    forward(typename remove_reference<T>::type &&t) noexcept
    {
        return static_cast<T &&>(t);
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : move                                                              *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Cast t to rvalue reference.                                       *
     *            Replaces std::move - no <utility> needed.                         *
     *                                                                              *
     * *****************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr typename remove_reference<T>::type &&
    move(T &&t) noexcept
    {
        return static_cast<typename remove_reference<T>::type &&>(t);
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : exchange                                                          *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Replace obj with new_val, return old value.                       *
     *            Useful in move constructors and swap implementations.             *
     *                                                                              *
     * *****************************************************************************/
    template <typename T, typename U = T>
    constexpr T exchange(T &obj, U &&new_val) noexcept
    {
        T old = move(obj);
        obj = forward<U>(new_val);
        return old;
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : addressof                                                         *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Take address of ref bypassing overloaded operator&.               *
     *                                                                              *
     * *****************************************************************************/
    template <typename T>
    [[nodiscard]] constexpr T *addressof(T &ref) noexcept
    {
        return __builtin_addressof(ref);
    }

    /* ******************************************************************************
     *                                                                              *
     *  AUTHOR  : Trollycat                                                         *
     *  FUNC    : declval                                                           *
     *  DATE    : 2026                                                              *
     *  PURPOSE : Produce T&& in unevaluated contexts without constructing T.       *
     *                                                                              *
     * *****************************************************************************/
    template <typename T>
    typename remove_reference<T>::type &&declval() noexcept;

    template <typename A, typename B>
    struct is_same
    {
        static constexpr bool value = false;
    };
    template <typename A>
    struct is_same<A, A>
    {
        static constexpr bool value = true;
    };

    template <typename A, typename B>
    inline constexpr bool is_same_v = is_same<A, B>::value;

    template <typename T>
    struct is_integral
    {
        static constexpr bool value = false;
    };
    template <>
    struct is_integral<u8>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<u16>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<u32>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<u64>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<i8>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<i16>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<i32>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<i64>
    {
        static constexpr bool value = true;
    };
    template <>
    struct is_integral<bool>
    {
        static constexpr bool value = true;
    };

    template <typename T>
    inline constexpr bool is_integral_v = is_integral<T>::value;

    template <typename T>
    struct is_pointer
    {
        static constexpr bool value = false;
    };
    template <typename T>
    struct is_pointer<T *>
    {
        static constexpr bool value = true;
    };
    template <typename T>
    struct is_pointer<T *const>
    {
        static constexpr bool value = true;
    };

    template <typename T>
    inline constexpr bool is_pointer_v = is_pointer<T>::value;

    template <typename T>
    struct is_const
    {
        static constexpr bool value = false;
    };
    template <typename T>
    struct is_const<const T>
    {
        static constexpr bool value = true;
    };

    template <typename T>
    inline constexpr bool is_const_v = is_const<T>::value;

    template <bool Cond, typename A, typename B>
    struct conditional
    {
        using type = A;
    };
    template <typename A, typename B>
    struct conditional<false, A, B>
    {
        using type = B;
    };

    template <bool Cond, typename A, typename B>
    using conditional_t = typename conditional<Cond, A, B>::type;

    template <bool Cond, typename T = void>
    struct enable_if
    {
    };
    template <typename T>
    struct enable_if<true, T>
    {
        using type = T;
    };

    template <bool Cond, typename T = void>
    using enable_if_t = typename enable_if<Cond, T>::type;
} // namespace trunk