
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_thread.h"

void dumpThreadABI()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Thread ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    uint32_t xSize = 32;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xOffset4 = 24;

    uint32_t ySize = sizeof(PalThreadCreateInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalThreadCreateInfo);
    uint32_t yOffset1 = offsetof(PalThreadCreateInfo, stackSize);
    uint32_t yOffset2 = offsetof(PalThreadCreateInfo, allocator);
    uint32_t yOffset3 = offsetof(PalThreadCreateInfo, entry);
    uint32_t yOffset4 = offsetof(PalThreadCreateInfo, arg);

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      && 
        xOffset4 == yOffset4) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "PalThreadCreateInfo");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field         Expected     Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "size          %u          %u", xSize, ySize);
    palLog(nullptr, "align         %u           %u", xAlign, yAlign);
    palLog(nullptr, "stackSize @   %u           %u", xOffset1, yOffset1);
    palLog(nullptr, "allocator @   %u           %u", xOffset2, yOffset2);
    palLog(nullptr, "entry @       %u          %u", xOffset3, yOffset3);
    palLog(nullptr, "arg @         %u          %u", xOffset4, yOffset4);
    palLog(nullptr, "===========================================");

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}