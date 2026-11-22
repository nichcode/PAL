
#include "pal2/core/log.h"
#include "pal2/thread/thread.h"

#define THREAD_COUNT 4

typedef struct ThreadContext
{
    PalBool finished;
    PalThread* expectedHandle;
    PalThread* actualHandle;
} ThreadContext;

static void* PAL_CALL worker(void* arg)
{
    ThreadContext* context = arg;
    context->actualHandle = palGetCurrentThread();
    context->finished = PAL_TRUE;

    return nullptr;
}

static PalBool threadCreationAndJoin(void)
{
    PalThread* threads[THREAD_COUNT];
    ThreadContext contexts[THREAD_COUNT];

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;
    createInfo.stackSize = 0;
    createInfo.allocator = nullptr;

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        contexts[i].finished = PAL_FALSE;
        contexts[i].expectedHandle = nullptr;
        contexts[i].actualHandle = nullptr;

        createInfo.arg = &contexts[i];

        PalResult result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread");
            return PAL_FALSE;
        }
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        PalResult result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join threads");
            return PAL_FALSE;
        }
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        if (!contexts[i].finished) {
            palLog(nullptr, "Thread failed to complete");
            return PAL_FALSE;
        }
    }

    return PAL_TRUE;
}

static PalBool getCurrentThread(void)
{
    PalThread* threads[THREAD_COUNT];
    ThreadContext contexts[THREAD_COUNT];

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;
    createInfo.stackSize = 0;
    createInfo.allocator = nullptr;

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        contexts[i].finished = PAL_FALSE;
        contexts[i].expectedHandle = nullptr;
        contexts[i].actualHandle = nullptr;

        createInfo.arg = &contexts[i];

        PalResult result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread");
            return PAL_FALSE;
        }

        contexts[i].expectedHandle = threads[i];
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        PalResult result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join thread");
            return PAL_FALSE;
        }
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        if (!contexts[i].finished) {
            palLog(nullptr, "Thread failed to complete");
            return PAL_FALSE;
        }

        if (contexts[i].actualHandle == nullptr ||
            contexts[i].actualHandle != contexts[i].expectedHandle) {
            palLog(nullptr, "palGetCurrentThread() returned mismatched handle");
            return PAL_FALSE;
        }
    }

    return PAL_TRUE;
}

int main(void)
{
    palLog(nullptr, "Running thread test...");

    if (!threadCreationAndJoin()) {
        return -1;
    }

    if (!getCurrentThread()) {
        return -1;
    }

    return 0;
}