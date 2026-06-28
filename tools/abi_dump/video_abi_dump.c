
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_video.h"

static void monitorDump(PalBool verbose)
{
    uint32_t xSize = 64;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;
    uint32_t xOffset4 = 12;
    uint32_t xOffset5 = 16;
    uint32_t xOffset6 = 20;
    uint32_t xOffset7 = 24;
    uint32_t xOffset8 = 28;
    uint32_t xOffset9 = 32;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalMonitorInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalMonitorInfo);
    uint32_t yOffset1 = offsetof(PalMonitorInfo, x);
    uint32_t yOffset2 = offsetof(PalMonitorInfo, y);
    uint32_t yOffset3 = offsetof(PalMonitorInfo, width);
    uint32_t yOffset4 = offsetof(PalMonitorInfo, height);
    uint32_t yOffset5 = offsetof(PalMonitorInfo, dpi);
    uint32_t yOffset6 = offsetof(PalMonitorInfo, refreshRate);
    uint32_t yOffset7 = offsetof(PalMonitorInfo, orientation);
    uint32_t yOffset8 = offsetof(PalMonitorInfo, primary);
    uint32_t yOffset9 = offsetof(PalMonitorInfo, name);
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

    palLog(nullptr, "struct: PalMonitorInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field          Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size           %u           %u", xSize, ySize);
        palLog(nullptr, "align          %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding        %u            %u", xPadding, yPadding);
        palLog(nullptr, "x @            %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "y @            %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "width @        %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "height @       %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "dpi @          %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "refreshRate @  %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "orientation @  %u           %u", xOffset7, yOffset7);
        palLog(nullptr, "primary @      %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "name @         %u           %u", xOffset9, yOffset9);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void monitorModeDump(PalBool verbose)
{
    uint32_t xSize = 16;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;
    uint32_t xOffset4 = 12;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalMonitorMode);
    uint32_t yAlign = PAL_ALIGNOF(PalMonitorMode);
    uint32_t yOffset1 = offsetof(PalMonitorMode, bpp);
    uint32_t yOffset2 = offsetof(PalMonitorMode, refreshRate);
    uint32_t yOffset3 = offsetof(PalMonitorMode, width);
    uint32_t yOffset4 = offsetof(PalMonitorMode, height);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xOffset4 == yOffset4      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalMonitorMode");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field           Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size            %u           %u", xSize, ySize);
        palLog(nullptr, "align           %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding         %u            %u", xPadding, yPadding);
        palLog(nullptr, "bpp @           %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "refreshRate @   %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "width @         %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "height @        %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void flashDump(PalBool verbose)
{
    uint32_t xSize = 16;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 12;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalFlashInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalFlashInfo);
    uint32_t yOffset1 = offsetof(PalFlashInfo, flags);
    uint32_t yOffset2 = offsetof(PalFlashInfo, interval);
    uint32_t yOffset3 = offsetof(PalFlashInfo, count);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalFlashInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field        Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size         %u           %u", xSize, ySize);
        palLog(nullptr, "align        %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding      %u            %u", xPadding, yPadding);
        palLog(nullptr, "flags @      %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "interval @   %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "count @      %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void iconDump(PalBool verbose)
{
    uint32_t xSize = 16;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 12;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalIconCreateInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalIconCreateInfo);
    uint32_t yOffset1 = offsetof(PalIconCreateInfo, pixels);
    uint32_t yOffset2 = offsetof(PalIconCreateInfo, width);
    uint32_t yOffset3 = offsetof(PalIconCreateInfo, height);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalIconCreateInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field        Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size         %u           %u", xSize, ySize);
        palLog(nullptr, "align        %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding      %u            %u", xPadding, yPadding);
        palLog(nullptr, "pixels @     %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "width @      %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "height @     %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void cursorDump(PalBool verbose)
{
    uint32_t xSize = 24;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 12;
    uint32_t xOffset4 = 16;
    uint32_t xOffset5 = 20;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalCursorCreateInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalCursorCreateInfo);
    uint32_t yOffset1 = offsetof(PalCursorCreateInfo, pixels);
    uint32_t yOffset2 = offsetof(PalCursorCreateInfo, width);
    uint32_t yOffset3 = offsetof(PalCursorCreateInfo, height);
    uint32_t yOffset4 = offsetof(PalCursorCreateInfo, xHotspot);
    uint32_t yOffset5 = offsetof(PalCursorCreateInfo, yHotspot);
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
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalCursorCreateInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field        Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size         %u           %u", xSize, ySize);
        palLog(nullptr, "align        %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding      %u            %u", xPadding, yPadding);
        palLog(nullptr, "pixels @     %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "width @      %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "height @     %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "xHotspot @   %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "yHotspot @   %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void windowInfoDump(PalBool verbose)
{
    uint32_t xSize = 40;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xOffset4 = 24;
    uint32_t xOffset5 = 32;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalWindowHandleInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalWindowHandleInfo);
    uint32_t yOffset1 = offsetof(PalWindowHandleInfo, nativeInstance);
    uint32_t yOffset2 = offsetof(PalWindowHandleInfo, nativeWindow);
    uint32_t yOffset3 = offsetof(PalWindowHandleInfo, nativeHandle1);
    uint32_t yOffset4 = offsetof(PalWindowHandleInfo, nativeHandle2);
    uint32_t yOffset5 = offsetof(PalWindowHandleInfo, nativeHandle3);
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
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalWindowHandleInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field             Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size              %u           %u", xSize, ySize);
        palLog(nullptr, "align             %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding           %u            %u", xPadding, yPadding);
        palLog(nullptr, "nativeInstance @  %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "nativeWindow @    %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "nativeHandle1 @   %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "nativeHandle2 @   %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "nativeHandle3 @   %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void windowDump(PalBool verbose)
{
    uint32_t xSize = 72;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xOffset4 = 24;
    uint32_t xOffset5 = 32;
    uint32_t xOffset6 = 40;
    uint32_t xOffset7 = 44;
    uint32_t xOffset8 = 48;
    uint32_t xOffset9 = 52;
    uint32_t xOffset10 = 56;
    uint32_t xOffset11 = 60;
    uint32_t xOffset12 = 64;
    uint32_t xOffset13 = 68;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalWindowCreateInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalWindowCreateInfo);
    uint32_t yOffset1 = offsetof(PalWindowCreateInfo, style);
    uint32_t yOffset2 = offsetof(PalWindowCreateInfo, title);
    uint32_t yOffset3 = offsetof(PalWindowCreateInfo, monitor);
    uint32_t yOffset4 = offsetof(PalWindowCreateInfo, appName);
    uint32_t yOffset5 = offsetof(PalWindowCreateInfo, instanceName);
    uint32_t yOffset6 = offsetof(PalWindowCreateInfo, fbConfigBackend);
    uint32_t yOffset7 = offsetof(PalWindowCreateInfo, fbConfigIndex);
    uint32_t yOffset8 = offsetof(PalWindowCreateInfo, width);
    uint32_t yOffset9 = offsetof(PalWindowCreateInfo, height);
    uint32_t yOffset10 = offsetof(PalWindowCreateInfo, show);
    uint32_t yOffset13 = offsetof(PalWindowCreateInfo, center);
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
        xOffset10 == yOffset10    &&
        xOffset13 == yOffset13    &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalWindowCreateInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field              Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size               %u           %u", xSize, ySize);
        palLog(nullptr, "align              %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding            %u            %u", xPadding, yPadding);
        palLog(nullptr, "style @            %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "title @            %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "monitor @          %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "appName @          %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "instanceName @     %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "fbConfigBackend @  %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "fbConfigIndex @    %u           %u", xOffset7, yOffset7);
        palLog(nullptr, "width @            %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "height @           %u           %u", xOffset9, yOffset9);
        palLog(nullptr, "show @             %u           %u", xOffset10, yOffset10);
        palLog(nullptr, "center @           %u           %u", xOffset13, yOffset13);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

void videoABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Video ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    monitorDump(verbose);
    monitorModeDump(verbose);
    flashDump(verbose);
    iconDump(verbose);
    cursorDump(verbose);
    windowInfoDump(verbose);
    windowDump(verbose);
}