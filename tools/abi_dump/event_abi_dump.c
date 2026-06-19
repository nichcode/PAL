
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#include "dumps.h"
#include "pal/pal_event.h"

static void eventDump(PalBool verbose)
{
    uint32_t xSize = 32;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xOffset4 = 24;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalEvent);
    uint32_t yAlign = PAL_ALIGNOF(PalEvent);
    uint32_t yOffset1 = offsetof(PalEvent, userId);
    uint32_t yOffset2 = offsetof(PalEvent, data);
    uint32_t yOffset3 = offsetof(PalEvent, data2);
    uint32_t yOffset4 = offsetof(PalEvent, type);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      && 
        xOffset4 == yOffset4      && 
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalEvent");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field         Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size          %u          %u", xSize, ySize);
        palLog(nullptr, "align         %u           %u", xAlign, yAlign);
        palLog(nullptr, "padding       %u           %u", xPadding, yPadding);
        palLog(nullptr, "userId @      %u           %u", xOffset1, yOffset1);
        palLog(nullptr, "data @        %u           %u", xOffset2, yOffset2);
        palLog(nullptr, "data2 @       %u          %u", xOffset3, yOffset3);
        palLog(nullptr, "type @        %u          %u", xOffset4, yOffset4);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void eventQueueDump(PalBool verbose)
{
    uint32_t xSize = 24;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalEventQueue);
    uint32_t yAlign = PAL_ALIGNOF(PalEventQueue);
    uint32_t yOffset1 = offsetof(PalEventQueue, push);
    uint32_t yOffset2 = offsetof(PalEventQueue, poll);
    uint32_t yOffset3 = offsetof(PalEventQueue, userData);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      && 
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalEventQueue");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field         Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size          %u          %u", xSize, ySize);
        palLog(nullptr, "align         %u           %u", xAlign, yAlign);
        palLog(nullptr, "padding       %u           %u", xPadding, yPadding);
        palLog(nullptr, "push @        %u           %u", xOffset1, yOffset1);
        palLog(nullptr, "poll @        %u           %u", xOffset2, yOffset2);
        palLog(nullptr, "userData @    %u          %u", xOffset3, yOffset3);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void eventCreateInfoDump(PalBool verbose)
{
    uint32_t xSize = 32;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;
    uint32_t xOffset4 = 24;
    uint32_t xPadding = 0;

    uint32_t ySize = sizeof(PalEventDriverCreateInfo);
    uint32_t yAlign = PAL_ALIGNOF(PalEventDriverCreateInfo);
    uint32_t yOffset1 = offsetof(PalEventDriverCreateInfo, allocator);
    uint32_t yOffset2 = offsetof(PalEventDriverCreateInfo, queue);
    uint32_t yOffset3 = offsetof(PalEventDriverCreateInfo, callback);
    uint32_t yOffset4 = offsetof(PalEventDriverCreateInfo, userData);
    uint32_t yPadding = (yAlign - (ySize % yAlign)) % yAlign;

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3      && 
        xOffset4 == yOffset4      && 
        xPadding == yPadding) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "struct: PalEventDriverCreateInfo");
    if (verbose) {
        palLog(nullptr, "===========================================");
        palLog(nullptr, "Field         Expected     Actual");
        palLog(nullptr, "===========================================");

        palLog(nullptr, "size          %u          %u", xSize, ySize);
        palLog(nullptr, "align         %u           %u", xAlign, yAlign);
        palLog(nullptr, "padding       %u           %u", xPadding, yPadding);
        palLog(nullptr, "allocator @   %u           %u", xOffset1, yOffset1);
        palLog(nullptr, "queue @       %u           %u", xOffset2, yOffset2);
        palLog(nullptr, "callback @    %u          %u", xOffset3, yOffset3);
        palLog(nullptr, "userData @    %u          %u", xOffset4, yOffset4);
        palLog(nullptr, "===========================================");
    }

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

void eventABIDump(PalBool verbose)
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Event ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    eventDump(verbose);
    eventQueueDump(verbose);
    eventCreateInfoDump(verbose);
}