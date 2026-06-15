
#include "dumps.h"

static void palVersionDump()
{
    uint32_t expectedSize = 12;
    uint32_t expectedAlignof = 4;
    uint32_t expectedMajorOffset = 0;
    uint32_t expectedMinorOffset = 4;
    uint32_t expectedBuildOffset = 8;

    uint32_t size = sizeof(PalVersion);
    uint32_t alignof = PAL_ALIGNOF(PalVersion);
    uint32_t majorOffset = offsetof(PalVersion, major);
    uint32_t minorOffset = offsetof(PalVersion, minor);
    uint32_t buildOffset = offsetof(PalVersion, build);

    const char* result = s_FailedString;
    // clang-format off
    if (expectedSize == size               && 
        expectedAlignof == alignof         && 
        expectedMajorOffset == majorOffset && 
        expectedMinorOffset == minorOffset && 
        expectedBuildOffset == buildOffset) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "PalVersion");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field         Expected     Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "sizeof        %u          %u", expectedSize, size);
    palLog(nullptr, "alignof       %u           %u", expectedAlignof, alignof);
    palLog(nullptr, "major @       %u           %u", expectedMajorOffset, majorOffset);
    palLog(nullptr, "minor @       %u           %u", expectedMinorOffset, minorOffset);
    palLog(nullptr, "build @       %u           %u", expectedBuildOffset, buildOffset);
    palLog(nullptr, "===========================================");

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void palAllocatorDump()
{
    uint32_t expectedSize = 24;
    uint32_t expectedAlignof = 8;
    uint32_t expectedAllocateOffset = 0;
    uint32_t expectedFreeOffset = 8;
    uint32_t expectedUserDataOffset = 16;

    uint32_t size = sizeof(PalAllocator);
    uint32_t alignof = PAL_ALIGNOF(PalAllocator);
    uint32_t allocateOffset = offsetof(PalAllocator, allocate);
    uint32_t freeOffset = offsetof(PalAllocator, free);
    uint32_t userDataOffset = offsetof(PalAllocator, userData);

    const char* result = s_FailedString;
    // clang-format off
    if (expectedSize == size                        && 
        expectedAlignof == alignof                  && 
        expectedAllocateOffset == allocateOffset    && 
        expectedFreeOffset == freeOffset            && 
        expectedUserDataOffset == userDataOffset) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "PalAllocator");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field         Expected     Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "sizeof        %u          %u", expectedSize, size);
    palLog(nullptr, "alignof       %u           %u", expectedAlignof, alignof);
    palLog(nullptr, "allocate @    %u           %u", expectedAllocateOffset, allocateOffset);
    palLog(nullptr, "free @        %u           %u", expectedFreeOffset, freeOffset);
    palLog(nullptr, "userData @    %u          %u", expectedUserDataOffset, userDataOffset);
    palLog(nullptr, "===========================================");

    palLog(nullptr, "Status: %s", result);
    palLog(nullptr, "");
}

static void palLoggerDump()
{
    uint32_t expectedSize = 16;
    uint32_t expectedAlignof = 8;
    uint32_t expectedCallbackOffset = 0;
    uint32_t expectedUserDataOffset = 8;

    uint32_t size = sizeof(PalLogger);
    uint32_t alignof = PAL_ALIGNOF(PalLogger);
    uint32_t callbackOffset = offsetof(PalLogger, callback);
    uint32_t userDataOffset = offsetof(PalLogger, userData);

    const char* result = s_FailedString;
    // clang-format off
    if (expectedSize == size                        && 
        expectedAlignof == alignof                  && 
        expectedCallbackOffset == callbackOffset    &&
        expectedUserDataOffset == userDataOffset) {
        result = s_PassedString;
    }
    // clang-format on

    palLog(nullptr, "PalLogger");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Field         Expected     Actual");
    palLog(nullptr, "===========================================");

    palLog(nullptr, "sizeof        %u          %u", expectedSize, size);
    palLog(nullptr, "alignof       %u           %u", expectedAlignof, alignof);
    palLog(nullptr, "allocate @    %u           %u", expectedCallbackOffset, callbackOffset);
    palLog(nullptr, "userData @    %u           %u", expectedUserDataOffset, userDataOffset);
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

    palVersionDump();
    palAllocatorDump();
    palLoggerDump();
}