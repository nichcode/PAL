
#include "helpers.h"
#include "pal2/system/cpu.h"

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