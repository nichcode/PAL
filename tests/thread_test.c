
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

#define THREAD_TIME 1000
#define THREAD_COUNT 4

static const char* g_ThreadNames[THREAD_COUNT] = {"Thread1", "Thread2", "Thread3", "Thread4"};

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

    PalResult  result;
    PalThread* threads[THREAD_COUNT];

    // fill the thread creation struct
    PalThreadCreateInfo createInfo = {};
    createInfo.entry               = worker; // will be the same for all threads
    createInfo.stackSize           = 0;      // same for all threads
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)(IntPtr)i + 1; // use struct to make casting easy

        // create thread
        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread: %s", palFormatResult(result));
            return false;
        }
    }

    // join threads
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        result = palJoinThread(threads[i], nullptr); // we dont need the return value
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join threads: %s", palFormatResult(result));
            return false;
        }
    }

    // check if we support setting thread names and set for each created thread
    PalThreadFeatures features = palGetThreadFeatures();
    if (features & PAL_THREAD_FEATURE_NAME) {
        for (Int32 i = 0; i < THREAD_COUNT; i++) {
            result = palSetThreadName(threads[i], g_ThreadNames[i]);
            if (result != PAL_RESULT_SUCCESS) {
                palLog(nullptr, "Failed to set thread nane: %s", palFormatResult(result));
                return false;
            }
        }
    }

    if (features & PAL_THREAD_FEATURE_NAME) {
        for (Int32 i = 0; i < THREAD_COUNT; i++) {
            char* name = palGetThreadName(threads[i]);
            palLog(nullptr, "Thread %d: (%s) finnished successfully", i + 1, name);

            palFree(nullptr, name);
            name = nullptr;
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