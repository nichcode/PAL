
#include "helpers.h"
#include "pal2/video/init.h"

bool videoDriverStruct(void)
{
    FieldInfo fields[] = {
        {"features", {0, 8}, FIELD(PalVideoDriver, features)},
        {"type", {8, 4}, FIELD(PalVideoDriver, type)},
        {"id", {12, 4}, FIELD(PalVideoDriver, id)},
    };

    StructInfo info = {0};
    info.name = "PalVideoDriver";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 16;
    info.expected.padding = 0;
    info.actual = STRUCT(PalVideoDriver);

    return dumpStruct(&info);
}