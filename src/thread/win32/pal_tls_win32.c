
/**
 PAL - Prime Abstraction Layer
 Copyright (C) 2025
 Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#ifndef NOMINMAX
#define NOMINMAX
#endif // NOMINMAX

// set unicode
#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "pal2/pal_thread.h"
#include <windows.h>

PalTLSId PAL_CALL palCreateTLS(PaTlsDestructorFn destructor)
{
    DWORD tlsid = FlsAlloc(destructor);
    if (tlsid == FLS_OUT_OF_INDEXES) {
        return 0;
    }
    return tlsid;
}

void PAL_CALL palDestroyTLS(PalTLSId id)
{
    FlsFree((DWORD)id);
}

void* PAL_CALL palGetTLS(PalTLSId id)
{
    return FlsGetValue((DWORD)id);
}

void PAL_CALL palSetTLS(
    PalTLSId id,
    void* data)
{
    FlsSetValue((DWORD)id, data);
}

#endif // _WIN32