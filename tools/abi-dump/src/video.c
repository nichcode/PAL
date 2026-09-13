
#include "helpers.h"
#include "pal2/pal_video.h"

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

bool iconCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        {"pixels", {0, 8}, FIELD(PalIconCreateInfo, pixels)},
        {"width", {8, 4}, FIELD(PalIconCreateInfo, width)},
        {"height", {12, 4}, FIELD(PalIconCreateInfo, height)}
    };

    StructInfo info = {0};
    info.name = "PalIconCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalIconCreateInfo);

    return dumpStruct(&info);
}

bool cursorCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        {"pixels", {0, 8}, FIELD(PalCursorCreateInfo, pixels)},
        {"width", {8, 4}, FIELD(PalCursorCreateInfo, width)},
        {"height", {12, 4}, FIELD(PalCursorCreateInfo, height)},
        {"xHotspot", {16, 4}, FIELD(PalCursorCreateInfo, xHotspot)},
        {"yHotspot", {20, 4}, FIELD(PalCursorCreateInfo, yHotspot)}
    };
   
    StructInfo info = {0};
    info.name = "PalCursorCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalCursorCreateInfo);

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

bool videoStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Video ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = monitorInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = monitorModeStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = flashInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = iconCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = cursorCreateInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = windowHandleInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return windowCreateInfoStruct();
}