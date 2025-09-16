
#include "pal/pal_thread.h"
#include "tests.h"

#define MAX_COUNTER 10000
#define THREAD_COUNT 2

typedef struct {
    PalMutex* mutex;
    Int32     counter;
} SharedData;

static void* PAL_CALL worker(void* arg)
{

    SharedData* data = (SharedData*)arg;

    // this is only needed when two or more threads are writing to the same variable
    for (Int32 i = 0; i < MAX_COUNTER; i++) {
        palLockMutex(data->mutex);
        data->counter++; // a shared variable. we need lock and unlocks
        palLog(nullptr, "Counter: %d", data->counter);
        palUnlockMutex(data->mutex);
    }

    return nullptr;
}

bool mutexTest()
{

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Mutex Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalResult  result;
    PalThread* threads[THREAD_COUNT];
    PalMutex*  mutex = nullptr;

    SharedData* data = palAllocate(nullptr, sizeof(SharedData), 0);
    if (!data) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }

    // create mutex
    result = palCreateMutex(&mutex);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create mutex: %s", palFormatResult(result));
        return false;
    }

    data->counter = 0;
    data->mutex   = mutex;

    // create threads
    PalThreadCreateInfo createInfo = {};
    createInfo.entry               = worker; // will be the same for all threads
    createInfo.stackSize           = 0;      // same for all threads
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)data;

        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread: %s", palFormatResult(result));
            return false;
        }
    }

    // join the threads to main thread
    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        palJoinThread(threads[i], nullptr);
    }

    for (Int32 i = 0; i < THREAD_COUNT; i++) {
        palDetachThread(threads[i]);
    }

    palDestroyMutex(data->mutex);

    palLog(nullptr, "Expected Counter: %d", MAX_COUNTER * THREAD_COUNT);
    palLog(nullptr, "Final Counter: %d", data->counter);
    palFree(nullptr, data);

    return true;
}