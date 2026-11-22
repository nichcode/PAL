#include "pal2/core/log.h"
#include "pal2/thread/thread.h"

#define THREAD_COUNT 4

typedef struct ThreadContext
{
    PalBool finished;
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
    PalThread* threads[THREAD_COUNT] = {0};
    ThreadContext contexts[THREAD_COUNT] = {0};

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
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
    PalThread* threads[THREAD_COUNT] = {0};
    ThreadContext contexts[THREAD_COUNT] = {0};

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
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
            contexts[i].actualHandle != threads[i]) {
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