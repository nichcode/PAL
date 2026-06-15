
#include "dumps.h"

static void versionDump()
{
    uint32_t xSize = 12;
    uint32_t xAlign = 4;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 4;
    uint32_t xOffset3 = 8;

    uint32_t ySize = sizeof(PalVersion);
    uint32_t yAlign = PAL_ALIGNOF(PalVersion);
    uint32_t yOffset1 = offsetof(PalVersion, major);
    uint32_t yOffset2 = offsetof(PalVersion, minor);
    uint32_t yOffset3 = offsetof(PalVersion, build);

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize            && 
        xAlign == yAlign          && 
        xOffset1 == yOffset1      && 
        xOffset2 == yOffset2      && 
        xOffset3 == yOffset3) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "PalVersion");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field         Expected     Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "size          %u          %u", xSize, ySize);
    palLog(nullptr, "align         %u           %u", xAlign, yAlign);
    palLog(nullptr, "major @       %u           %u", xOffset1, yOffset1);
    palLog(nullptr, "minor @       %u           %u", xOffset2, yOffset2);
    palLog(nullptr, "build @       %u           %u", xOffset3, yOffset3);
    palLog(nullptr, "===========================================");

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void allocatorDump()
{
    uint32_t xSize = 24;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;
    uint32_t xOffset3 = 16;

    uint32_t ySize = sizeof(PalAllocator);
    uint32_t yAlign = PAL_ALIGNOF(PalAllocator);
    uint32_t yOffset1 = offsetof(PalAllocator, allocate);
    uint32_t yOffset2 = offsetof(PalAllocator, free);
    uint32_t yOffset3 = offsetof(PalAllocator, userData);

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize          && 
        xAlign == yAlign        && 
        xOffset1 == yOffset1    && 
        xOffset2 == yOffset2    && 
        xOffset3 == yOffset3) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "PalAllocator");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field         Expected     Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "size          %u          %u", xSize, ySize);
    palLog(nullptr, "align         %u           %u", xAlign, yAlign);
    palLog(nullptr, "allocate @    %u           %u", xOffset1, yOffset1);
    palLog(nullptr, "free @        %u           %u", xOffset2, yOffset2);
    palLog(nullptr, "userData @    %u          %u", xOffset3, yOffset3);
    palLog(nullptr, "===========================================");

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void loggerDump()
{
    uint32_t xSize = 16;
    uint32_t xAlign = 8;
    uint32_t xOffset1 = 0;
    uint32_t xOffset2 = 8;

    uint32_t ySize = sizeof(PalLogger);
    uint32_t yAlign = PAL_ALIGNOF(PalLogger);
    uint32_t yOffset1 = offsetof(PalLogger, callback);
    uint32_t yOffset2 = offsetof(PalLogger, userData);

    const char* result = s_FailedString;
    // clang-format off
    if (xSize == ySize          && 
        xAlign == yAlign        && 
        xOffset1 == yOffset1    &&
        xOffset2 == yOffset2) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "PalLogger");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field         Expected     Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "size          %u          %u", xSize, ySize);
    palLog(nullptr, "align         %u           %u", xAlign, yAlign);
    palLog(nullptr, "allocate @    %u           %u", xOffset1, yOffset1);
    palLog(nullptr, "userData @    %u           %u", xOffset2, yOffset2);
    palLog(nullptr, "===========================================");

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

void dumpCoreABI()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Core System ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    versionDump();
    allocatorDump();
    loggerDump();
}