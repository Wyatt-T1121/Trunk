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
 *  FILE    : Result.h                                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Rust style Result<> for error handling.                            *
 *            Safer, better, and actually usuable compared to c++ exceptions     *
 *                                                                               *
 ********************************************************************************/
#pragma once

#include <trunk/Types.h>

namespace tklib
{

    template <typename T>
    struct OkTag
    {
        T value;
    };

    template <typename E>
    struct ErrTag
    {
        E value;
    };

    template <typename T>
    OkTag<T> Ok(T value) { return OkTag<T>{value}; }

    template <typename E>
    ErrTag<E> Err(E value) { return ErrTag<E>{value}; }

    template <typename T, typename E>
    class Result
    {
    public:
        Result(OkTag<T> ok) : m_is_ok(true)
        {
            new (m_storage) T(ok.value);
        }

        Result(ErrTag<E> err) : m_is_ok(false)
        {
            new (m_storage) E(err.value);
        }

        Result(const Result &) = delete;
        Result &operator=(const Result &) = delete;
        Result(Result &&) = delete;
        Result &operator=(Result &&) = delete;

        ~Result()
        {
            if (m_is_ok)
                reinterpret_cast<T *>(m_storage)->~T();
            else
                reinterpret_cast<E *>(m_storage)->~E();
        }

        [[nodiscard]] bool is_ok() const noexcept { return m_is_ok; }
        [[nodiscard]] bool is_err() const noexcept { return !m_is_ok; }

        T &value()
        {
            if (!m_is_ok)
                for (;;)
                    asm volatile("cli; hlt");
            return *reinterpret_cast<T *>(m_storage);
        }

        const T &value() const
        {
            if (!m_is_ok)
                for (;;)
                    asm volatile("cli; hlt");
            return *reinterpret_cast<const T *>(m_storage);
        }

        E &error()
        {
            if (m_is_ok)
                for (;;)
                    asm volatile("cli; hlt");
            return *reinterpret_cast<E *>(m_storage);
        }

        const E &error() const
        {
            if (m_is_ok)
                for (;;)
                    asm volatile("cli; hlt");
            return *reinterpret_cast<const E *>(m_storage);
        }

    private:
        static constexpr usize SIZE = sizeof(T) > sizeof(E) ? sizeof(T) : sizeof(E);
        static constexpr usize ALIGN = alignof(T) > alignof(E) ? alignof(T) : alignof(E);

        alignas(ALIGN) unsigned char m_storage[SIZE];
        bool m_is_ok;
    };

} // namespace tklib

template <typename T, typename E>
using Result = tklib::Result<T, E>;

template <typename T>
auto Ok(T value) { return tklib::OkTag<T>{value}; }

template <typename E>
auto Err(E value) { return tklib::ErrTag<E>{value}; }