
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
        if (!allocator->alignedAlloc || !allocator->alloc ||
            !allocator->alignedFree || !allocator->free) {
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
    s_System.version.minor = PAL_VERSION_MINOR;
    s_System.version.patch = PAL_VERSION_PATCH;

    palSetError(PAL_ERROR_OUT_OF_MEMORY);
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

void* _PCALL palAllocate(Uint64 size)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        return NULL;
    }
    return s_System.allocator->alloc(size);
}

void* _PCALL palAlignAllocate(Uint64 size, Uint64 alignment)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        return NULL;
    }
    return s_System.allocator->alignedAlloc(size, alignment);
}

void _PCALL palFree(void* memory)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        return;
    }

    if (!memory) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return;
    }
    s_System.allocator->free(memory);
}

void _PCALL palAlignFree(void* memory)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        return;
    }

    if (!memory) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return;
    }
    s_System.allocator->alignedFree(memory);
}
