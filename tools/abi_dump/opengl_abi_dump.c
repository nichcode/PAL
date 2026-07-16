
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_opengl.h"

PalBool openglABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Opengl ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    FieldInfo glInfoFields[] = {
        { "extensions", {0, 8}, FIELD(PalGLInfo, extensions) },
        { "major", {8, 4}, FIELD(PalGLInfo, major) },
        { "minor", {12, 4}, FIELD(PalGLInfo, minor) },
        { "backend", {16, 4}, FIELD(PalGLInfo, backend) },
        { "api", {20, 4}, FIELD(PalGLInfo, api) },
        { "vendor", {24, 32}, FIELD(PalGLInfo, vendor) },
        { "graphicsCard", {56, 64}, FIELD(PalGLInfo, graphicsCard) },
        { "version", {120, 64}, FIELD(PalGLInfo, version) }
    };

    FieldInfo fbConfigFields[] = {
        { "doubleBuffer", {0, 4}, FIELD(PalGLFBConfig, doubleBuffer) },
        { "stereo", {4, 4}, FIELD(PalGLFBConfig, stereo) },
        { "sRGB", {8, 4}, FIELD(PalGLFBConfig, sRGB) },
        { "index", {12, 2}, FIELD(PalGLFBConfig, index) },
        { "redBits", {14, 2}, FIELD(PalGLFBConfig, redBits) },
        { "greenBits", {16, 2}, FIELD(PalGLFBConfig, greenBits) },
        { "blueBits", {18, 2}, FIELD(PalGLFBConfig, blueBits) },
        { "alphaBits", {20, 2}, FIELD(PalGLFBConfig, alphaBits) },
        { "depthBits", {22, 2}, FIELD(PalGLFBConfig, depthBits) },
        { "stencilBits", {24, 2}, FIELD(PalGLFBConfig, stencilBits) },
        { "samples", {26, 2}, FIELD(PalGLFBConfig, samples) }
    };

    FieldInfo windowFields[] = {
        { "instance", {0, 8}, FIELD(PalGLWindow, instance) },
        { "window", {8, 8}, FIELD(PalGLWindow, window) }
    };

    FieldInfo contextCreateInfoFields[] = {
        { "window", {0, 8}, FIELD(PalGLContextCreateInfo, window) },
        { "fbConfig", {8, 8}, FIELD(PalGLContextCreateInfo, fbConfig) },
        { "shareContext", {16, 8}, FIELD(PalGLContextCreateInfo, shareContext) },
        { "profile", {24, 4}, FIELD(PalGLContextCreateInfo, profile) },
        { "reset", {28, 4}, FIELD(PalGLContextCreateInfo, reset) },
        { "release", {32, 4}, FIELD(PalGLContextCreateInfo, release) },
        { "forward", {36, 4}, FIELD(PalGLContextCreateInfo, forward) },
        { "noError", {40, 4}, FIELD(PalGLContextCreateInfo, noError) },
        { "debug", {44, 4}, FIELD(PalGLContextCreateInfo, debug) },
        { "major", {48, 4}, FIELD(PalGLContextCreateInfo, major) },
        { "minor", {52, 4}, FIELD(PalGLContextCreateInfo, minor) }
    };

    StructInfo glInfo = {0};
    glInfo.name = "PalGLInfo";
    glInfo.fields = glInfoFields;
    glInfo.fieldCount = ARRAY_SIZE(glInfoFields);
    glInfo.expected.alignof = 8;
    glInfo.expected.size = 184;
    glInfo.expected.padding = 0;
    glInfo.actual = STRUCT(PalGLInfo);

    StructInfo fbConfig = {0};
    fbConfig.name = "PalGLFBConfig";
    fbConfig.fields = fbConfigFields;
    fbConfig.fieldCount = ARRAY_SIZE(fbConfigFields);
    fbConfig.expected.alignof = 4;
    fbConfig.expected.size = 28;
    fbConfig.expected.padding = 0;
    fbConfig.actual = STRUCT(PalGLFBConfig);

    StructInfo window = {0};
    window.name = "PalGLWindow";
    window.fields = windowFields;
    window.fieldCount = ARRAY_SIZE(windowFields);
    window.expected.alignof = 8;
    window.expected.size = 16;
    window.expected.padding = 0;
    window.actual = STRUCT(PalGLWindow);

    StructInfo contextCreateInfo = {0};
    contextCreateInfo.name = "PalGLContextCreateInfo";
    contextCreateInfo.fields = contextCreateInfoFields;
    contextCreateInfo.fieldCount = ARRAY_SIZE(contextCreateInfoFields);
    contextCreateInfo.expected.alignof = 8;
    contextCreateInfo.expected.size = 56;
    contextCreateInfo.expected.padding = 0;
    contextCreateInfo.actual = STRUCT(PalGLContextCreateInfo);

    PalBool status = checkABI(&glInfo, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&fbConfig, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    status = checkABI(&window, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    return checkABI(&contextCreateInfo, verbose);
}