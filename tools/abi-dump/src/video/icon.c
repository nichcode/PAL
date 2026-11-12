
#include "helpers.h"
#include "pal2/video/icon.h"

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