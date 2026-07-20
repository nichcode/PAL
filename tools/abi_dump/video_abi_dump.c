
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_video.h"

PalBool videoABIDump(uint32_t flags)
{
    if (!(flags & DUMP_FLAG_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Video ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    FieldInfo monitorInfoFields[] = {
        {"x", {0, 4}, FIELD(PalMonitorInfo, x)},
        {"y", {4, 4}, FIELD(PalMonitorInfo, y)},
        {"width", {8, 4}, FIELD(PalMonitorInfo, width)},
        {"height", {12, 4}, FIELD(PalMonitorInfo, height)},
        {"dpi", {16, 4}, FIELD(PalMonitorInfo, dpi)},
        {"refreshRate", {20, 4}, FIELD(PalMonitorInfo, refreshRate)},
        {"orientation", {24, 4}, FIELD(PalMonitorInfo, orientation)},
        {"primary", {28, 4}, FIELD(PalMonitorInfo, primary)},
        {"name", {32, 32}, FIELD(PalMonitorInfo, name)}};

    FieldInfo monitorModeFields[] = {
        {"bpp", {0, 4}, FIELD(PalMonitorMode, bpp)},
        {"refreshRate", {4, 4}, FIELD(PalMonitorMode, refreshRate)},
        {"width", {8, 4}, FIELD(PalMonitorMode, width)},
        {"height", {12, 4}, FIELD(PalMonitorMode, height)}};

    FieldInfo flashInfoFields[] = {
        {"flags", {0, 4}, FIELD(PalFlashInfo, flags)},
        {"interval", {4, 4}, FIELD(PalFlashInfo, interval)},
        {"count", {8, 4}, FIELD(PalFlashInfo, count)}};

    FieldInfo iconCreateInfoFields[] = {
        {"pixels", {0, 8}, FIELD(PalIconCreateInfo, pixels)},
        {"width", {8, 4}, FIELD(PalIconCreateInfo, width)},
        {"height", {12, 4}, FIELD(PalIconCreateInfo, height)}};

    FieldInfo cursorCreateInfoFields[] = {
        {"pixels", {0, 8}, FIELD(PalCursorCreateInfo, pixels)},
        {"width", {8, 4}, FIELD(PalCursorCreateInfo, width)},
        {"height", {12, 4}, FIELD(PalCursorCreateInfo, height)},
        {"xHotspot", {16, 4}, FIELD(PalCursorCreateInfo, xHotspot)},
        {"yHotspot", {20, 4}, FIELD(PalCursorCreateInfo, yHotspot)}};

    FieldInfo windowHandleInfoFields[] = {
        {"nativeInstance", {0, 8}, FIELD(PalWindowHandleInfo, nativeInstance)},
        {"nativeWindow", {8, 8}, FIELD(PalWindowHandleInfo, nativeWindow)},
        {"nativeHandle1", {16, 8}, FIELD(PalWindowHandleInfo, nativeHandle1)},
        {"nativeHandle2", {24, 8}, FIELD(PalWindowHandleInfo, nativeHandle2)},
        {"nativeHandle3", {32, 8}, FIELD(PalWindowHandleInfo, nativeHandle3)}};

    FieldInfo windowCreateInfoFields[] = {
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
        {"center", {60, 4}, FIELD(PalWindowCreateInfo, center)}};

    StructInfo monitorInfo = {0};
    monitorInfo.name = "PalMonitorInfo";
    monitorInfo.fields = monitorInfoFields;
    monitorInfo.fieldCount = ARRAY_SIZE(monitorInfoFields);
    monitorInfo.expected.alignof = 4;
    monitorInfo.expected.size = 64;
    monitorInfo.expected.padding = 0;
    monitorInfo.actual = STRUCT(PalMonitorInfo);

    StructInfo monitorMode = {0};
    monitorMode.name = "PalMonitorMode";
    monitorMode.fields = monitorModeFields;
    monitorMode.fieldCount = ARRAY_SIZE(monitorModeFields);
    monitorMode.expected.alignof = 4;
    monitorMode.expected.size = 16;
    monitorMode.expected.padding = 0;
    monitorMode.actual = STRUCT(PalMonitorMode);

    StructInfo flashInfo = {0};
    flashInfo.name = "PalFlashInfo";
    flashInfo.fields = flashInfoFields;
    flashInfo.fieldCount = ARRAY_SIZE(flashInfoFields);
    flashInfo.expected.alignof = 4;
    flashInfo.expected.size = 12;
    flashInfo.expected.padding = 0;
    flashInfo.actual = STRUCT(PalFlashInfo);

    StructInfo iconCreateInfo = {0};
    iconCreateInfo.name = "PalIconCreateInfo";
    iconCreateInfo.fields = iconCreateInfoFields;
    iconCreateInfo.fieldCount = ARRAY_SIZE(iconCreateInfoFields);
    iconCreateInfo.expected.alignof = 8;
    iconCreateInfo.expected.size = 16;
    iconCreateInfo.expected.padding = 0;
    iconCreateInfo.actual = STRUCT(PalIconCreateInfo);

    StructInfo cursorCreateInfo = {0};
    cursorCreateInfo.name = "PalCursorCreateInfo";
    cursorCreateInfo.fields = cursorCreateInfoFields;
    cursorCreateInfo.fieldCount = ARRAY_SIZE(cursorCreateInfoFields);
    cursorCreateInfo.expected.alignof = 8;
    cursorCreateInfo.expected.size = 24;
    cursorCreateInfo.expected.padding = 0;
    cursorCreateInfo.actual = STRUCT(PalCursorCreateInfo);

    StructInfo windowHandleInfo = {0};
    windowHandleInfo.name = "PalWindowHandleInfo";
    windowHandleInfo.fields = windowHandleInfoFields;
    windowHandleInfo.fieldCount = ARRAY_SIZE(windowHandleInfoFields);
    windowHandleInfo.expected.alignof = 8;
    windowHandleInfo.expected.size = 40;
    windowHandleInfo.expected.padding = 0;
    windowHandleInfo.actual = STRUCT(PalWindowHandleInfo);

    StructInfo windowCreateInfo = {0};
    windowCreateInfo.name = "PalWindowCreateInfo";
    windowCreateInfo.fields = windowCreateInfoFields;
    windowCreateInfo.fieldCount = ARRAY_SIZE(windowCreateInfoFields);
    windowCreateInfo.expected.alignof = 8;
    windowCreateInfo.expected.size = 64;
    windowCreateInfo.expected.padding = 0;
    windowCreateInfo.actual = STRUCT(PalWindowCreateInfo);

    PalBool status = checkABI(&monitorInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&monitorMode, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&flashInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&iconCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&cursorCreateInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&windowHandleInfo, flags);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&windowCreateInfo, flags);
}