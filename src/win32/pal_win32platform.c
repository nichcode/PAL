
#include "pal_win32platform.h"

PAlAllocator palPlatformGetAllocator()
{
    PAlAllocator allocator;
    allocator.alloc = malloc;
    allocator.free = free;
    allocator.alignedAlloc = _aligned_malloc;
    allocator.alignedFree = _aligned_free;

    return allocator;
}

PalTLSID _PCALL palCreateTLS()
{
    DWORD slot = TlsAlloc();
    if (slot == TLS_OUT_OF_INDEXES) {
        return 0;
    }

    return slot;
}

bool _PCALL palDestroyTLS(PalTLSID id)
{
    return TlsFree((DWORD)id);
}

void* _PCALL palGetTLS(PalTLSID id)
{
    return TlsGetValue((DWORD)id);
}

bool _PCALL palSetTLS(PalTLSID id, void* data, void (*destructor)(void*))
{
    return TlsSetValue((DWORD)id, data);
}