
#include "pal_pch.h"
#include "pal/pal_system.h"
#include "pal_platform.h"

#define PAL_VERSION_MAJOR 1
#define PAL_VERSION_MINOR 0
#define PAL_VERSION_PATCH 0

struct PalSystem
{
    PAlAllocator* allocator, sysAllocator;
    PalVersion version;
    bool initialized, debug;
};

static struct PalSystem s_System = {};

bool _PCALL palInitSystem(const PAlAllocator* allocator, bool debug)
{
    s_System.sysAllocator = palPlatformGetAllocator();
    
    if (allocator) {
        if (!allocator->alignAlloc || !allocator->alloc ||
            !allocator->alignFree || !allocator->free) {
            palSetError(PAL_ERROR_INVALID_ALLOCATOR);
            return PAL_FALSE;
        }

    } else {
        s_System.allocator = &s_System.sysAllocator;
    }

    if (debug) {
        s_System.debug = PAL_TRUE;
    }

    s_System.version.major = PAL_VERSION_MAJOR;
    s_System.version.minir = PAL_VERSION_MINOR;
    s_System.version.patch = PAL_VERSION_PATCH;

    s_System.initialized = PAL_TRUE;
    return PAL_TRUE;
}

void _PCALL palShutdownSystem()
{
    s_System.initialized = PAL_FALSE;
}

const PalVersion* _PCALL palGetVersion()
{
    if (!s_System.initialized) { 
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        return PAL_NULL;
    }

    return &s_System.version;
}

const PAlAllocator* _PCALL palGetSysAllocator()
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        return PAL_NULL;
    }
    return &s_System.sysAllocator;
}