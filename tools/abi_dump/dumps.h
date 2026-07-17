
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _DUMPS_H
#define _DUMPS_H

#include "pal/pal_core.h"
#include <stddef.h>

static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

#ifdef _MSC_VER
#define ALIGNOF(type) __alignof(type)
#else
#define ALIGNOF(type) __alignof__(type)
#endif // _MSC_VER

#define DUMP_FLAG_CORE (1u << 0)
#define DUMP_FLAG_EVENT (1u << 1)
#define DUMP_FLAG_THREAD (1u << 2)
#define DUMP_FLAG_OPENGL (1u << 3)
#define DUMP_FLAG_GRAPHICS (1u << 4)
#define DUMP_FLAG_SYSTEM (1u << 5)
#define DUMP_FLAG_VIDEO (1u << 6)
#define DUMP_FLAG_VERSION (1u << 7)
#define DUMP_FLAG_HELP (1u << 8)
#define DUMP_FLAG_VERBOSE (1u << 9)
#define DUMP_FLAG_QUICK (1u << 10)
#define DUMP_FLAG_ALL 0x7F

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define PADDING(type) (((ALIGNOF(type) - sizeof(type)) % ALIGNOF(type)))

typedef struct {
    uint32_t offset;
    uint32_t size;
} FieldBase;

typedef struct {
    uint32_t alignof;
    uint32_t size;
    uint32_t padding;
} StructBase;

typedef struct {
    const char* name;
    FieldBase expected;
    FieldBase actual;
} FieldInfo;

typedef struct {
    const char* name;
    FieldInfo* fields; 
    uint32_t fieldCount;
    StructBase expected;
    StructBase actual;
} StructInfo;

#define FIELD(type, field) ((FieldBase){offsetof(type, field), sizeof(((type*)0)->field)})
#define STRUCT(type) ((StructBase){ALIGNOF(type), sizeof(type), PADDING(type)})

static PalBool checkABI(
    const StructInfo* info,
    uint32_t flags)
{
    // find the size of the field column
    uint32_t fieldSize = 0;
    const uint32_t expectedSize = 14;
    const uint32_t actualSize = 8;
    char seperator[256];
    PalBool passed = PAL_TRUE;
    const char* result = s_PassedString;

    // clang-format off
    if (info->expected.size != info->actual.size       ||
        info->expected.alignof != info->actual.alignof ||
        info->expected.padding != info->actual.padding) {
        passed = PAL_FALSE;
        result = s_FailedString;
    }
    // clang-format on

    for (uint32_t i = 0; i < info->fieldCount; i++) {
        FieldInfo* field = &info->fields[i];
        uint32_t size = strlen(field->name);
        if (size > fieldSize) {
            fieldSize = size;
        }
    }

    fieldSize += 2; // add 6 spaces
    uint32_t seperatorSize = fieldSize + expectedSize + actualSize + 4; // add 4 spaces
    for (int i = 0; i < seperatorSize; i++) {
        seperator[i] = '=';
    }
    seperator[seperatorSize] = '\0';

    if (!(flags & DUMP_FLAG_QUICK)) {
        palLog(nullptr, "Struct: %s", info->name);
    }

    if (flags & DUMP_FLAG_VERBOSE) {
        palLog(nullptr, "");
        palLog(nullptr, "Struct format: Property: (Expected, Actual)");
        palLog(nullptr, "Field format: (Offset, Size)");
        palLog(nullptr, "");

        palLog(nullptr, "Size:      (%03u, %03u)", info->expected.size, info->actual.size);
        palLog(nullptr, "Alignment: (%u, %u)", info->expected.alignof, info->actual.alignof);
        palLog(nullptr, "Padding:   (%02u, %02u)", info->expected.padding, info->actual.padding);
        palLog(nullptr, seperator);
        
        palLog(
            nullptr, 
            "%-*s %-*s %-*s", 
            fieldSize, 
            "Field", 
            expectedSize, 
            "Expected", 
            actualSize, 
            "Actual");

        palLog(nullptr, seperator);
    }

    for (uint32_t i = 0; i < info->fieldCount; i++) {
        FieldInfo* field = &info->fields[i];

        // clang-format off
        if (field->expected.offset != field->actual.offset ||
            field->expected.size != field->actual.size) {
            passed = PAL_FALSE;
            result = s_FailedString;
        }
        // clang-format on

        if (flags & DUMP_FLAG_VERBOSE) {
            palLog(nullptr, 
                "%-*s (%03u, %03u)    (%03u, %03u)", 
                fieldSize,
                field->name,
                field->expected.offset,
                field->expected.size,
                field->actual.offset,
                field->actual.size);
        }
    }
    if (flags & DUMP_FLAG_VERBOSE) {
        palLog(nullptr, seperator);
    }

    if (!(flags & DUMP_FLAG_QUICK)) {
        palLog(nullptr, "Status: %s", result);
        palLog(nullptr, "");
    }

    return passed;
}

PalBool coreABIDump(uint32_t flags);
PalBool eventABIDump(uint32_t flags);
PalBool threadABIDump(uint32_t flags);
PalBool systemABIDump(uint32_t flags);
PalBool videoABIDump(uint32_t flags);
PalBool openglABIDump(uint32_t flags);
PalBool graphicsABIDump(uint32_t flags);

#endif // _DUMPS_H