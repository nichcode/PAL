
#ifndef _PAL_VIDEO_INTERNAL_H
#define _PAL_VIDEO_INTERNAL_H

#include "pal/pal_video.h"

struct PalVideo_T {
    PalAllocator* allocator;
    PalEventDriver eventDriver;
    void* platformData;
    Uint32 nextWindowID;
    PalVideoFeatureFlags featureFlags;
};

Uint32 palGetSupportedFeatures();
PalResult palCreateVideoData(PalAllocator* allocator, void** outData);
void palDestroyVideoData(PalAllocator* allocator, void* data);

#endif // _PAL_VIDEO_INTERNAL_H