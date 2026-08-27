
#include "tests.h"
#include "pal2/pal_thread.h"

#define THREAD_COUNT 4
#define MAX_COUNTER 10000
#define THREAD_TIME 1000

#if PAL_HAS_THREAD_MODULE

typedef struct {
    PalTLSId tlsId;
} ThreadData;

typedef struct {
    const char* name;
    uint32_t number;
} TlsData;

typedef struct {
    PalMutex* mutex;
    int32_t counter;
} MutexData;

typedef struct {
    PalMutex* mutex;
    PalCondVar* condition;
} SharedData;

typedef struct {
    PalBool ready;
    uint32_t id;
    SharedData* shared;
} CondVarData;

static void PAL_CALL TlsDestructor(void* userData)
{
    palLog(nullptr, "Tls destructor started");

    TlsData* tlsData = userData;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }

    palLog(nullptr, "Tls destructor finished");
}

static void* PAL_CALL threadWorker(void* arg)
{
    // palLog is thread safe so there should'nt be any race conditions
    int32_t id = (int32_t)(intptr_t)arg;
    palLog(nullptr, "Thread %d: started", id);

    palSleep(THREAD_TIME * id);
    palLog(nullptr, "Thread %d: finished", id);
    return nullptr;
}

static void* PAL_CALL tlsWorker(void* arg)
{
    ThreadData* threadData = (ThreadData*)arg;
    palLog(nullptr, "Thread 0: started");

    // allocate and buffer and store it with the tls
    TlsData* data = palAllocate(nullptr, sizeof(TlsData), 0);
    if (!data) {
        palLog(nullptr, "Failed to allocate memory");
        return nullptr;
    }

    data->number = 10;
    data->name = "TLS Data";

    // for the tls destructor to be called, the tls must have a non null value
    palSetTLS(threadData->tlsId, data);
    palLog(nullptr, "TLS %d: Data string: %s", threadData->tlsId, data->name);

    palLog(nullptr, "Thread 0: finished");
    return nullptr;
}

static void* PAL_CALL mutexWorker(void* arg)
{
    MutexData* data = (MutexData*)arg;

    // this is only needed when two or more threads are writing to the same variable
    for (int32_t i = 0; i < MAX_COUNTER; i++) {
        palLockMutex(data->mutex);
        data->counter++; // a shared variable. we need lock and unlocks
        palUnlockMutex(data->mutex);
    }
    return nullptr;
}

static void* PAL_CALL condvarWorker(void* arg)
{
    CondVarData* data = (CondVarData*)arg;
    palLog(nullptr, "Thread %d waiting...", data->id);

    palLockMutex(data->shared->mutex);
    while (!data->ready) {
        palWaitCondVar(data->shared->condition, data->shared->mutex);
    }

    palUnlockMutex(data->shared->mutex);
    palLog(nullptr, "Thread %d signaled", data->id);
    return nullptr;
}

PalBool threadTest()
{
    PalResult result;
    PalThread* threads[THREAD_COUNT];

    // fill the thread creation struct
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = threadWorker;

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)((intptr_t)i + 1);

        // create thread
        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            return PAL_FALSE;
        }
    }

    // join threads
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        // we dont need the return value joint threads does not need to be detached
        result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            return PAL_FALSE;
        }
    }

    palLog(nullptr, "All threads finished successfully");
    return PAL_TRUE;
}

PalBool tlsTest()
{
    PalThread* thread = nullptr;

    // create tls
    PalTLSId tlsID = palCreateTLS(TlsDestructor);
    if (tlsID == 0) {
        palLog(nullptr, "Failed to create TLS");
        return PAL_FALSE;
    }

    // allocate thread data
    ThreadData* threadData = palAllocate(nullptr, sizeof(ThreadData), 0);
    if (!threadData) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }
    threadData->tlsId = tlsID;

    // create a thread
    PalThreadCreateInfo info = {0};
    info.arg = threadData;
    info.entry = tlsWorker;

    PalResult result = palCreateThread(&info, &thread);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    // join thread
    // joint threads does not need to be detached
    palJoinThread(thread, nullptr); // wait for thread to finish

    // destroy the tls
    palDestroyTLS(tlsID);

    palFree(nullptr, threadData);
    return PAL_TRUE;
}

PalBool mutexTest()
{
    PalResult result;
    PalThread* threads[THREAD_COUNT];
    PalMutex* mutex = nullptr;

    MutexData* data = palAllocate(nullptr, sizeof(MutexData), 0);
    if (!data) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    // create mutex
    result = palCreateMutex(nullptr, &mutex);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    data->counter = 0;
    data->mutex = mutex;

    // create threads
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = mutexWorker;

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        createInfo.arg = (void*)data;

        // create thread
        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            return PAL_FALSE;
        }
    }

    // join the threads to main thread joint threads does not need to be detached
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        palJoinThread(threads[i], nullptr);
    }

    palDestroyMutex(data->mutex);
    palLog(nullptr, "Expected Counter: %d", MAX_COUNTER * THREAD_COUNT);
    palLog(nullptr, "Final Counter: %d", data->counter);
    palFree(nullptr, data);

    return PAL_TRUE;
}

PalBool condvarTest()
{
    PalResult result;
    PalThread* threads[THREAD_COUNT];

    CondVarData* data = nullptr;
    SharedData* sharedData = nullptr;
    data = palAllocate(nullptr, sizeof(CondVarData) * THREAD_COUNT, 0);
    sharedData = palAllocate(nullptr, sizeof(SharedData), 0);
    if (!data || !sharedData) {
        palLog(nullptr, "Failed to allocate memory");
        return PAL_FALSE;
    }

    // create mutex
    result = palCreateMutex(nullptr, &sharedData->mutex);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    // create condition
    result = palCreateCondVar(nullptr, &sharedData->condition);
    if (result != PAL_RESULT_SUCCESS) {
        return PAL_FALSE;
    }

    // create threads
    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = condvarWorker;

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        CondVarData* threadData = &data[i];
        threadData->id = i + 1;
        threadData->ready = PAL_FALSE;
        threadData->shared = sharedData;
        createInfo.arg = (void*)threadData;

        result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
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
    palLockMutex(sharedData->mutex);
    data[0].ready = PAL_TRUE;
    palSignalCondVar(sharedData->condition);
    palUnlockMutex(sharedData->mutex);

    // wait for thread one to finish
    palJoinThread(threads[0], nullptr);
    palLog(nullptr, "Thread %d finished successfully", data[0].id);

    // broadcast to all remaining threads
    palLockMutex(sharedData->mutex);
    for (int32_t i = 1; i < THREAD_COUNT; i++) {
        data[i].ready = PAL_TRUE;
    }

    palBroadcastCondVar(sharedData->condition);
    palUnlockMutex(sharedData->mutex);

    // wait for the remaining threads joint threads does not need to be detached
    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        palJoinThread(threads[i], nullptr);
        palLog(nullptr, "Thread %d finished successfully", data[i].id);
    }

    palFree(nullptr, data);
    return PAL_TRUE;
}

#endif // PAL_HAS_THREAD_MODULE

void registerThreadTests()
{
#if PAL_HAS_THREAD_MODULE
    registerTest(threadTest, "Thread Test");
    registerTest(tlsTest, "TLS Test");
    registerTest(mutexTest, "Mutex Test");
    registerTest(condvarTest, "Condvar Test");
#endif // PAL_HAS_THREAD_MODULE
}