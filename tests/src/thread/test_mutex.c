
#include "pal2/pal_thread.h"
#include <stdbool.h>

#define MAX_COUNTER 10000
#define THREAD_COUNT 2

typedef struct SharedData
{
    PalMutex* mutex;
    int32_t counter;
} SharedData;

static void* PAL_CALL worker(void* arg)
{
    SharedData* data = (SharedData*)arg;

    for (int32_t i = 0; i < MAX_COUNTER; i++) {
        palLockMutex(data->mutex);
        data->counter++;
        palUnlockMutex(data->mutex);
    }

    return nullptr;
}

int main(void)
{
    palLog(nullptr, "Running mutex test...");

    PalThread* threads[THREAD_COUNT];
    PalMutex* mutex = nullptr;

    SharedData* data = palAllocate(nullptr, sizeof(SharedData), 0);
    if (!data) {
        palLog(nullptr, "Failed to allocate memory");
        return -1;
    }

    PalResult result = palCreateMutex(nullptr, &mutex);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create mutex");
        return -1;
    }

    data->counter = 0;
    data->mutex = mutex;

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)data;

        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread");
            return -1;
        }
    }

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join thread");
            return -1;
        }
    }

    palDestroyMutex(data->mutex);

    if (data->counter != MAX_COUNTER * THREAD_COUNT) {
        palLog(nullptr, "Final counter was not the expected");
        return -1;
    }

    palFree(nullptr, data);

    return 0;
}