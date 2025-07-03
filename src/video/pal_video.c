
#include "pal_pch.h"
#include "pal_video_c.h"

PalVideo g_Video;

bool _PCALL palInitVideo(const PalAllocator* allocator)
{
    if (g_Video.initialized) {
        return PAL_TRUE;
    }

    if (allocator) {
        if (!allocator->alloc || !allocator->free) {
            palSetError(PAL_INVALID_ALLOCATOR);
            return PAL_FALSE;
        }

        g_Video.allocator.alloc = allocator->alloc;
        g_Video.allocator.free = allocator->free;
    }

    g_Video.allocator.alloc = palAlloc;
    g_Video.allocator.free = palFree;

    bool success = PAL_FALSE;
    success = palInitDisplay();
    if (!success) {
        return PAL_FALSE;
    }

    success = palInitWindow();
    if (!success) {
        return PAL_FALSE;
    }

    // init window hashmap
    g_Video.map = palCreateHashMap(&g_Video.allocator, PAL_MAX_WINDOWS);
    g_Video.nextWindowID = 1;

    g_Video.initialized = PAL_TRUE;
    return success;
}

void _PCALL palShutdownVideo()
{
    if (!g_Video.initialized) {
        return;
    }

    palDestroyHashMap(&g_Video.map);
    palShutdownDisplay();
    palShutdownWindow();

    g_Video.initialized = PAL_FALSE;
}

bool _PCALL palIsVideoInit()
{
    return g_Video.initialized;
}