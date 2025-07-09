
#include "pal_pch.h"
#include "pal/pal_core.h"

PalTLSID _PCALL palCreateTLS()
{
    DWORD slot = TlsAlloc();
    if (slot == TLS_OUT_OF_INDEXES) {
        return 0;
    }
    return slot;
}

void _PCALL palDestroyTLS(PalTLSID id)
{
    TlsFree((DWORD)id);
}

void* _PCALL palGetTLS(PalTLSID id)
{
    return TlsGetValue((DWORD)id);
}

void _PCALL palSetTLS(PalTLSID id, void* data)
{
    TlsSetValue((DWORD)id, data);
}