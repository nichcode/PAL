
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

static PalBool checkABI(
    const StructInfo* info,
    PalBool verbose)
{
    // find the size of the field column
    uint32_t fieldSize = 0;
    const uint32_t expectedSize = 12;
    const uint32_t actualSize = 6;
    char seperator[256];
    PalBool passed = PAL_TRUE;
    const char* result = s_PassedString;

    if (info->expected.size != info->actual.size &&
        info->expected.alignof != info->actual.alignof &&
        info->expected.padding != info->actual.padding) {
        passed = PAL_FALSE;
        result = s_FailedString;
    }

    for (uint32_t i = 0; i < info->fieldCount; i++) {
        FieldInfo* field = &info->fields[i];
        uint32_t size = strlen(field->name);
        if (size > fieldSize) {
            fieldSize = size;
        }
    }

    fieldSize += 2; // add 6 spaces
    palLog(nullptr, "%llu", fieldSize);
    uint32_t seperatorSize = fieldSize + expectedSize + actualSize + 4; // add 4 spaces
    for (int i = 0; i < seperatorSize; i++) {
        seperator[i] = '=';
    }
    seperator[seperatorSize] = '\0';

    palLog(nullptr, "Struct: %s", info->name);
    if (verbose) {
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
        if (field->expected.offset != field->actual.offset && 
            field->expected.size != field->actual.size) {
            passed = PAL_FALSE;
            result = s_FailedString;
        }

        if (verbose) {
            palLog(nullptr, 
                "%-*s (%03zu, %02zu)    (%03zu, %02zu)", 
                fieldSize,
                field->name,
                field->expected.offset,
                field->expected.size,
                field->actual.offset,
                field->actual.size);
        }
    }
    palLog(nullptr, seperator);
    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");

    return passed;
}

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))
#define FIELD(type, field) ((FieldBase){offsetof(type, field), sizeof(((type*)0)->field)})
#define PADDING(type) (ALIGNOF(type) - sizeof(type) % ALIGNOF(type))
#define STRUCT(type) ((StructBase){ALIGNOF(type), sizeof(type), PADDING(type)})

PalBool coreABIDump(PalBool verbose);
// PalBool eventABIDump(PalBool verbose);
// PalBool threadABIDump(PalBool verbose);
// PalBool systemABIDump(PalBool verbose);
// PalBool videoABIDump(PalBool verbose);
// PalBool openglABIDump(PalBool verbose);
// PalBool graphicsABIDump(PalBool verbose);

#endif // _DUMPS_H