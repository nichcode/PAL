
#include "pal/pal_core.h"
#include "pal/pal_thread.h"

void TlsDestructor(void* data) {

    palLog(nullptr, "TLS destructor called");
}

bool tlsTest() {

    palLog(nullptr, "");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "TLS Test");
    palLog(nullptr, "===========================================");
    palLog(nullptr, "");

    PalTlsId tlsID = palCreateTls(TlsDestructor);
    if (tlsID == 0) {
        palLog(nullptr, "Failed to create TLS");
        return false;
    }

    // allocate and buffer and store it with the tls


    palDestroyTls(tlsID); // on Windows Vista and above, this should call the TlsDestructor
    return true;
}