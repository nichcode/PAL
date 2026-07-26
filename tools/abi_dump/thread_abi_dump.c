
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal2/pal_thread.h"

PalBool threadABIDump(uint32_t flags)
{
    if (!(flags & DUMP_FLAG_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Thread ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    FieldInfo threadCreateInfoFields[] = {
        {"stackSize", {0, 8}, FIELD(PalThreadCreateInfo, stackSize)},
        {"allocator", {8, 8}, FIELD(PalThreadCreateInfo, allocator)},
        {"entry", {16, 8}, FIELD(PalThreadCreateInfo, entry)},
        {"arg", {24, 8}, FIELD(PalThreadCreateInfo, arg)}};

    StructInfo threadCreateInfo = {0};
    threadCreateInfo.name = "PalThreadCreateInfo";
    threadCreateInfo.fields = threadCreateInfoFields;
    threadCreateInfo.fieldCount = ARRAY_SIZE(threadCreateInfoFields);
    threadCreateInfo.expected.alignof = 8;
    threadCreateInfo.expected.size = 32;
    threadCreateInfo.expected.padding = 0;
    threadCreateInfo.actual = STRUCT(PalThreadCreateInfo);

    return checkABI(&threadCreateInfo, flags);
}