
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_system.h"

PalBool systemABIDump(uint32_t flags)
{
    if (!(flags & DUMP_FLAG_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "System ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    FieldInfo platformInfoFields[] = {
        { "type", {0, 4}, FIELD(PalPlatformInfo, type) },
        { "apiType", {4, 4}, FIELD(PalPlatformInfo, apiType) },
        { "totalMemory", {8, 4}, FIELD(PalPlatformInfo, totalMemory) },
        { "totalRAM", {12, 4}, FIELD(PalPlatformInfo, totalRAM) },
        { "version", {16, 12}, FIELD(PalPlatformInfo, version) },
        { "name", {28, 32}, FIELD(PalPlatformInfo, name) }
    };

    FieldInfo cpuInfoFields[] = {
        { "features", {0, 8}, FIELD(PalCPUInfo, features) },
        { "architecture", {8, 4}, FIELD(PalCPUInfo, architecture) },
        { "numCores", {12, 4}, FIELD(PalCPUInfo, numCores) },
        { "cacheL1", {16, 4}, FIELD(PalCPUInfo, cacheL1) },
        { "cacheL2", {20, 4}, FIELD(PalCPUInfo, cacheL2) },
        { "cacheL3", {24, 4}, FIELD(PalCPUInfo, cacheL3) },
        { "numLogicalProcessors", {28, 4}, FIELD(PalCPUInfo, numLogicalProcessors) },
        { "vendor", {32, 16}, FIELD(PalCPUInfo, vendor) },
        { "model", {48, 64}, FIELD(PalCPUInfo, model) }
    };
  
    StructInfo platformInfo = {0};
    platformInfo.name = "PalPlatformInfo";
    platformInfo.fields = platformInfoFields;
    platformInfo.fieldCount = ARRAY_SIZE(platformInfoFields);
    platformInfo.expected.alignof = 4;
    platformInfo.expected.size = 60;
    platformInfo.expected.padding = 0;
    platformInfo.actual = STRUCT(PalPlatformInfo);

    StructInfo cpuInfo = {0};
    cpuInfo.name = "PalCPUInfo";
    cpuInfo.fields = cpuInfoFields;
    cpuInfo.fieldCount = ARRAY_SIZE(cpuInfoFields);
    cpuInfo.expected.alignof = 8;
    cpuInfo.expected.size = 112;
    cpuInfo.expected.padding = 0;
    cpuInfo.actual = STRUCT(PalCPUInfo);

    PalBool status = checkABI(&platformInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&cpuInfo, flags);
}