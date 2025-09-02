
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

void* workerThread(void* arg) {
    // palLog is thread safe so there should'nt be any race conditions
    palLog(nullptr, "arg=%s: working", (char*)arg);
    return nullptr;
}

bool threadTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "Thread Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalThread* thread1, *thread2, *thread3 = nullptr;

    PalThreadCreateInfo info = {};
    info.arg = "Thread 1";
    info.entry = workerThread;
    info.stackSize = 0; // for default
    PalResult result = palCreateThread(&info, &thread1);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Error: %s", palFormatResult(result));
        return false;
    }

    // reuse the dame info with small changes
    info.arg = "Thread 2";   
    result = palCreateThread(&info, &thread2);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Error: %s", palFormatResult(result));
        return false;
    } 

    info.arg = "Thread 3";
    result = palCreateThread(&info, &thread3);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Error: %s", palFormatResult(result));
        return false;
    }

    palSleep(1000); // to allow other threads to run before exiting

    // set and get the name of the thread if supported
    PalThreadFeatures features = palGetThreadFeatures();

    if (features & PAL_THREAD_FEATURE_NAME) {
        result = palSetThreadName(thread1, "Thread1");
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Error: %s", palFormatResult(result));
            return false;
        }

        char* threadName1 = palGetThreadName(thread1);
        palLog(nullptr, "Thread 1 Name: %s", threadName1);
        palFree(nullptr, threadName1);
    }

    // we set thread priority for thread 2 to always be high
    // platform (OS) might block access
    if (features & PAL_THREAD_FEATURE_PRIORITY) {
        result = palSetThreadPriority(thread2,PAL_THREAD_PRIORITY_HIGH);
        if (result != PAL_RESULT_SUCCESS) {
            palLog(nullptr, "Error: %s", palFormatResult(result));
            return false;
        }

        PalThreadPriority priority = palGetThreadPriority(thread2);
    }

    palDetachThread(thread1);
    palDetachThread(thread2);
    palDetachThread(thread3);

    return true;
}