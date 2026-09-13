
#include "helpers.h"
#include "pal2/pal_event.h"

bool eventStruct(void)
{
    FieldInfo fields[] = {
        {"data", {0, 8}, FIELD(PalEvent, data)},
        {"data2", {8, 8}, FIELD(PalEvent, data2)},
        {"userId", {16, 4}, FIELD(PalEvent, userId)},
        {"type", {20, 4}, FIELD(PalEvent, type)}
    };

    StructInfo info = {0};
    info.name = "PalEvent";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalEvent);

    return dumpStruct(&info);
}