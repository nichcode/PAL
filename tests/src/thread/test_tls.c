#include "pal2/thread/thread.h"
#include "pal2/thread/tls.h"
#include "pal2/core/log.h"

typedef struct ThreadData
{
    PalBool tlsDestructorCalled;
    PalTLSId tlsId;
} ThreadData;

typedef struct TLSData
{
    ThreadData* threadData;
} TLSData;

static void PAL_CALL tlsDestructor(void* userData)
{
    if (!userData) {
        return;
    }

    TLSData* data = userData;
    data->threadData->tlsDestructorCalled = PAL_TRUE;
    palFree(nullptr, data);
}

static void* PAL_CALL worker(void* arg)
{
    ThreadData* threadData = arg;

    TLSData* tlsData = palAllocate(nullptr, sizeof(TLSData), 0);
    if (!tlsData) {
        palLog(nullptr, "Failed to allocate memory");
        return nullptr;
    }

    tlsData->threadData = threadData;
    palSetTLS(threadData->tlsId, tlsData);

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
        palDestroyTLS(tlsID);
        palLog(nullptr, "Failed to allocate memory");
        return -1;
    }

    threadData->tlsId = tlsID;
    threadData->tlsDestructorCalled = PAL_FALSE;

    PalThreadCreateInfo createInfo = {0};
    createInfo.arg = threadData;
    createInfo.entry = worker;

    PalResult result = palCreateThread(&createInfo, &thread);
    if (result != PAL_RESULT_SUCCESS) {
        palFree(nullptr, threadData);
        palDestroyTLS(tlsID);
        palLog(nullptr, "Failed to create thread");
        return -1;
    }

    result = palJoinThread(thread, nullptr);
    if (result != PAL_RESULT_SUCCESS) {
        palFree(nullptr, threadData);
        palDestroyTLS(tlsID);
        palLog(nullptr, "Failed to join thread");
        return -1;
    }

    palDestroyTLS(tlsID);

    if (!threadData->tlsDestructorCalled) {
        palFree(nullptr, threadData);
        palLog(nullptr, "TLS destructor was not called");
        return -1;
    }

    palFree(nullptr, threadData);
    return 0;
}