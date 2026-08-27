
#include "tests.h"

#define LOGGER_COUNT 4

#ifdef _WIN32
#define PLATFORM_SOURCE PAL_RESULT_SOURCE_WIN32
#define INVALID_ARGUMENT_NATIVE_CODE 87
#define OUT_OF_MEMORY_NATIVE_CODE 8
#define TIMEOUT_NATIVE_CODE 1460
#define INVALID_OPERATION_NATIVE_CODE 1
#define INVALID_HANDLE_NATIVE_CODE 6
#define PLATFORM_FAILURE_NATIVE_CODE 31
#else
#define PLATFORM_SOURCE PAL_RESULT_SOURCE_POSIX
#define INVALID_ARGUMENT_NATIVE_CODE 22
#define OUT_OF_MEMORY_NATIVE_CODE 12
#define TIMEOUT_NATIVE_CODE 110
#define INVALID_OPERATION_NATIVE_CODE 95
#define INVALID_HANDLE_NATIVE_CODE 9
#define PLATFORM_FAILURE_NATIVE_CODE 5
#endif // _WIN32

typedef struct {
    PalResultCode code;
    uint32_t nativeCode;
    PalResultSource source;
} ResultInfo;

typedef struct {
    uint32_t totalAllocations;
    uint32_t totalDeallocations;
} AllocatorContext;

typedef struct {
    uint64_t frequency;
    uint64_t startTime;
} Timer;

static void* PAL_CALL allocateMemory(
    void* userData,
    uint64_t size,
    uint64_t alignment)
{
    AllocatorContext* context = userData;
    void* block = palAllocate(nullptr, size, alignment);
    if (block) {
        context->totalAllocations++;
        uint32_t deallocationRequired = context->totalAllocations - context->totalDeallocations;
        palLog(nullptr, "Deallocations Required: %lu", deallocationRequired);
        return block;
    }
    return nullptr;
}

static void PAL_CALL freeMemory(
    void* userData,
    void* ptr)
{
    AllocatorContext* context = userData;
    palLog(nullptr, "Memory Address %p Freed", ptr);
    palFree(nullptr, ptr);

    context->totalDeallocations++;
    uint32_t deallocationRequired = context->totalAllocations - context->totalDeallocations;
    palLog(nullptr, "Deallocations Required: %lu", deallocationRequired);
}

static void PAL_CALL onLogger(
    void* userData,
    const char* msg)
{
    // if the logger paramter is not set to nullptr when logging in a log
    // callback, the log will be discard. Example doing this below: and
    // myLogger's callback function is the same function. This will trigger a
    // recursive call and pal will discard the log. palLog(myLogger, "%s - %s",
    // "Logger1 -", msg);

    char* name = (char*)userData;
    palLog(nullptr, "%s: %s", name, msg);
}

static inline double getTime(Timer* timer)
{
    uint64_t now = palGetPerformanceCounter();
    return (double)(now - timer->startTime) / (double)timer->frequency;
}

