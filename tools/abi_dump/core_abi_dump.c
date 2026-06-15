
#include "dumps.h"

static void palVersionDump()
{
    uint32_t size = sizeof(PalVersion);
    uint32_t alignof = PAL_ALIGNOF(PalVersion);
    uint32_t majorOffset = offsetof(PalVersion, major);
    uint32_t minorOffset = offsetof(PalVersion, minor);
    uint32_t buildOffset = offsetof(PalVersion, build);

    palLog(nullptr, "PalVersion");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field      Expected    Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "sizeof     %u         %u", 12, size);
    palLog(nullptr, "alignof    %u          %u", 4, alignof);
    palLog(nullptr, "major@     %u          %u", 0, majorOffset);
    palLog(nullptr, "minor@     %u          %u", 4, minorOffset);
    palLog(nullptr, "build@     %u          %u", 8, buildOffset);
    palLog(nullptr, "");
}

void dumpCoreABI()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Core System ABI Dump");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    palVersionDump();
    // PalAllocator
    // PalLogger
}