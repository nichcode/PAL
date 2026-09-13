
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

bool eventQueueStruct(void)
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

bool eventDriverCreateInfoStruct(void)
{
    FieldInfo fields[] = {
        {"allocator", {0, 8}, FIELD(PalEventDriverCreateInfo, allocator)},
        {"queue", {8, 8}, FIELD(PalEventDriverCreateInfo, queue)},
        {"callback", {16, 8}, FIELD(PalEventDriverCreateInfo, callback)},
        {"userData", {24, 8}, FIELD(PalEventDriverCreateInfo, userData)},
    };

    StructInfo info = {0};
    info.name = "PalEventDriverCreateInfo";
    info.fields = fields;
    info.fieldCount = ARRAY_SIZE(fields);
    info.expected.align = 8;
    info.expected.size = 32;
    info.expected.padding = 0;
    info.actual = STRUCT(PalEventDriverCreateInfo);

    return dumpStruct(&info);
}

bool eventStructs(void)
{
    if (!(g_DumpFlags & ABI_DUMP_QUICK)) {
        palLog(nullptr, "");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Event ABI Dump");
        palLog(nullptr, "===========================================");
        palLog(nullptr, "");
    }

    bool status = eventStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    status = eventQueueStruct();
    if (status == PAL_FALSE) {
        return PAL_FALSE;
    }

    return eventDriverCreateInfoStruct();
}