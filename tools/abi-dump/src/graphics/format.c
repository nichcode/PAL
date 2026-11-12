
#include "helpers.h"
#include "pal2/graphics/format.h"

bool formatInfoStruct(void)
{
    FieldInfo fields[] = {
        { "usages", {0, 4}, FIELD(PalFormatInfo, usages) },
        { "format", {4, 4}, FIELD(PalFormatInfo, format) },
        { "sampleCount", {8, 4}, FIELD(PalFormatInfo, sampleCount) }
    };

    StructInfo info = {0};
    info.name = "PalFormatInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 4;
    info.expected.size = 12;
    info.expected.padding = 0;
    info.actual = STRUCT(PalFormatInfo);

    return dumpStruct(&info);
}