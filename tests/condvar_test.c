
#include "pal/pal_thread.h"
#include "tests.h"

#define THREAD_COUNT 4

// we dont want to allocate this and pass it to every thread
static PalMutex* g_Mutex;
static PalCondVar* g_Condition;

typedef struct {
    bool ready;
    Uint32 id;
} ThreadData;

static void* PAL_CALL worker(void* arg)
{
    ThreadData* data = (ThreadData*)arg;
    palLog(nullptr, "Thread %d waiting...", data->id);

    palLockMutex(g_Mutex);
    while (!data->ready) {
        palWaitCondVar(g_Condition, g_Mutex);
    }

    palUnlockMutex(g_Mutex);
    palLog(nullptr, "Thread %d signaled", data->id);
    return nullptr;
}

bool condvarTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Condvar Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult result;
    PalThread* threads[THREAD_COUNT];

    ThreadData* data = nullptr;
    data = palAllocate(nullptr, sizeof(ThreadData) * THREAD_COUNT, 0);
    if (!data) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    // create mutex
    result = palCreateMutex(nullptr, &g_Mutex);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create mutex: %s", error);
        return false;
    }

    // create condition
    result = palCreateCondVar(nullptr, &g_Condition);
    if (result != PAL_RESULT_SUCCESS) {
        const char* error = palFormatResult(result);
        palLog(nullptr, "Failed to create cond var: %s", error);
        return false;
    }

    // create threads
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker; // will be the same for all threads
    createInfo.stackSize = 0;  // same for all threads
    createInfo.allocator = nullptr;// default
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        ThreadData* threadData = &data[i];
        threadData->id = i + 1;
        threadData->ready = false;
        createInfo.arg = (void*)threadData;

        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            const char* error = palFormatResult(result);
            palLog(nullptr, "Failed to create thread: %s", error);
            return false;
        }
    }

    // wait for a while
    palSleep(1000);

    // signal thread 1
    palLockMutex(g_Mutex);
    data[0].ready = true;
    palSignalCondVar(g_Condition);
    palUnlockMutex(g_Mutex);

    // wait for thread one to finish
    palJoinThread(threads[0], nullptr);
    palLog(nullptr, "Thread %d finished successfully", data[0].id);

    // broadcast to all remaining threads
    palLockMutex(g_Mutex);
    for (Int32 i = 1; i < THREAD_COUNT; i++) {
        data[i].ready = true;
    }

    palBroadcastCondVar(g_Condition);
    palUnlockMutex(g_Mutex);

    // wait for the remaining threads
    for (Int32 i = 1; i < THREAD_COUNT; i++) {
        palJoinThread(threads[i], nullptr);
        palLog(nullptr, "Thread %d finished successfully", data[i].id);
    }

    palFree(nullptr, data);
    return true;
}