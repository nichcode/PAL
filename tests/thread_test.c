
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
    info.allocator = nullptr;
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

    palDetachThread(thread1);
    palDetachThread(thread2);
    palDetachThread(thread3);

    return true;
}