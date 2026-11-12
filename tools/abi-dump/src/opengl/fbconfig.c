
#include "helpers.h"
#include "pal2/opengl/fbconfig.h"

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