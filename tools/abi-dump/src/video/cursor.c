
#include "helpers.h"
#include "pal2/video/cursor.h"

bool cursorCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        {"pixels", {0, 8}, FIELD(PalCursorCreateInfo, pixels)},
        {"width", {8, 4}, FIELD(PalCursorCreateInfo, width)},
        {"height", {12, 4}, FIELD(PalCursorCreateInfo, height)},
        {"xHotspot", {16, 4}, FIELD(PalCursorCreateInfo, xHotspot)},
        {"yHotspot", {20, 4}, FIELD(PalCursorCreateInfo, yHotspot)}
    };
   
    StructInfo info = {0};
    info.name = "PalCursorCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalCursorCreateInfo);

    return dumpStruct(&info);
}