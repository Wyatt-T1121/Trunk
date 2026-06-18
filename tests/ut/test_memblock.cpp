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
 *  PURPOSE : Unit tests for memblock                                            *
 ********************************************************************************/

#include <cbk/mem/memblock.h>
#include <gtest/gtest.h>

static trunk::boot::BootInfo make_fake_boot_info()
{
    trunk::boot::BootInfo info{};

    info.mmap[0].base   = 0x100000;
    info.mmap[0].length = 0x400000;
    info.mmap[0].type   = trunk::boot::MemoryType::Available;

    info.mmap[1].base   = 0x600000;
    info.mmap[1].length = 0x200000;
    info.mmap[1].type   = trunk::boot::MemoryType::Available;

    info.mmap[2].base   = 0x000000;
    info.mmap[2].length = 0x100000;
    info.mmap[2].type   = trunk::boot::MemoryType::Reserved;

    info.mmap_count = 3;
    return info;
}

class MemblockTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        auto info = make_fake_boot_info();
        trunk::mem::memblock_init(info);
    }
};

TEST_F(MemblockTest, InitSetsCounters)
{
    EXPECT_GT(trunk::mem::memblock_total_free(), 0ull);
    EXPECT_GT(trunk::mem::memblock_total_reserved(), 0ull);
}

TEST_F(MemblockTest, AllocBasic)
{
    u64 a = trunk::mem::memblock_alloc(0x1000, 0x1000);
    EXPECT_NE(a, 0ull);

    u64 b = trunk::mem::memblock_alloc(0x1000, 0x1000);
    EXPECT_NE(b, 0ull);
    EXPECT_NE(a, b);
}

TEST_F(MemblockTest, AllocAlignment)
{
    u64 a = trunk::mem::memblock_alloc(0x100, 0x1000);
    EXPECT_EQ(a % 0x1000, 0ull);

    u64 b = trunk::mem::memblock_alloc(0x100, 0x200);
    EXPECT_EQ(b % 0x200, 0ull);

    u64 c = trunk::mem::memblock_alloc(0x100, 0x10);
    EXPECT_EQ(c % 0x10, 0ull);
}

TEST_F(MemblockTest, AllocZeroSizeReturnsZero)
{
    EXPECT_EQ(trunk::mem::memblock_alloc(0, 0x1000), 0ull);
    EXPECT_EQ(trunk::mem::memblock_alloc(0x1000, 0), 0ull);
}

TEST_F(MemblockTest, ReserveAndIsReserved)
{
    trunk::mem::memblock_reserve(0x200000, 0x1000);
    EXPECT_TRUE(trunk::mem::memblock_is_reserved(0x200000, 0x1000));
    EXPECT_TRUE(trunk::mem::memblock_is_reserved(0x200500, 0x100));
    EXPECT_FALSE(trunk::mem::memblock_is_reserved(0x300000, 0x1000));
}

TEST_F(MemblockTest, AllocSkipsReserved)
{
    trunk::mem::memblock_reserve(0x100000, 0x200000);

    u64 a = trunk::mem::memblock_alloc(0x1000, 0x1000);
    EXPECT_NE(a, 0ull);
    EXPECT_FALSE(trunk::mem::memblock_is_reserved(a, 0x1000) && a < 0x300000);
}

TEST_F(MemblockTest, AllocDecreasesTotalFree)
{
    u64 free_before = trunk::mem::memblock_total_free();
    trunk::mem::memblock_alloc(0x1000, 0x1000);
    u64 free_after = trunk::mem::memblock_total_free();
    EXPECT_LT(free_after, free_before);
}

TEST_F(MemblockTest, AllocNonOverlapping)
{
    u64 a = trunk::mem::memblock_alloc(0x2000, 0x1000);
    u64 b = trunk::mem::memblock_alloc(0x2000, 0x1000);
    u64 c = trunk::mem::memblock_alloc(0x2000, 0x1000);

    EXPECT_NE(a, 0ull);
    EXPECT_NE(b, 0ull);
    EXPECT_NE(c, 0ull);
    EXPECT_NE(a, b);
    EXPECT_NE(b, c);
    EXPECT_NE(a, c);

    EXPECT_FALSE(a < b && a + 0x2000 > b);
    EXPECT_FALSE(b < c && b + 0x2000 > c);
}

TEST_F(MemblockTest, IsReservedEdgeCases)
{
    EXPECT_FALSE(trunk::mem::memblock_is_reserved(0x200000, 0));
    trunk::mem::memblock_reserve(0x200000, 0x1000);
    EXPECT_FALSE(trunk::mem::memblock_is_reserved(0x201000, 0x1000));
    EXPECT_FALSE(trunk::mem::memblock_is_reserved(0x1FF000, 0x1000));
}