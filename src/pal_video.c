
#include "pal_pch.h"
#include "pal/pal_video.h"
#include "pal_internal.h"

bool _PCALL palInitVideoSystem(const PalAllocator* allocator)
{
    // check if we have not initialized PAL core system
    if (!s_PAL.initialized) {
        if (!palInit(allocator, PAL_INIT_NONE)) {
            palSetError(PAL_NOT_INITIALIZED);
            return PAL_FALSE;
        }

        s_PALVideo.allocator = s_PAL.allocator;

    } else {
        // core system initialized already
        if (allocator) {
            if (!allocator->alignedAlloc || 
                !allocator->alignedAlloc ||
                !allocator->alloc        || 
                !allocator->free) 
            {
                palSetError(PAL_INVALID_ALLOCATOR);
                return PAL_FALSE;
            }
            s_PALVideo.allocator = s_PAL.allocator;
        }
    }

    if (!_palPlatformVideoInit()) {
        return PAL_FALSE;
    }

    s_PALVideo.initialized = PAL_TRUE;
    return PAL_TRUE;
}

void _PCALL palShutdownVideoSystem()
{
    _palPlatformVideoShutdown();
    s_PALVideo.initialized = PAL_FALSE;
    
    // shutdown core system 
    if (s_PAL.initialized) {
        palShutdown();
    }  
}

bool _PCALL palIsVideoSystemInitialized()
{
    return s_PALVideo.initialized;
}