PalBool resultTest()
{
    // clang-format off
    ResultInfo resultInfos[PAL_RESULT_CODE_COUNT - 1] = {
        { PAL_RESULT_CODE_INVALID_ARGUMENT, INVALID_ARGUMENT_NATIVE_CODE, PLATFORM_SOURCE },
        { PAL_RESULT_CODE_OUT_OF_MEMORY, OUT_OF_MEMORY_NATIVE_CODE, PLATFORM_SOURCE},
        { PAL_RESULT_CODE_PLATFORM_FAILURE, PLATFORM_FAILURE_NATIVE_CODE, PLATFORM_SOURCE },
        { PAL_RESULT_CODE_TIMEOUT, TIMEOUT_NATIVE_CODE, PLATFORM_SOURCE },
        { PAL_RESULT_CODE_INVALID_HANDLE, 12299, PAL_RESULT_SOURCE_EGL },
        { PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED, 4294967288, PAL_RESULT_SOURCE_VULKAN },
        { PAL_RESULT_CODE_INVALID_OPERATION, INVALID_OPERATION_NATIVE_CODE, PLATFORM_SOURCE },
        { PAL_RESULT_CODE_DEVICE_LOST, 2289696773, PAL_RESULT_SOURCE_D3D12 },
        { PAL_RESULT_CODE_OUT_OF_DATE, 3294966292, PAL_RESULT_SOURCE_VULKAN }
    };
    // clang-format on

    PalBool status = PAL_TRUE;
    char buffer[256];

    for (uint32_t i = 0; i < PAL_RESULT_CODE_COUNT - 1; i++) {
        ResultInfo* info = &resultInfos[i];
        PalResult result = palMakeResult(info->code, info->source, info->nativeCode);

        // get the individual parts and check if its the same
        PalResultCode code = palGetResultCode(result);
        PalResultSource source = palGetResultSource(result);
        uint32_t nativeCode = palGetResultNativeCode(result);

        // format and log the result
        palFormatResult(result, 256, buffer);
        palLog(nullptr, buffer);

        if (info->code != code || info->source != source || info->nativeCode != nativeCode) {
            status = PAL_FALSE;
        }
    }

    return status;
}

PalBool allocatorTest()
{
    AllocatorContext context = {0};
    context.totalAllocations = 0;
    context.totalDeallocations = 0;

    PalAllocator allocator = {0};
    allocator.allocate = allocateMemory;
    allocator.free = freeMemory;
    allocator.userData = &context;

    uint32_t* ptr1 = palAllocate(&allocator, sizeof(uint32_t), 0);
    uint64_t* ptr2 = palAllocate(&allocator, sizeof(uint64_t), 0);
    uint64_t* ptr3 = palAllocate(&allocator, sizeof(uint64_t), 0);
    if (!ptr1 || !ptr2 || !ptr3) {
        return PAL_FALSE;
    }

    *ptr1 = 100000000;
    *ptr2 = 5000000000000000;
    *ptr3 = 2000000000000000;

    palLog(nullptr, "%p value: %lu", ptr1, *ptr1);
    palLog(nullptr, "%p value: %llu", ptr2, *ptr2);
    palLog(nullptr, "%p value: %llu", ptr3, *ptr3);

    palFree(&allocator, ptr1);
    palFree(&allocator, ptr2);
    palFree(&allocator, ptr3);

    return PAL_TRUE;
}

PalBool loggerTest()
{
    // clang-format off
    static const char* g_LoggerNames[LOGGER_COUNT] = {
        "Logger1",
        "Logger2",
        "Logger3",
        "Logger4"
    };
    // clang-format on

    PalLogger loggers[LOGGER_COUNT];
    for (int32_t i = 0; i < LOGGER_COUNT; i++) {
        loggers[i].callback = onLogger;
        loggers[i].userData = (void*)g_LoggerNames[i];
    }

    for (int32_t i = 0; i < LOGGER_COUNT; i++) {
        // push a log message to all loggers
        palLog(&loggers[i], "This is directed to a logger");
    }

    return PAL_TRUE;
}

PalBool timeTest()
{
    // create and set the frequency and start time for time related calculations
    Timer timer;
    timer.frequency = palGetPerformanceFrequency();
    timer.startTime = palGetPerformanceCounter();

    // get the start time normalize by timer.startTime
    double lastTime = getTime(&timer);
    double totalTime = 0.0;
    int32_t frameCount = 0;

    // run the loop for 5 seconds
    while (totalTime < 5.0) {
        double now = getTime(&timer);
        totalTime = now - lastTime;
        frameCount++;
    }

    palLog(nullptr, "Loop finished after %f seconds and %d frames", totalTime, frameCount);
    return PAL_TRUE;
}

void registerCoreTests()
{
    registerTest(resultTest, "Result Test");
    registerTest(allocatorTest, "Allocator Test");
    registerTest(loggerTest, "Logger Test");
    registerTest(timeTest, "Time Test");
}