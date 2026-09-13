
#include "helpers.h"
#include "pal2/pal_system.h"

bool platformInfoStruct(void)
{
    FieldInfo fields[] = {
        {"type", {0, 4}, FIELD(PalPlatformInfo, type)},
        {"apiType", {4, 4}, FIELD(PalPlatformInfo, apiType)},
        {"totalMemory", {8, 4}, FIELD(PalPlatformInfo, totalMemory)},
        {"totalRAM", {12, 4}, FIELD(PalPlatformInfo, totalRAM)},
        {"version", {16, 12}, FIELD(PalPlatformInfo, version)},
        {"name", {28, 32}, FIELD(PalPlatformInfo, name)}
    };

    StructInfo info = {0};
    info.name = "PalPlatformInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 60;
    info.expected.padding = 0;
    info.actual = STRUCT(PalPlatformInfo);

    return dumpStruct(&info);
}

bool cpuInfoStruct(void)
{
    FieldInfo fields[] = {
        {"features", {0, 8}, FIELD(PalCPUInfo, features)},
        {"architecture", {8, 4}, FIELD(PalCPUInfo, architecture)},
        {"numCores", {12, 4}, FIELD(PalCPUInfo, numCores)},
        {"cacheL1", {16, 4}, FIELD(PalCPUInfo, cacheL1)},
        {"cacheL2", {20, 4}, FIELD(PalCPUInfo, cacheL2)},
        {"cacheL3", {24, 4}, FIELD(PalCPUInfo, cacheL3)},
        {"numLogicalProcessors", {28, 4}, FIELD(PalCPUInfo, numLogicalProcessors)},
        {"vendor", {32, 16}, FIELD(PalCPUInfo, vendor)},
        {"model", {48, 64}, FIELD(PalCPUInfo, model)}
    };

    StructInfo info = {0};
    info.name = "PalCPUInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 112;
    info.expected.padding = 0;
    info.actual = STRUCT(PalCPUInfo);

    return dumpStruct(&info);
}

bool systemStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "System ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = platformInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return cpuInfoStruct();
}