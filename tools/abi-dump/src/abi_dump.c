
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

#include "abi_dump.h"

static const char* s_FailedString = "FAILED";
static const char* s_PassedString = "PASSED";

bool checkABI(
    const StructInfo* info,
    uint32_t flags)
{
    uint32_t fieldSize = 0;
    const uint32_t expectedSize = 14;
    const uint32_t actualSize = 8;
    char seperator[256];
    bool passed = PAL_TRUE;
    const char* result = s_PassedString;

    if (info->expected.size != info->actual.size       ||
        info->expected.align != info->actual.align     ||
        info->expected.padding != info->actual.padding) {
        passed = PAL_FALSE;
        result = s_FailedString;
    }

    for (uint32_t i = 0; i < info->fieldCount; i++) {
        FieldInfo* field = &info->fields[i];
        uint32_t size = (uint32_t)strlen(field->name);
        if (size > fieldSize) {
            fieldSize = size;
        }
    }

    fieldSize += 2;
    uint32_t seperatorSize = fieldSize + expectedSize + actualSize + 4; // add 4 spaces
    for (int i = 0; i < seperatorSize; i++) {
        seperator[i] = '=';
    }
    seperator[seperatorSize] = '\0';

    if (!(flags & ABI_DUMP_FLAG_QUICK)) {
        palLog(nullptr, "Struct: %s", info->name);
    }

    if (flags & ABI_DUMP_FLAG_VERBOSE) {
        palLog(nullptr, "");
        palLog(nullptr, "Struct format: Property: (Expected, Actual)");
        palLog(nullptr, "Field format: (Offset, Size)");
        palLog(nullptr, "");

        palLog(nullptr, "Size:      (%04u, %04u)", info->expected.size, info->actual.size);
        palLog(nullptr, "Alignment: (%u, %u)", info->expected.align, info->actual.align);
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

        if (field->expected.offset != field->actual.offset ||
            field->expected.size != field->actual.size) {
            passed = PAL_FALSE;
            result = s_FailedString;
        }

        if (flags & ABI_DUMP_FLAG_VERBOSE) {
            palLog(
                nullptr,
                "%-*s (%04u, %03u)    (%04u, %03u)",
                fieldSize,
                field->name,
                field->expected.offset,
                field->expected.size,
                field->actual.offset,
                field->actual.size);
        }
    }
    if (flags & ABI_DUMP_FLAG_VERBOSE) {
        palLog(nullptr, seperator);
    }

    if (!(flags & ABI_DUMP_FLAG_QUICK)) {
        palLog(nullptr, "Status: %s", result);
        palLog(nullptr, "");
    }

    return passed;
}