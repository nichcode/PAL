
#include "pal_pch.h"
#include "pal/pal_core.h"

PalTlsID _PCALL palCreateTls() {

    DWORD slot = TlsAlloc();
    if (slot == TLS_OUT_OF_INDEXES) {
        return 0;
    }
    return slot;
}

void _PCALL palDestroyTls(PalTlsID id) {

    TlsFree((DWORD)id);
}

void* _PCALL palGetTls(PalTlsID id) {

    return TlsGetValue((DWORD)id);
}

void _PCALL palSetTls(PalTlsID id, void* data) {

    TlsSetValue((DWORD)id, data);
}
