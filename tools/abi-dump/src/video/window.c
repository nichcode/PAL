
#include "helpers.h"
#include "pal2/video/window.h"

bool flashInfoStruct(void)
{
    FieldInfo fields[] = {
        {"flags", {0, 4}, FIELD(PalFlashInfo, flags)},
        {"interval", {4, 4}, FIELD(PalFlashInfo, interval)},
        {"count", {8, 4}, FIELD(PalFlashInfo, count)}
    };

    StructInfo info = {0};
    info.name = "PalFlashInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalFlashInfo);

    return dumpStruct(&info);
}

bool windowHandleInfoStruct(void)
{
    FieldInfo fields[] = {
        {"nativeInstance", {0, 8}, FIELD(PalWindowHandleInfo, nativeInstance)},
        {"nativeWindow", {8, 8}, FIELD(PalWindowHandleInfo, nativeWindow)},
        {"nativeHandle1", {16, 8}, FIELD(PalWindowHandleInfo, nativeHandle1)},
        {"nativeHandle2", {24, 8}, FIELD(PalWindowHandleInfo, nativeHandle2)},
        {"nativeHandle3", {32, 8}, FIELD(PalWindowHandleInfo, nativeHandle3)}
    };

    StructInfo info = {0};
    info.name = "PalWindowHandleInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 40;
    info.expected.padding = 0;
    info.actual = STRUCT(PalWindowHandleInfo);

    return dumpStruct(&info);
}

bool windowCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        {"title", {0, 8}, FIELD(PalWindowCreateInfo, title)},
        {"monitor", {8, 8}, FIELD(PalWindowCreateInfo, monitor)},
        {"appName", {16, 8}, FIELD(PalWindowCreateInfo, appName)},
        {"instanceName", {24, 8}, FIELD(PalWindowCreateInfo, instanceName)},
        {"fbConfigBackend", {32, 4}, FIELD(PalWindowCreateInfo, fbConfigBackend)},
        {"fbConfigIndex", {36, 4}, FIELD(PalWindowCreateInfo, fbConfigIndex)},
        {"width", {40, 4}, FIELD(PalWindowCreateInfo, width)},
        {"height", {44, 4}, FIELD(PalWindowCreateInfo, height)},
        {"show", {48, 4}, FIELD(PalWindowCreateInfo, show)},
        {"style", {52, 4}, FIELD(PalWindowCreateInfo, style)},
        {"state", {56, 4}, FIELD(PalWindowCreateInfo, state)},
        {"center", {60, 4}, FIELD(PalWindowCreateInfo, center)}
    };

    StructInfo info = {0};
    info.name = "PalWindowCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 64;
    info.expected.padding = 0;
    info.actual = STRUCT(PalWindowCreateInfo);

    return dumpStruct(&info);
}