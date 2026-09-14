
#include "pal2/pal_thread.h"
#include <stdbool.h>

typedef struct ThreadData
{
    bool tlsDestructorCalled;
    PalTLSId tlsId;
} ThreadData;

typedef struct TLSData
{
    ThreadData* threadData;
} TLSData;

static void PAL_CALL tlsDestructor(void* userData)
{
    TLSData* data = userData;
    ThreadData* threadData = (ThreadData*)data->threadData;
    threadData->tlsDestructorCalled = true;

    if (data) {
        palFree(nullptr, data);
    }
}

static void* PAL_CALL worker(void* arg)
{
    ThreadData* threadData = (ThreadData*)arg;

    TLSData* tlsData = palAllocate(nullptr, sizeof(TLSData), 0);
    if (!tlsData) {
        palLog(nullptr, "Failed to allocate memory");
        return nullptr;
    }

    palSetTLS(threadData->tlsId, tlsData);

    tlsData->threadData = threadData;

    return nullptr;
}

int main(void)
{
    palLog(nullptr, "Running tls test...");

    PalThread* thread = nullptr;

    PalTLSId tlsID = palCreateTLS(tlsDestructor);
    if (tlsID == 0) {
        palLog(nullptr, "Failed to create TLS");
        return -1;
    }

    ThreadData* threadData = palAllocate(nullptr, sizeof(ThreadData), 0);
    if (!threadData) {
        palLog(nullptr, "Failed to allocate memory");
        return -1;
    }

    threadData->tlsId = tlsID;
    threadData->tlsDestructorCalled = false;

    PalThreadCreateInfo createInfo = {0};
    createInfo.arg = threadData;
    createInfo.entry = worker;
    createInfo.stackSize = 0;
    createInfo.allocator = nullptr;

    PalResult result = palCreateThread(&createInfo, &thread);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create thread");
        return -1;
    }

    result = palJoinThread(thread, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to join thread");
        return -1;
    }

    palDestroyTLS(tlsID);

    if (!threadData->tlsDestructorCalled) {
        palLog(nullptr, "TLS destructor was not called");
        return -1;
    }

    palFree(nullptr, threadData);

    return 0;
}