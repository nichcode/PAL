
#include "pal_pch.h"
#include "pal/pal_system.h"
#include "pal_platform.h"
#include "pal_internal.h"

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

#define SYS_LOG(...) {                     \
    if (s_System.debug) {                  \
        PAL_INFO(__VA_ARGS__);             \
    }                                      \
} 

#define SYS_ERROR(...) {                   \
    if (s_System.debug) {                  \
        PAL_ERROR(__VA_ARGS__);            \
    }                                      \
} 

bool _PCALL palInitSystem(const PAlAllocator* allocator, bool debug)
{
    s_System.sysAllocator = palPlatformGetAllocator();
    s_System.debug = PAL_TRUE;
    
    if (allocator) {
        if (!allocator->alignedAlloc || !allocator->alloc ||
            !allocator->alignedFree || !allocator->free) {
            return PAL_FALSE;
        }
        s_System.allocator = (PAlAllocator*)allocator;
        SYS_LOG("User allocator set");

    } else {
        s_System.allocator = &s_System.sysAllocator;
        SYS_LOG("Default allocator set");
    }

    if (debug) {
        SYS_LOG("Debug enabled");

    } else {
        SYS_LOG("Debug disabled");
    }

    s_System.version.major = PAL_VERSION_MAJOR;
    s_System.version.minor = PAL_VERSION_MINOR;
    s_System.version.patch = PAL_VERSION_PATCH;

    s_System.debug = PAL_TRUE;
    s_System.initialized = PAL_TRUE;

    SYS_LOG("System initialized");
    SYS_LOG(
        "Version: (%i.%i.%i)", 
        PAL_VERSION_MAJOR, 
        PAL_VERSION_MINOR, 
        PAL_VERSION_PATCH
    );
    return PAL_TRUE;
}

void _PCALL palShutdownSystem()
{
    s_System.initialized = PAL_FALSE;
    SYS_LOG("System shutdown");
}

const PalVersion* _PCALL palGetVersion()
{
    if (!s_System.initialized) { 
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        SYS_ERROR("System is not initialized");
        return PAL_NULL;
    }

    return &s_System.version;
}

const PAlAllocator* _PCALL palGetSysAllocator()
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        SYS_ERROR("System is not initialized");
        return PAL_NULL;
    }
    return &s_System.sysAllocator;
}

void* _PCALL palAllocate(Uint64 size)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        SYS_ERROR("System is not initialized");
        return NULL;
    }

    void* block = s_System.allocator->alloc(size);
    if (!block) {
        palSetError(PAL_ERROR_OUT_OF_MEMORY);
        SYS_ERROR("Failed to allocate memory");
        return PAL_NULL;
    }

    return block;
}

void* _PCALL palAlignAllocate(Uint64 size, Uint64 alignment)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        SYS_ERROR("System is not initialized");
        return NULL;
    }

    void* block = s_System.allocator->alignedAlloc(size, alignment);
    if (!block) {
        palSetError(PAL_ERROR_OUT_OF_MEMORY);
        SYS_ERROR("Failed to allocate aligned memory");
        return PAL_NULL;
    }

    return block;
}

void _PCALL palFree(void* memory)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        SYS_ERROR("System is not initialized");
        return;
    }

    if (!memory) {
        palSetError(PAL_ERROR_NULL_POINTER);
        SYS_ERROR("palFree function expects a valid memory pointer");
        return;
    }
    s_System.allocator->free(memory);
}

void _PCALL palAlignFree(void* memory)
{
    if (!s_System.initialized) {
        palSetError(PAL_ERROR_SYSTEM_NOT_INITIALIZED);
        SYS_ERROR("System is not initialized");
        return;
    }

    if (!memory) {
        palSetError(PAL_ERROR_NULL_POINTER);
        SYS_ERROR("palAlignFree function expects a valid memory pointer");
        return;
    }
    s_System.allocator->alignedFree(memory);
}
