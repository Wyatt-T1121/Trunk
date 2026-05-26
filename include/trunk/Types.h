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
 *  FILE    : include/trunk/Types.h                                              *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Freestanding primitive type aliases for the entire kernel.         *
 *            No standard library headers are included anywhere in Trunk.        *
 *            Every file that needs a sized integer includes this.               *
 *                                                                               *
 ********************************************************************************/

#pragma once

/* *******************************************************************************
 *  AUTHOR  : Trollycat                                                          *
 *  FUNC    : Unsigned integer aliases                                           *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Fixed-width unsigned types.                                        *
 ********************************************************************************/
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

/* *******************************************************************************
 *  AUTHOR  : Trollycat                                                          *
 *  FUNC    : Signed integer aliases                                             *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Fixed-width signed types.                                          *
 ********************************************************************************/
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

/* *******************************************************************************
 *  AUTHOR  : Trollycat                                                          *
 *  FUNC    : Size and pointer-sized types                                       *
 *  DATE    : 2026                                                               *
 *  PURPOSE : usize for counts/sizes, uptr for physical/virtual addresses.       *
 ********************************************************************************/
using usize = unsigned long long;
using isize = signed long long;
using uptr = unsigned long long;