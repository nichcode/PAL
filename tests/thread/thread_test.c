
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

#define THREAD_TIME 1000
#define THREAD_COUNT 4

static void* PAL_CALL worker(void* arg)
{
    // palLog is thread safe so there should'nt be any race conditions
    int32_t id = (int32_t)(intptr_t)arg;
    palLog(nullptr, "Thread %d: started", id);

    palSleep(THREAD_TIME * id);
    palLog(nullptr, "Thread %d: finished", id);
    return nullptr;
}

bool threadTest()
{
    PalResult result;
    PalThread* threads[THREAD_COUNT];

    // fill the thread creation struct
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;      // will be the same for all threads
    createInfo.stackSize = 0;       // same for all threads
    createInfo.allocator = nullptr; // default
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)((intptr_t)i + 1);

        // create thread
        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create thread: %s", error);
            return false;
        }
    }

    // join threads
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        // we dont need the return value
        // joint threads does not need to be detached
        result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to join threads: %s", error);
            return false;
        }
    }

    palLog(nullptr, "All threads finished successfully");
    return true;
}
