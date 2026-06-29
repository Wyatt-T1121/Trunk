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
 *  MODULE  : Global definition                                                  *
 *  DATE    : 2026                                                               *
 *  PURPOSE : NT-style STATUS codes                                              *
 ********************************************************************************/
#pragma once

#include <types.h>

typedef LONG CBKSTATUS;

// clang-format off
#define NT_SUCCESS(Status)           (((CBKSTATUS)(Status)) >= 0)
#define NT_INFORMATION(Status)       ((((ULONG)(Status)) >> 30) == 1)
#define NT_WARNING(Status)           ((((ULONG)(Status)) >> 30) == 2)
#define NT_ERROR(Status)             ((((ULONG)(Status)) >> 30) == 3)

// SUCCESS CODES (0x00000000 - 0x3FFFFFFF)
constexpr CBKSTATUS STATUS_SUCCESS      = 0x00000000L;
constexpr CBKSTATUS STATUS_WAIT_0       = 0x00000000L;
constexpr CBKSTATUS STATUS_WAIT_TIMEOUT = 0x00000102L;
constexpr CBKSTATUS STATUS_PENDING      = 0x00000103L;
constexpr CBKSTATUS STATUS_REPARSE      = 0x00000104L;
constexpr CBKSTATUS STATUS_MORE_ENTRIES = 0x00000105L;

// INFORMATIONAL CODES (0x40000000 - 0x7FFFFFFF)
constexpr CBKSTATUS STATUS_OBJECT_NAME_EXISTS   = 0x40000000L;
constexpr CBKSTATUS STATUS_THREAD_WAS_SUSPENDED = 0x40000001L;

// WARNING CODES (0x80000000 - 0xBFFFFFFF)
constexpr CBKSTATUS STATUS_GUARD_PAGE_VIOLATION  = 0x80000001L;
constexpr CBKSTATUS STATUS_DATATYPE_MISALIGNMENT = 0x80000002L;
constexpr CBKSTATUS STATUS_BREAKPOINT            = 0x80000003L;
constexpr CBKSTATUS STATUS_SINGLE_STEP           = 0x80000004L;
constexpr CBKSTATUS STATUS_BUFFER_OVERFLOW       = 0x80000005L;

// ERROR CODES (0xC0000000 - 0xFFFFFFFF)

// -- General System Errors --
constexpr CBKSTATUS STATUS_UNSUCCESSFUL             = 0xC0000001L;
constexpr CBKSTATUS STATUS_NOT_IMPLEMENTED          = 0xC0000002L;
constexpr CBKSTATUS STATUS_INVALID_INFO_CLASS       = 0xC0000003L;
constexpr CBKSTATUS STATUS_INFO_LENGTH_MISMATCH     = 0xC0000004L;
constexpr CBKSTATUS STATUS_ACCESS_VIOLATION         = 0xC0000005L;
constexpr CBKSTATUS STATUS_INVALID_HANDLE           = 0xC0000008L;
constexpr CBKSTATUS STATUS_INVALID_PARAMETER        = 0xC000000DL;
constexpr CBKSTATUS STATUS_NO_SUCH_DEVICE           = 0xC000000EL;
constexpr CBKSTATUS STATUS_NO_SUCH_FILE             = 0xC000000FL;
constexpr CBKSTATUS STATUS_MORE_PROCESSING_REQUIRED = 0xC0000016L;

// -- Memory & VMM Errors --
constexpr CBKSTATUS STATUS_NO_MEMORY                = 0xC0000017L;
constexpr CBKSTATUS STATUS_CONFLICTING_ADDRESSES    = 0xC0000018L;
constexpr CBKSTATUS STATUS_NOT_MAPPED_VIEW          = 0xC0000019L;
constexpr CBKSTATUS STATUS_UNABLE_TO_FREE_VM        = 0xC000001AL;
constexpr CBKSTATUS STATUS_UNABLE_TO_DELETE_SECTION = 0xC000001BL;
constexpr CBKSTATUS STATUS_ILLEGAL_INSTRUCTION      = 0xC000001DL;
constexpr CBKSTATUS STATUS_PAGEFILE_QUOTA           = 0xC000001CL;
constexpr CBKSTATUS STATUS_COMMITMENT_LIMIT         = 0xC000012DL;
constexpr CBKSTATUS STATUS_INSUFFICIENT_RESOURCES   = 0xC000009AL;

// -- Security & Access Errors --
constexpr CBKSTATUS STATUS_ACCESS_DENIED        = 0xC0000022L;
constexpr CBKSTATUS STATUS_BUFFER_TOO_SMALL     = 0xC0000023L;
constexpr CBKSTATUS STATUS_OBJECT_TYPE_MISMATCH = 0xC0000024L;
constexpr CBKSTATUS STATUS_LOGON_FAILURE        = 0xC000006DL;
constexpr CBKSTATUS STATUS_PRIVILEGE_NOT_HELD   = 0xC0000061L;

// -- Object & File System Errors --
constexpr CBKSTATUS STATUS_OBJECT_NAME_INVALID     = 0xC0000033L;
constexpr CBKSTATUS STATUS_OBJECT_NAME_NOT_FOUND   = 0xC0000034L;
constexpr CBKSTATUS STATUS_OBJECT_NAME_COLLISION   = 0xC0000035L;
constexpr CBKSTATUS STATUS_PORT_DISCONNECTED       = 0xC0000037L;
constexpr CBKSTATUS STATUS_DEVICE_ALREADY_ATTACHED = 0xC0000038L;
constexpr CBKSTATUS STATUS_OBJECT_PATH_INVALID     = 0xC0000039L;
constexpr CBKSTATUS STATUS_OBJECT_PATH_NOT_FOUND   = 0xC000003AL;
constexpr CBKSTATUS STATUS_SHARING_VIOLATION       = 0xC0000043L;
constexpr CBKSTATUS STATUS_DIRECTORY_NOT_EMPTY     = 0xC0000101L;

// -- Hardware & I/O Errors --
constexpr CBKSTATUS STATUS_IO_TIMEOUT           = 0xC00000B5L;
constexpr CBKSTATUS STATUS_DEVICE_NOT_CONNECTED = 0xC000009DL;
constexpr CBKSTATUS STATUS_DEVICE_POWER_FAILURE = 0xC000009EL;
constexpr CBKSTATUS STATUS_NOT_FOUND            = 0xC0000225L;
constexpr CBKSTATUS STATUS_DEVICE_NOT_READY     = 0xC00000A3L;

// clang-format on