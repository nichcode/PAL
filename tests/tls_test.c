
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

typedef struct {
    const char* name;
    Uint32 id;
} TlsData;

void TlsDestructor(void* data) {

    TlsData* tlsData = data;
    if (tlsData) {
        palFree(nullptr, tlsData);
    }
}

void* onThread(void* arg) {

    // create tls
    PalTlsId tlsID = (PalTlsId)(UintPtr)arg;

    // allocate and buffer and store it with the tls
    TlsData *data = palAllocate(nullptr, sizeof(TlsData), 0); // use default alignment(16)
    if (!data) {
        return nullptr;
    }

    data->id = 10;
    data->name = "TLS Data";

    palSetTls(tlsID, data); // saves the data at the tlsID slot
    palLog(nullptr, "TLS id: %d, Data string: %s", tlsID, data->name);

    return nullptr;
}

bool tlsTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "TLS Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalThread* thread = nullptr;

    // create tls
    PalTlsId tlsID = palCreateTls(TlsDestructor);
    if (tlsID == 0) {
        palLog(nullptr, "Failed to create TLS");
        return false;
    }

    PalThreadCreateInfo info = {};
    info.arg = (void*)(UintPtr)tlsID; // use a struct for easy casting
    info.entry = onThread;
    info.stackSize = 0; // for default
    PalResult result = palCreateThread(&info, &thread);
    if (result != PAL_RESULT_SUCCESS) {
        palLog(nullptr, "Error: %s", palFormatResult(result));
        return false;
    }

    palJoinThread(thread, nullptr); // wait for thread to finish
    palDetachThread(thread);

    // destroy the tls.
    palDestroyTls(tlsID); 
    return true;
}