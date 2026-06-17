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
 *  MODULE  : Global definitions                                                 *
 *  DATE    : 2026                                                               *
 *  PURPOSE : Stores the operating system version                                *
 ********************************************************************************/
#pragma once

#include <macros.h>
#include <types.h>

#ifndef TRUNK_VERSION_STRING
#define TRUNK_VERSION_STRING "0.0.0-dev"
#endif

#ifndef TRUNK_VERSION_MAJOR
#define TRUNK_VERSION_MAJOR 0
#endif

#ifndef TRUNK_VERSION_MINOR
#define TRUNK_VERSION_MINOR 0
#endif

#ifndef TRUNK_VERSION_PATCH
#define TRUNK_VERSION_PATCH 0
#endif

struct KernelVersion
{
    u16 major;
    u16 minor;
    u16 patch;
    const char *build_string;
};

constexpr KernelVersion g_KernelVersion{.major        = TRUNK_VERSION_MAJOR,
                                        .minor        = TRUNK_VERSION_MINOR,
                                        .patch        = TRUNK_VERSION_PATCH,
                                        .build_string = TRUNK_VERSION_STRING};

NO_DISCARD inline const KernelVersion &get_version() noexcept
{
    return g_KernelVersion;
}
