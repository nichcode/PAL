
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_opengl.h"

static void infoDump(PalBool verbose)
{
    uint32_t xSize = 184;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 12;
    uint32_t xOffset4 = 16;
    uint32_t xOffset5 = 20;
    uint32_t xOffset6 = 24;
    uint32_t xOffset7 = 56;
    uint32_t xOffset8 = 120;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalGLInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalGLInfo);
    uint32_t yOffset1 = offsetof(PalGLInfo, extensions);
    uint32_t yOffset2 = offsetof(PalGLInfo, major);
    uint32_t yOffset3 = offsetof(PalGLInfo, minor);
    uint32_t yOffset4 = offsetof(PalGLInfo, backend);
    uint32_t yOffset5 = offsetof(PalGLInfo, api);
    uint32_t yOffset6 = offsetof(PalGLInfo, vendor);
    uint32_t yOffset7 = offsetof(PalGLInfo, graphicsCard);
    uint32_t yOffset8 = offsetof(PalGLInfo, version);
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
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalGLInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field           Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size            %u           %u", xSize, ySize);
        palLog(nullptr, "align           %u             %u", xAlign, yAlign);
        palLog(nullptr, "padding         %u             %u", xPadding, yPadding);
        palLog(nullptr, "extensions @    %u             %u", xOffset1, yOffset1);
        palLog(nullptr, "major @         %u             %u", xOffset2, yOffset2);
        palLog(nullptr, "minor @         %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "backend @       %u            %u", xOffset4, yOffset4);
        palLog(nullptr, "api @           %u            %u", xOffset5, yOffset5);
        palLog(nullptr, "vendor @        %u            %u", xOffset6, yOffset6);
        palLog(nullptr, "graphicsCard @  %u            %u", xOffset7, yOffset7);
        palLog(nullptr, "version @       %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void configDump(PalBool verbose)
{
    uint32_t xSize = 28;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;
    uint32_t xOffset4 = 12;
    uint32_t xOffset5 = 14;
    uint32_t xOffset6 = 16;
    uint32_t xOffset7 = 18;
    uint32_t xOffset8 = 20;
    uint32_t xOffset9 = 22;
    uint32_t xOffset10 = 24;
    uint32_t xOffset11 = 26;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalGLFBConfig);
    uint32_t yAlign = PAL_ALIGNOF(PalGLFBConfig);
    uint32_t yOffset1 = offsetof(PalGLFBConfig, doubleBuffer);
    uint32_t yOffset2 = offsetof(PalGLFBConfig, stereo);
    uint32_t yOffset3 = offsetof(PalGLFBConfig, sRGB);
    uint32_t yOffset4 = offsetof(PalGLFBConfig, index);
    uint32_t yOffset5 = offsetof(PalGLFBConfig, redBits);
    uint32_t yOffset6 = offsetof(PalGLFBConfig, greenBits);
    uint32_t yOffset7 = offsetof(PalGLFBConfig, blueBits);
    uint32_t yOffset8 = offsetof(PalGLFBConfig, alphaBits);
    uint32_t yOffset9 = offsetof(PalGLFBConfig, depthBits);
    uint32_t yOffset10 = offsetof(PalGLFBConfig, stencilBits);
    uint32_t yOffset11 = offsetof(PalGLFBConfig, samples);
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
        xOffset11 == yOffset11    &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalGLFBConfig");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field           Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size            %u           %u", xSize, ySize);
        palLog(nullptr, "align           %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding         %u            %u", xPadding, yPadding);
        palLog(nullptr, "doubleBuffer @  %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "stereo @        %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "sRGB @          %u            %u", xOffset3, yOffset3);
        palLog(nullptr, "index @         %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "redBits @       %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "greenBits @     %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "blueBits @      %u           %u", xOffset7, yOffset7);
        palLog(nullptr, "alphaBits @     %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "depthBits @     %u           %u", xOffset9, yOffset9);
        palLog(nullptr, "stencilBits @   %u           %u", xOffset10, yOffset10);
        palLog(nullptr, "samples @       %u           %u", xOffset11, yOffset11);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void windowDump(PalBool verbose)
{
    uint32_t xSize = 16;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalGLWindow);
    uint32_t yAlign = PAL_ALIGNOF(PalGLWindow);
    uint32_t yOffset1 = offsetof(PalGLWindow, instance);
    uint32_t yOffset2 = offsetof(PalGLWindow, window);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalGLWindow");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field       Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size        %u           %u", xSize, ySize);
        palLog(nullptr, "align       %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding     %u            %u", xPadding, yPadding);
        palLog(nullptr, "instance @  %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "window @    %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void contextDump(PalBool verbose)
{
    uint32_t xSize = 56;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xOffset4 = 24;
    uint32_t xOffset5 = 28;
    uint32_t xOffset6 = 32;
    uint32_t xOffset7 = 36;
    uint32_t xOffset8 = 40;
    uint32_t xOffset9 = 44;
    uint32_t xOffset10 = 48;
    uint32_t xOffset11 = 52;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalGLContextCreateInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalGLContextCreateInfo);
    uint32_t yOffset1 = offsetof(PalGLContextCreateInfo, window);
    uint32_t yOffset2 = offsetof(PalGLContextCreateInfo, fbConfig);
    uint32_t yOffset3 = offsetof(PalGLContextCreateInfo, shareContext);
    uint32_t yOffset4 = offsetof(PalGLContextCreateInfo, profile);
    uint32_t yOffset5 = offsetof(PalGLContextCreateInfo, reset);
    uint32_t yOffset6 = offsetof(PalGLContextCreateInfo, release);
    uint32_t yOffset7 = offsetof(PalGLContextCreateInfo, forward);
    uint32_t yOffset8 = offsetof(PalGLContextCreateInfo, noError);
    uint32_t yOffset9 = offsetof(PalGLContextCreateInfo, debug);
    uint32_t yOffset10 = offsetof(PalGLContextCreateInfo, major);
    uint32_t yOffset11 = offsetof(PalGLContextCreateInfo, minor);
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
        xOffset11 == yOffset11    &&
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalGLContextCreateInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field           Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size            %u           %u", xSize, ySize);
        palLog(nullptr, "align           %u            %u", xAlign, yAlign);
        palLog(nullptr, "padding         %u            %u", xPadding, yPadding);
        palLog(nullptr, "window @        %u            %u", xOffset1, yOffset1);
        palLog(nullptr, "fbConfig @      %u            %u", xOffset2, yOffset2);
        palLog(nullptr, "shareContext @  %u           %u", xOffset3, yOffset3);
        palLog(nullptr, "profile @       %u           %u", xOffset4, yOffset4);
        palLog(nullptr, "reset @         %u           %u", xOffset5, yOffset5);
        palLog(nullptr, "release @       %u           %u", xOffset6, yOffset6);
        palLog(nullptr, "forward @       %u           %u", xOffset7, yOffset7);
        palLog(nullptr, "noError @       %u           %u", xOffset8, yOffset8);
        palLog(nullptr, "debug @         %u           %u", xOffset9, yOffset9);
        palLog(nullptr, "major @         %u           %u", xOffset10, yOffset10);
        palLog(nullptr, "minor @         %u           %u", xOffset11, yOffset11);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

void openglABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Opengl ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    infoDump(verbose);
    configDump(verbose);
    windowDump(verbose);
    contextDump(verbose);
}