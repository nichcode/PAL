
#include "pal_pch.h"
#include "pal_video_internal.h"

PalResult _PCALL palCreateVideo(PalAllocator* allocator, PalVideo** outVideo)
{
    PalVideo* video = PAL_NULL;
    if (!outVideo) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    if (allocator) {
        if (!allocator->alloc && !allocator->free) {
            palSetError(PAL_ERROR_INVALID_ALLOCATOR);
            return PAL_RESULT_FAIL;
        }
    }

    video = palAlloc(allocator, sizeof(PalVideo));
    if (!video) {
        palSetError(PAL_ERROR_ALLOCATION_FAILED);
        return PAL_RESULT_FAIL;
    }

    memset(video, 0, sizeof(PalVideo));
    video->allocator = allocator;

    *outVideo = video;
    return PAL_RESULT_OK;
}

void _PCALL palDestroyVideo(PalVideo* video)
{
    if (!video) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return;
    }
    
    PalAllocator* allocator = video->allocator;
    palFree(video->allocator, video);
}