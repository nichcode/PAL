
#include "pal/pal_thread.h"
#include "tests.h"

#define THREAD_COUNT 4

// we dont want to allocate this and pass it to every thread
static PalMutex* g_Mutex;
static PalCondVar* g_Condition;

typedef struct {
    PalBool ready;
    uint32_t id;
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

PalBool condvarTest()
{
    PalResult result;
    PalThread* threads[THREAD_COUNT];

    ThreadData* data = nullptr;
    data = palAllocate(nullptr, sizeof(ThreadData) * THREAD_COUNT, 0);
    if (!data) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    // create mutex
    result = palCreateMutex(nullptr, &g_Mutex);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create mutex");
        return PAL_FALSE;
    }

    // create condition
    result = palCreateCondVar(nullptr, &g_Condition);
    if (result != PAL_RESULT_SUCCESS) {
        logResult(result, "Failed to create condition variable");
        return PAL_FALSE;
    }

    // create threads
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;      // will be the same for all threads
    createInfo.stackSize = 0;       // same for all threads
    createInfo.allocator = nullptr; // default
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        ThreadData* threadData = &data[i];
        threadData->id = i + 1;
        threadData->ready = PAL_FALSE;
        createInfo.arg = (void*)threadData;

        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            logResult(result, "Failed to create thread");
            return PAL_FALSE;
        }

        // since thread priority is not supported on all platforms
        // the first thread might not be the one to lock the mutex
        // so we wait a while for the first thread to lock the mutex
        if (i == 0) {
            palSleep(200);
        }
    }

    // wait for a while
    palSleep(1000);

    // signal thread 1
    palLockMutex(g_Mutex);
    data[0].ready = PAL_TRUE;
    palSignalCondVar(g_Condition);
    palUnlockMutex(g_Mutex);

    // wait for thread one to finish
    palJoinThread(threads[0], nullptr);
    palLog(nullptr, "Thread %d finished successfully", data[0].id);

    // broadcast to all remaining threads
    palLockMutex(g_Mutex);
    for (int32_t i = 1; i < THREAD_COUNT; i++) {
        data[i].ready = PAL_TRUE;
    }

    palBroadcastCondVar(g_Condition);
    palUnlockMutex(g_Mutex);

    // wait for the remaining threads
    // joint threads does not need to be detached
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        palJoinThread(threads[i], nullptr);
        palLog(nullptr, "Thread %d finished successfully", data[i].id);
    }

    palFree(nullptr, data);
    return PAL_TRUE;
}
