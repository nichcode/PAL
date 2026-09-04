
/**
 * PAL - Prime Abstraction Layer (PAL)
 * A cross platform abstraction layer over graphics and windowing APIs
 * -------------------------------------------------------------------
 * 
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef ABI_DUMP_H
#define ABI_DUMP_H

#include "pal2/pal_core.h"
#include <stddef.h>
#include <stdbool.h>

#ifdef _MSC_VER
#define ALIGNOF(type) __alignof(type)
#else
#define ALIGNOF(type) __alignof__(type)
#endif // _MSC_VER

#define ABI_DUMP_FLAG_CORE (1u << 0)
#define ABI_DUMP_FLAG_EVENT (1u << 1)
#define ABI_DUMP_FLAG_THREAD (1u << 2)
#define ABI_DUMP_FLAG_OPENGL (1u << 3)
#define ABI_DUMP_FLAG_GRAPHICS (1u << 4)
#define ABI_DUMP_FLAG_SYSTEM (1u << 5)
#define ABI_DUMP_FLAG_VIDEO (1u << 6)
#define ABI_DUMP_FLAG_VERBOSE (1u << 7)
#define ABI_DUMP_FLAG_QUICK (1u << 8)
#define ABI_DUMP_FLAG_ALL 0x7F

typedef struct FieldBase
{
    uint32_t offset;
    uint32_t size;
} FieldBase;

typedef struct StructBase
{
    uint32_t align;
    uint32_t size;
    uint32_t padding;
} StructBase;

typedef struct FieldInfo
{
    const char* name;
    FieldBase expected;
    FieldBase actual;
} FieldInfo;

typedef struct StructInfo
{
    const char* name;
    FieldInfo* fields;
    uint32_t fieldCount;
    StructBase expected;
    StructBase actual;
} StructInfo;

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define PADDING(type) (((ALIGNOF(type) - sizeof(type)) % ALIGNOF(type)))
#define FIELD(type, field) ((FieldBase){offsetof(type, field), sizeof(((type*)0)->field)})
#define STRUCT(type) ((StructBase){ALIGNOF(type), sizeof(type), PADDING(type)})

bool checkABI(
    const StructInfo* info,
    uint32_t flags);

#endif // ABI_DUMP_H