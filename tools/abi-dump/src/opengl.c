
#include "helpers.h"
#include "pal2/pal_opengl.h"

bool openglInfoStruct(void)
{
    FieldInfo fields[] = {
        {"extensions", {0, 8}, FIELD(PalGLInfo, extensions)},
        {"major", {8, 4}, FIELD(PalGLInfo, major)},
        {"minor", {12, 4}, FIELD(PalGLInfo, minor)},
        {"backend", {16, 4}, FIELD(PalGLInfo, backend)},
        {"api", {20, 4}, FIELD(PalGLInfo, api)},
        {"vendor", {24, 32}, FIELD(PalGLInfo, vendor)},
        {"graphicsCard", {56, 64}, FIELD(PalGLInfo, graphicsCard)},
        {"version", {120, 64}, FIELD(PalGLInfo, version)}
    };

    StructInfo info = {0};
    info.name = "PalGLInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 184;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGLInfo);

    return dumpStruct(&info);
}

bool openglFBConfigStruct(void)
{
    FieldInfo fields[] = {
        {"doubleBuffer", {0, 4}, FIELD(PalGLFBConfig, doubleBuffer)},
        {"stereo", {4, 4}, FIELD(PalGLFBConfig, stereo)},
        {"sRGB", {8, 4}, FIELD(PalGLFBConfig, sRGB)},
        {"index", {12, 2}, FIELD(PalGLFBConfig, index)},
        {"redBits", {14, 2}, FIELD(PalGLFBConfig, redBits)},
        {"greenBits", {16, 2}, FIELD(PalGLFBConfig, greenBits)},
        {"blueBits", {18, 2}, FIELD(PalGLFBConfig, blueBits)},
        {"alphaBits", {20, 2}, FIELD(PalGLFBConfig, alphaBits)},
        {"depthBits", {22, 2}, FIELD(PalGLFBConfig, depthBits)},
        {"stencilBits", {24, 2}, FIELD(PalGLFBConfig, stencilBits)},
        {"samples", {26, 2}, FIELD(PalGLFBConfig, samples)}
    };

    StructInfo info = {0};
    info.name = "PalGLFBConfig";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 28;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGLFBConfig);

    return dumpStruct(&info);
}

bool openglWindowStruct(void)
{
    FieldInfo fields[] = {
        {"instance", {0, 8}, FIELD(PalGLWindow, instance)},
        {"window", {8, 8}, FIELD(PalGLWindow, window)}
    };

    StructInfo info = {0};
    info.name = "PalGLWindow";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGLWindow);

    return dumpStruct(&info);
}

bool contextCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        {"window", {0, 8}, FIELD(PalGLContextCreateInfo, window)},
        {"fbConfig", {8, 8}, FIELD(PalGLContextCreateInfo, fbConfig)},
        {"shareContext", {16, 8}, FIELD(PalGLContextCreateInfo, shareContext)},
        {"profile", {24, 4}, FIELD(PalGLContextCreateInfo, profile)},
        {"reset", {28, 4}, FIELD(PalGLContextCreateInfo, reset)},
        {"release", {32, 4}, FIELD(PalGLContextCreateInfo, release)},
        {"forward", {36, 4}, FIELD(PalGLContextCreateInfo, forward)},
        {"noError", {40, 4}, FIELD(PalGLContextCreateInfo, noError)},
        {"debug", {44, 4}, FIELD(PalGLContextCreateInfo, debug)},
        {"major", {48, 4}, FIELD(PalGLContextCreateInfo, major)},
        {"minor", {52, 4}, FIELD(PalGLContextCreateInfo, minor)}
    };

    StructInfo info = {0};
    info.name = "PalGLContextCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 56;
    info.expected.padding = 0;
    info.actual = STRUCT(PalGLContextCreateInfo);

    return dumpStruct(&info);
}

bool openglStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Opengl ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = openglInfoStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = openglFBConfigStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = openglWindowStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return contextCreateInfoStruct();
}