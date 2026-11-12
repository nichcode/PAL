
#include "helpers.h"
#include "pal2/system/platform.h"

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