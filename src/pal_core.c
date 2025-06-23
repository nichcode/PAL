
#include "pal_pch.h"
#include "pal/pal_core.h"
#include "pal_platform.h"

static PalTLSID s_ErrorTLSID = 0;

struct ErrData
{
    PalError code;
};

void _PCALL palSetError(PalError error)
{
    struct ErrData* errData = PAL_NULL;
    errData = palGetTLS(s_ErrorTLSID);
    if (!errData) {
        // create TLS
        PAlAllocator allocator = palPlatformGetAllocator();
        errData = allocator.alloc(sizeof(struct ErrData));
        errData->code = error;
        palSetTLS(s_ErrorTLSID, errData, allocator.free);
    }
}

PalError _PCALL palGetError()
{
    struct ErrData* errData = palGetTLS(s_ErrorTLSID);
    if (!errData) {
        return PAL_ERROR_NONE;
    }
    return errData->code;
}