
#include "helpers.h"
#include "pal2/video/monitor.h"

bool monitorInfoStruct(void)
{
    FieldInfo fields[] = {
        {"x", {0, 4}, FIELD(PalMonitorInfo, x)},
        {"y", {4, 4}, FIELD(PalMonitorInfo, y)},
        {"width", {8, 4}, FIELD(PalMonitorInfo, width)},
        {"height", {12, 4}, FIELD(PalMonitorInfo, height)},
        {"dpi", {16, 4}, FIELD(PalMonitorInfo, dpi)},
        {"refreshRate", {20, 4}, FIELD(PalMonitorInfo, refreshRate)},
        {"orientation", {24, 4}, FIELD(PalMonitorInfo, orientation)},
        {"primary", {28, 4}, FIELD(PalMonitorInfo, primary)},
        {"name", {32, 32}, FIELD(PalMonitorInfo, name)}
    };

    StructInfo info = {0};
    info.name = "PalMonitorInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 64;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMonitorInfo);

    return dumpStruct(&info);
}

bool monitorModeStruct(void)
{
    FieldInfo fields[] = {
        {"bpp", {0, 4}, FIELD(PalMonitorMode, bpp)},
        {"refreshRate", {4, 4}, FIELD(PalMonitorMode, refreshRate)},
        {"width", {8, 4}, FIELD(PalMonitorMode, width)},
        {"height", {12, 4}, FIELD(PalMonitorMode, height)}
    };

    StructInfo info = {0};
    info.name = "PalMonitorMode";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalMonitorMode);

    return dumpStruct(&info);
}