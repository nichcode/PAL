
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"

PalBool coreABIDump(uint32_t flags)
{
    if (!(flags & DUMP_FLAG_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Core ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    FieldInfo versionFields[] = {
        {"major", {0, 4}, FIELD(PalVersion, major)},
        {"minor", {4, 4}, FIELD(PalVersion, minor)},
        {"build", {8, 4}, FIELD(PalVersion, build)}};

    FieldInfo allocatorFields[] = {
        {"allocate", {0, 8}, FIELD(PalAllocator, allocate)},
        {"free", {8, 8}, FIELD(PalAllocator, free)},
        {"userData", {16, 8}, FIELD(PalAllocator, userData)}};

    FieldInfo loggerFields[] = {
        {"callback", {0, 8}, FIELD(PalLogger, callback)},
        {"userData", {8, 8}, FIELD(PalLogger, userData)}};

    StructInfo versionInfo = {0};
    versionInfo.name = "PalVersion";
    versionInfo.fields = versionFields;
    versionInfo.fieldCount = ARRAY_SIZE(versionFields);
    versionInfo.expected.alignof = 4;
    versionInfo.expected.size = 12;
    versionInfo.expected.padding = 0;
    versionInfo.actual = STRUCT(PalVersion);

    StructInfo allocatorInfo = {0};
    allocatorInfo.name = "PalAllocator";
    allocatorInfo.fields = allocatorFields;
    allocatorInfo.fieldCount = ARRAY_SIZE(allocatorFields);
    allocatorInfo.expected.alignof = 8;
    allocatorInfo.expected.size = 24;
    allocatorInfo.expected.padding = 0;
    allocatorInfo.actual = STRUCT(PalAllocator);

    StructInfo loggerInfo = {0};
    loggerInfo.name = "PalLogger";
    loggerInfo.fields = loggerFields;
    loggerInfo.fieldCount = ARRAY_SIZE(loggerFields);
    loggerInfo.expected.alignof = 8;
    loggerInfo.expected.size = 16;
    loggerInfo.expected.padding = 0;
    loggerInfo.actual = STRUCT(PalLogger);

    PalBool status = checkABI(&versionInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&allocatorInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&loggerInfo, flags);
}