
// ==================================================
// Includes
// ==================================================

#include "pal/pal_thread.h"

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

#include <windows.h>


// ==================================================
// Typedefs, enums and structs
// ==================================================



// ==================================================
// Internal API
// ==================================================



// ==================================================
// Public API
// ==================================================

PalTlsId _PCALL palCreateTls(PaTlsDestructorFn destructor) {

    DWORD tlsid = 0;
#if _WIN32_WINNT >= 0x0600
    tlsid = FlsAlloc(destructor);
#else
    // we are on windows XP
    tlsid = TlsAlloc();
#endif // _WIN32_WINNT >= 0x0600

    if (tlsid == TLS_OUT_OF_INDEXES) {
        return 0;
    }
    return tlsid;
}

void _PCALL palDestroyTls(PalTlsId id) {

#if _WIN32_WINNT >= 0x0600
    FlsFree((DWORD)id);
#else
    // we are on windows XP
    TlsFree((DWORD)id);
#endif // _WIN32_WINNT >= 0x0600
}

void* _PCALL palGetTls(PalTlsId id) {

    return TlsGetValue((DWORD)id);
}

void _PCALL palSetTls(PalTlsId id, void* data) {

    TlsSetValue((DWORD)id, data);
}
