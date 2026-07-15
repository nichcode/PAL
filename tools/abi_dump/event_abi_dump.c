
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_event.h"

PalBool eventABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Event ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    FieldInfo eventFields[] = {
        { "data", {0, 8}, FIELD(PalEvent, data) },
        { "data2", {8, 8}, FIELD(PalEvent, data2) },
        { "userId", {16, 4}, FIELD(PalEvent, userId) },
        { "type", {20, 4}, FIELD(PalEvent, type) }
    };

    StructInfo eventInfo = {0};
    eventInfo.name = "PalEvent";
    eventInfo.fields = eventFields;
    eventInfo.fieldCount = ARRAY_SIZE(eventFields);
    eventInfo.expected.alignof = 8;
    eventInfo.expected.size = 24;
    eventInfo.expected.padding = 0;
    eventInfo.actual = STRUCT(PalEvent);

    PalBool status = checkABI(&eventInfo, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    FieldInfo eventQueueFields[] = {
        { "push", {0, 8}, FIELD(PalEventQueue, push) },
        { "poll", {8, 8}, FIELD(PalEventQueue, poll) },
        { "userData", {16, 8}, FIELD(PalEventQueue, userData) }
    };

    StructInfo eventQueueInfo = {0};
    eventQueueInfo.name = "PalEventQueue";
    eventQueueInfo.fields = eventQueueFields;
    eventQueueInfo.fieldCount = ARRAY_SIZE(eventQueueFields);
    eventQueueInfo.expected.alignof = 8;
    eventQueueInfo.expected.size = 24;
    eventQueueInfo.expected.padding = 0;
    eventQueueInfo.actual = STRUCT(PalEventQueue);

    status = checkABI(&eventQueueInfo, verbose);
    if (status == PAL_FALSE) {
        return status;
    }

    FieldInfo eventDriverCreateInfoFields[] = {
        { "allocator", {0, 8}, FIELD(PalEventDriverCreateInfo, allocator) },
        { "queue", {8, 8}, FIELD(PalEventDriverCreateInfo, queue) },
        { "callback", {16, 8}, FIELD(PalEventDriverCreateInfo, callback) },
        { "userData", {24, 8}, FIELD(PalEventDriverCreateInfo, userData) },
    };

    StructInfo eventDriverCreateInfo = {0};
    eventDriverCreateInfo.name = "PalEventDriverCreateInfo";
    eventDriverCreateInfo.fields = eventDriverCreateInfoFields;
    eventDriverCreateInfo.fieldCount = ARRAY_SIZE(eventDriverCreateInfoFields);
    eventDriverCreateInfo.expected.alignof = 8;
    eventDriverCreateInfo.expected.size = 24;
    eventDriverCreateInfo.expected.padding = 0;
    eventDriverCreateInfo.actual = STRUCT(PalEventDriverCreateInfo);

    status = checkABI(&eventDriverCreateInfo, verbose);
    if (status == PAL_FALSE) {
        return status;
    }
}