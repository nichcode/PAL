
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

// data every thread will have its own copy of
typedef struct {
    const char* name;
    Uint32 number;
} TlsData;

// thread data
typedef struct {
    PalTLSId tlsId;
} ThreadData;

// the tls destructor
static void PAL_CALL TlsDestructor(void* userData)
{
    palLog(nullptr, "Tls destructor started");

    TlsData* tlsData = userData;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }

    palLog(nullptr, "Tls destructor finished");
}

static void* PAL_CALL worker(void* arg)
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

bool tlsTest()
{
    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "TLS Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalThread* thread = nullptr;

    // create tls
    PalTLSId tlsID = palCreateTLS(TlsDestructor);
    if (tlsID == 0) {
        palLog(nullptr, "Failed to create TLS");
        return false;
    }

    // allocate thread data
    ThreadData* threadData = palAllocate(nullptr, sizeof(ThreadData), 0);
    if (!threadData) {
        palLog(nullptr, "Failed to allocate memory");
        return false;
    }
    threadData->tlsId = tlsID;

    // create a thread
    PalThreadCreateInfo info = {};
    info.arg = threadData;
    info.entry = worker;
    info.stackSize = 0; // for default

    PalResult result = palCreateThread(&info, &thread);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Failed to create thread: %s", palFormatResult(result));
        return false;
    }

    // join thread
    palJoinThread(thread, nullptr); // wait for thread to finish

    // detach thread
    palDetachThread(thread);

    // destroy the tls
    palDestroyTLS(tlsID);

    palFree(nullptr, threadData);
    return true;
}