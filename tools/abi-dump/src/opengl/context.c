
#include "helpers.h"
#include "pal2/opengl/context.h"

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