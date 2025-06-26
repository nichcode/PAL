
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

        s_Video.allocator = s_PAL.allocator;

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
        }

        s_Video.allocator = s_PAL.allocator;
    }

    if (!_palPlatformVideoInit()) {
        return PAL_FALSE;
    }

    s_Video.initialized = PAL_TRUE;
    return PAL_TRUE;
}

void _PCALL palShutdownVideoSystem()
{
    _palPlatformVideoShutdown();
    s_Video.initialized = PAL_FALSE;
    
    // shutdown core system 
    if (s_PAL.initialized) {
        palShutdown();
    }  
}

bool _PCALL palIsVideoSystemInitialized()
{
    return s_Video.initialized;
}

int _PCALL palGetDisplayCount()
{
    return s_Video.displayCount;
}

const PalDisplay* _PCALL palGetPrimaryDisplay()
{
    if (s_Video.displayCount) {
        return &s_Video.displays[0];
    }
    return PAL_NULL;
}

const PalDisplay* _PCALL palGetDisplay(int index)
{
    if (s_Video.displayCount && (index >= 0 && index < s_Video.displayCount)) {
        return &s_Video.displays[index];
    }
    return PAL_NULL;
}
