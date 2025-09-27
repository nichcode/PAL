
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

#define THREAD_TIME 1000
#define THREAD_COUNT 4

// clang-format off
static const char* g_ThreadNames[THREAD_COUNT] = {
    "Thread1", 
    "Thread2", 
    "Thread3", 
    "Thread4"};
// clang-format on

static void* PAL_CALL worker(void* arg)
{
    // palLog is thread safe so there should'nt be any race conditions
    Int32 id = (Int32)(IntPtr)arg;
    palLog(nullptr, "Thread %d: started", id);

    palSleep(THREAD_TIME * id);
    palLog(nullptr, "Thread %d: finished", id);
    return nullptr;
}

bool threadTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Thread Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalThread* threads[THREAD_COUNT];

    // fill the thread creation struct
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker; // will be the same for all threads
    createInfo.stackSize = 0;  // same for all threads
    createInfo.allocator = nullptr;// default
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)((IntPtr)i + 1);

        // create thread
        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create thread: %s", error);
            return false;
        }
    }

    // join threads
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        // we dont need the return value
        result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to join threads: %s", error);
            return false;
        }
    }

    // check if we support setting thread names and set for each created thread
    PalThreadFeatures features = palGetThreadFeatures();
    if (features & PAL_THREAD_FEATURE_NAME) {
        for (Int32 i = 0; i < THREAD_COUNT; i++) {
            result = palSetThreadName(threads[i], g_ThreadNames[i]);
            if (result != PAL_RESULT_SUCCESS) {
                const char* error = palFormatResult(result);
                palLog(nullptr, "Failed to set thread name %s", error);
                return false;
            }
        }
    }

    if (features & PAL_THREAD_FEATURE_NAME) {
        char buffer[64] = {0};
        for (Int32 i = 0; i < THREAD_COUNT; i++) {
            palGetThreadName(threads[i], 64, nullptr, buffer);
            palLog(
                nullptr,
                "Thread %d: (%s) finnished successfully",
                i + 1,
                buffer);
        }
    }

    palLog(nullptr, "All threads finished successfully");

    // detach threads
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        palDetachThread(threads[i]);
        palLog(nullptr, "Thread %d: detached", i + 1);
    }

    return true;
}