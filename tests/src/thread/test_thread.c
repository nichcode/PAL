
#include "pal2/pal_thread.h"
#include <stdbool.h>

#define THREAD_COUNT 4

typedef struct ThreadContext
{
    bool finished;
} ThreadContext;

static void* PAL_CALL worker(void* arg)
{
    ThreadContext* context = arg;
    context->finished = true;

    return nullptr;
}

int main(void)
{
    palLog(nullptr, "Running thread test...");

    PalThread* threads[THREAD_COUNT];
    ThreadContext contexts[THREAD_COUNT];

    PalThreadCreateInfo createInfo = {0};
    createInfo.entry = worker;
    createInfo.stackSize = 0;
    createInfo.allocator = nullptr;

    for (uint32_t i = 0; i < THREAD_COUNT; i++) {
        contexts[i].finished = false;

        createInfo.arg = &contexts[i];

        PalResult result = palCreateThread(&createInfo, &threads[i]);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to create thread");
            return -1;
        }
    }

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        PalResult result = palJoinThread(threads[i], nullptr);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Failed to join threads");
            return -1;
        }
    }

    for (int32_t i = 0; i < THREAD_COUNT; i++) {
        if (!contexts[i].finished) {
            palLog(nullptr, "Thread failed to complete");
            return -1;
        }
    }

    return 0;
}