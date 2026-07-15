
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_system.h"

static void platformDump(PalBool verbose)
{
    uint32_t xSize = 60;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;
    uint32_t xOffset4 = 12;
    uint32_t xOffset5 = 16;
    uint32_t xOffset6 = 28;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalPlatformInfo);
    uint32_t yAlign = ALIGNOF(PalPlatformInfo);
    uint32_t yOffset1 = offsetof(PalPlatformInfo, type);
    uint32_t yOffset2 = offsetof(PalPlatformInfo, apiType);
    uint32_t yOffset3 = offsetof(PalPlatformInfo, totalMemory);
    uint32_t yOffset4 = offsetof(PalPlatformInfo, totalRAM);
    uint32_t yOffset5 = offsetof(PalPlatformInfo, version);
    uint32_t yOffset6 = offsetof(PalPlatformInfo, name);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xOffset4 == yOffset4      &&
        xOffset5 == yOffset5      &&
        xOffset6 == yOffset6      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalPlatformInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field          Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size           %u           %u", xSize, ySize);
        palLog(nullptr, "align          %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding        %u            %u", xPadding, yPadding);
        palLog(nullptr, "type @         %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "apiType @      %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "totalMemory @  %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "totalRAM @     %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "version @      %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "name @         %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void cpuDump(PalBool verbose)
{
    uint32_t xSize = 112;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 12;
    uint32_t xOffset4 = 16;
    uint32_t xOffset5 = 20;
    uint32_t xOffset6 = 24;
    uint32_t xOffset7 = 28;
    uint32_t xOffset8 = 32;
    uint32_t xOffset9 = 48;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalCPUInfo);
    uint32_t yAlign = ALIGNOF(PalCPUInfo);
    uint32_t yOffset1 = offsetof(PalCPUInfo, features);
    uint32_t yOffset2 = offsetof(PalCPUInfo, architecture);
    uint32_t yOffset3 = offsetof(PalCPUInfo, numCores);
    uint32_t yOffset4 = offsetof(PalCPUInfo, cacheL1);
    uint32_t yOffset5 = offsetof(PalCPUInfo, cacheL2);
    uint32_t yOffset6 = offsetof(PalCPUInfo, cacheL3);
    uint32_t yOffset7 = offsetof(PalCPUInfo, numLogicalProcessors);
    uint32_t yOffset8 = offsetof(PalCPUInfo, vendor);
    uint32_t yOffset9 = offsetof(PalCPUInfo, model);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xOffset4 == yOffset4      &&
        xOffset5 == yOffset5      &&
        xOffset6 == yOffset6      &&
        xOffset7 == yOffset7      &&
        xOffset8 == yOffset8      &&
        xOffset9 == yOffset9      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalCPUInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field                   Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size                    %u          %u", xSize, ySize);
        palLog(nullptr, "align                   %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding                 %u            %u", xPadding, yPadding);
        palLog(nullptr, "features @              %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "architecture @          %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "numCores @              %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "cacheL1 @               %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "cacheL2 @               %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "cacheL3 @               %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "numLogicalProcessors @  %u           %u", xOffset7, yOffset7);
        palLog(nullptr, "vendor @                %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "model @                 %u           %u", xOffset9, yOffset9);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

void systemABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "System ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    platformDump(verbose);
    cpuDump(verbose); 
}