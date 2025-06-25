
#include "pal_pch.h"
#include "pal/pal.h"
#include "pal_internal.h"

static PalTLSID s_ErrorTLSID = 0;

typedef struct ErrorTLSData
{
    PalError error;

} ErrorTLSData;

bool _PCALL palInit(PAlAllocator* allocator, Uint32 flags)
{
    s_PAL.tmpAllocator = palGetDefaultAllocator();
    if (flags & PAL_INIT_VIDEO) {
        // TODO: video system
    }

    if (allocator) {
        if (!allocator->alignedAlloc || 
            !allocator->alignedAlloc ||
            !allocator->alloc        || 
            !allocator->free) 
        {
            palSetError(PAL_INVALID_ALLOCATOR);
            return PAL_FALSE;
        }
    }

    // set allocator
    s_PAL.allocator = &s_PAL.tmpAllocator;
    s_PAL.version.major = PAL_VMAJOR;
    s_PAL.version.minor = PAL_VMINOR;
    s_PAL.version.patch = PAL_VPATCH;    
    s_PAL.initialized = PAL_TRUE;

    return PAL_TRUE; 
}

void _PCALL palShutdown()
{
    s_PAL.initialized = PAL_TRUE;
    s_PAL.videoInitialized = PAL_FALSE;
}

PalVersion _PCALL palGetVersion()
{
    if (!s_PAL.initialized) {
        PalVersion version;
        palZeroMemory(&version, sizeof(PalVersion));
        return version;
    }
    return s_PAL.version;
}

const char* _PCALL palFormatError(PalError error)
{
    switch (error) {
        case PAL_ERROR_NONE: 
        return "The operation was successful";

        case PAL_NULL_POINTER: 
        return "The pointer is invalid.";

        case PAL_INVALID_ALLOCATOR: 
        return "The allocator function pointers are not set. You must set all the function pointers";

        case PAL_NOT_INITIALIZED: 
        return "PAL core system is not intialized";

        case PAL_VIDEO_NOT_INITIALIZED:
        return "PAL video subsystem is not intialized";

        case PAL_OUT_OF_MEMORY: 
        return "The operating system has run out of memory";
    }

    return PAL_NULL;
}

void _PCALL palSetError(PalError error)
{
    PAlAllocator allocator = palGetDefaultAllocator();
    ErrorTLSData* errorData = palGetTLS(s_ErrorTLSID);
    if (!errorData) {
        errorData = allocator.alloc(sizeof(ErrorTLSData));
        errorData->error = error;
    }
    palSetTLS(s_ErrorTLSID, errorData, allocator.free);
}

PalError _PCALL palGetError()
{
    ErrorTLSData* errorData = palGetTLS(s_ErrorTLSID);
    if (!errorData) {
        return PAL_ERROR_NONE;
    }
    return errorData->error;
}