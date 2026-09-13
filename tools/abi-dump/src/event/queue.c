
#include "helpers.h"
#include "pal2/pal_event.h"

bool queueStruct(void)
{
    FieldInfo fields[] = {
        {"push", {0, 8}, FIELD(PalEventQueue, push)},
        {"poll", {8, 8}, FIELD(PalEventQueue, poll)},
        {"userData", {16, 8}, FIELD(PalEventQueue, userData)}
    };

    StructInfo info = {0};
    info.name = "PalEventQueue";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 24;
    info.expected.padding = 0;
    info.actual = STRUCT(PalEventQueue);

    return dumpStruct(&info);
}