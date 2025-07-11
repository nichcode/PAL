
#include "pal_pch.h"
#include "pal_video_internal.h"

PalResult _PCALL palCreateVideoInstance(
    PalVideoInstanceDesc* desc, 
    PalVideoInstance** outVideoInstance)
{
    PalVideoInstance* videoInstance = PAL_NULL;
    if (!outVideoInstance || !desc) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

    if (desc->allocator) {
        if (!desc->allocator->alloc && !desc->allocator->free) {
            palSetError(PAL_ERROR_INVALID_ALLOCATOR);
            return PAL_RESULT_FAIL;
        }
    }

    videoInstance = palAlloc(desc->allocator, sizeof(PalVideoInstance));
    if (!videoInstance) {
        palSetError(PAL_ERROR_ALLOCATION_FAILED);
        return PAL_RESULT_FAIL;
    }

    memset(videoInstance, 0, sizeof(PalVideoInstance));
    if (desc->allocator) {
        videoInstance->eventInstance = desc->eventInstance;
    }

    if (desc->eventInstance) {
        videoInstance->eventInstance = desc->eventInstance;
    }

    videoInstance->nextWindowID = 1;
    *outVideoInstance = videoInstance;
    return PAL_RESULT_OK;
}

void _PCALL palDestroyVideoInstance(
    PalVideoInstance* videoInstance)
{
    if (!videoInstance) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return;
    }
    
    PalAllocator* allocator = videoInstance->allocator;
    palFree(allocator, videoInstance);
}

PalResult _PCALL palUpdateWindows(
    PalVideoInstance* videoInstance)
{
    if (!videoInstance) {
        palSetError(PAL_ERROR_NULL_POINTER);
        return PAL_RESULT_FAIL;
    }

#ifdef _WIN32
    MSG msg;
    while (PeekMessageA(&msg, PAL_NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
#endif // _WIN32
}