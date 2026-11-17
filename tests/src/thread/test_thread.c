
#include "pal2/core/log.h"
#include "pal2/thread/thread.h"
#include <stdbool.h>

#define THREAD_COUNT 4

typedef struct ThreadContext
{
    bool finished;
    PalThread* expectedHandle;
    PalThread* actualHandle;
} ThreadContext;

static void* PAL_CALL worker(void* arg)
{
    ThreadContext* context = arg;
    context->actualHandle = palGetCurrentThread();
    context->finished = true;

    return nullptr;
}

static bool threadCreationAndJoin(void)
{
    PalThread* threads[THREAD_COUNT];
    ThreadContext contexts[THREAD_COUNT];

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;
    createInfo.stackSize = 0;
    createInfo.allocator = nullptr;

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        contexts[i].finished = false;
        contexts[i].expectedHandle = nullptr;
        contexts[i].actualHandle = nullptr;

        createInfo.arg = &contexts[i];

        PalResult result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread");
            return false;
        }
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        PalResult result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join threads");
            return false;
        }
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        if (!contexts[i].finished) {
            palLog(nullptr, "Thread failed to complete");
            return false;
        }
    }

    return true;
}

static bool getCurrentThread(void)
{
    PalThread* threads[THREAD_COUNT];
    ThreadContext contexts[THREAD_COUNT];

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;
    createInfo.stackSize = 0;
    createInfo.allocator = nullptr;

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        contexts[i].finished = false;
        contexts[i].expectedHandle = nullptr;
        contexts[i].actualHandle = nullptr;

        createInfo.arg = &contexts[i];

        PalResult result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread");
            return false;
        }

        contexts[i].expectedHandle = threads[i];
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        PalResult result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join thread");
            return false;
        }
    }

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        if (!contexts[i].finished) {
            palLog(nullptr, "Thread failed to complete");
            return false;
        }

        if (contexts[i].actualHandle == nullptr ||
            contexts[i].actualHandle != contexts[i].expectedHandle) {
            palLog(nullptr, "palGetCurrentThread() returned mismatched handle");
            return false;
        }
    }

    return true;
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