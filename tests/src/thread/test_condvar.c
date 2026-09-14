
#include "pal2/pal_thread.h"
#include <stdbool.h>

#define THREAD_COUNT 4

typedef struct SharedData
{
    PalMutex* mutex;
    PalCondVar* condVar;
} SharedData;

typedef struct ThreadData
{
    bool ready;
    bool finished;
    SharedData* sharedData;
} ThreadData;

static void* PAL_CALL worker(void* arg)
{
    ThreadData* data = (ThreadData*)arg;
    SharedData* sharedData = data->sharedData;

    palLockMutex(sharedData->mutex);
    while (!data->ready) {
        palWaitCondVar(sharedData->condVar, sharedData->mutex);
    }

    palUnlockMutex(sharedData->mutex);

    data->finished = true;

    return nullptr;
}

int main(void)
{
    palLog(nullptr, "Running condvar test...");

    PalThread* threads[THREAD_COUNT];
    ThreadData* threadData = nullptr;
    SharedData* sharedData = nullptr;

    threadData = palAllocate(nullptr, sizeof(ThreadData) * THREAD_COUNT, 0);
    sharedData = palAllocate(nullptr, sizeof(SharedData), 0);

    if (!threadData || !sharedData) {
        palLog(nullptr, "Failed to allocate memory");
        return -1;
    }

    PalResult result = palCreateMutex(nullptr, &sharedData->mutex);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create mutex");
        return -1;
    }

    result = palCreateCondVar(nullptr, &sharedData->condVar);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create condition variable");
        return -1;
    }

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        threadData[i].finished = false;
        threadData[i].ready = false;
        threadData[i].sharedData = sharedData;

        createInfo.arg = (void*)&threadData[i];

        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread");
            return -1;
        }

        // since thread priority is not supported on all platforms
        // the first thread might not be the one to lock the mutex
        // so we wait a while for the first thread to lock the mutex
        if (i == 0) {
            palSleep(200);
        }
    }

    palSleep(1000);

    palLockMutex(sharedData->mutex);
    threadData[0].ready = PAL_TRUE;
    palSignalCondVar(sharedData->condVar);
    palUnlockMutex(sharedData->mutex);

    result = palJoinThread(threads[0], nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to join thread");
        return -1;
    }

    palLockMutex(sharedData->mutex);
    for (int32_t i = 1; i < THREAD_COUNT; i++) {
        threadData[i].ready = PAL_TRUE;
    }

    palBroadcastCondVar(sharedData->condVar);
    palUnlockMutex(sharedData->mutex);

    for (int32_t i = 1; i < THREAD_COUNT; i++) {
        result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join thread");
            return -1;
        }
    }

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        if (!threadData[i].finished) {
            palLog(nullptr, "Thread was not signaled");
            return -1;
        }
    }

    palDestroyMutex(sharedData->mutex);
    palDestroyCondVar(sharedData->condVar);

    palFree(nullptr, sharedData);
    palFree(nullptr, threadData);

    return 0;
}