
#include "pal_pch.h"
#include "pal_video_internal.h"

// ==================================================
// Public API
// ==================================================

PalResult _PCALL palCreateVideo(
    PalVideoConfig* config,
    PalVideo* outVideo) {

    PalVideo video;
    if (!config || !outVideo) {
        return PAL_ERROR_NULL_POINTER;
    }

    if (config->allocator && (!config->allocator->alloc || !config->allocator->free)) {
        return PAL_ERROR_INVALID_ALLOCATOR;
    }

    video = palAllocate(config->allocator, sizeof(struct PalVideo_T));
    if (!video) {
        return PAL_ERROR_OUT_OF_MEMORY;
    }

    palZeroMemory(video, sizeof(struct PalVideo_T));
    if (config->allocator) {
        video->allocator = config->allocator;
    }

    if (config->eventDriver) {
        video->eventDriver = config->eventDriver;
    }

    PalResult result = palCreateVideoData(video->allocator, &video->platformData);
    if (result != PAL_SUCCESS) {
        return result;
    }

    video->featureFlags = palGetSupportedFeatures();
    *outVideo = video;
    return PAL_SUCCESS;
}

void _PCALL palDestroyVideo(
    PalVideo video) {
    
    if (!video) {
        return;
    }
    palDestroyVideoData(video->allocator, video->platformData);
    palFree(video->allocator, video);
}

PalResult _PCALL palGetVideoFeatures(
    PalVideo video,
    PalVideoFeatureFlags* features) {

    if (!video || !features) {
        return PAL_ERROR_NULL_POINTER;
    }

    *features = video->featureFlags;
    return PAL_SUCCESS;
}