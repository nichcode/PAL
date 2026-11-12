
#include "helpers.h"
#include "pal2/opengl/init.h"

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