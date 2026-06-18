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
 *  MODULE  : Test framework                                                     *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Unit tests for tklib::ubuiltin                                     *
 ********************************************************************************/

#include <gtest/gtest.h>
#include <tklib/ubuiltin.h>

TEST(Ubuiltin, Move)
{
    int a = 42;
    int b = static_cast<int>(tklib::move(a));
    EXPECT_EQ(b, 42);

    int x = 10;
    int y = tklib::move(x);
    EXPECT_EQ(y, 10);
}

TEST(Ubuiltin, Exchange)
{
    int a   = 1;
    int old = tklib::exchange(a, 2);
    EXPECT_EQ(old, 1);
    EXPECT_EQ(a, 2);

    int x = 99;
    old   = tklib::exchange(x, 0);
    EXPECT_EQ(old, 99);
    EXPECT_EQ(x, 0);
}

TEST(Ubuiltin, Addressof)
{
    int x  = 42;
    char c = 'A';
    EXPECT_EQ(tklib::addressof(x), &x);
    EXPECT_EQ(tklib::addressof(c), &c);
}

TEST(Ubuiltin, IsSame)
{
    EXPECT_TRUE((tklib::is_same_v<int, int>));
    EXPECT_TRUE((tklib::is_same_v<DWORD, DWORD>));
    EXPECT_FALSE((tklib::is_same_v<int, float>));
    EXPECT_FALSE((tklib::is_same_v<DWORD, QWORD>));
    EXPECT_FALSE((tklib::is_same_v<int, const int>));
}

TEST(Ubuiltin, IsIntegral)
{
    EXPECT_TRUE(tklib::is_integral_v<BYTE>);
    EXPECT_TRUE(tklib::is_integral_v<WORD>);
    EXPECT_TRUE(tklib::is_integral_v<DWORD>);
    EXPECT_TRUE(tklib::is_integral_v<QWORD>);
    EXPECT_TRUE(tklib::is_integral_v<CHAR>);
    EXPECT_TRUE(tklib::is_integral_v<SHORT>);
    EXPECT_TRUE(tklib::is_integral_v<LONG>);
    EXPECT_TRUE(tklib::is_integral_v<LONGLONG>);
    EXPECT_TRUE(tklib::is_integral_v<BOOL>);
    EXPECT_FALSE(tklib::is_integral_v<float>);
    EXPECT_FALSE(tklib::is_integral_v<double>);
    EXPECT_FALSE(tklib::is_integral_v<PVOID>);
}

TEST(Ubuiltin, IsPointer)
{
    EXPECT_TRUE(tklib::is_pointer_v<int *>);
    EXPECT_TRUE(tklib::is_pointer_v<const int *>);
    EXPECT_TRUE(tklib::is_pointer_v<PVOID>);
    EXPECT_FALSE(tklib::is_pointer_v<int>);
    EXPECT_FALSE(tklib::is_pointer_v<int &>);
}

TEST(Ubuiltin, IsConst)
{
    EXPECT_TRUE(tklib::is_const_v<const int>);
    EXPECT_TRUE(tklib::is_const_v<const QWORD>);
    EXPECT_FALSE(tklib::is_const_v<int>);
    EXPECT_FALSE(tklib::is_const_v<QWORD>);
}

TEST(Ubuiltin, Conditional)
{
    EXPECT_TRUE((tklib::is_same_v<tklib::conditional_t<true, int, float>, int>));
    EXPECT_TRUE((tklib::is_same_v<tklib::conditional_t<false, int, float>, float>));
}

TEST(Ubuiltin, EnableIf)
{
    EXPECT_TRUE((tklib::is_same_v<tklib::enable_if_t<true, int>, int>));
}