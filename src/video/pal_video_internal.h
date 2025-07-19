
#ifndef _PAL_VIDEO_INTERNAL_H
#define _PAL_VIDEO_INTERNAL_H

#include "pal/pal_video.h"

struct PalVideo_T {
    PalAllocator* allocator;
    PalEventDriver eventDriver;
    Uint32 nextWindowID;
    Uint32 windowCount;
    PalVideoFeatureFlags featureFlags;
};

Uint32 palGetSupportedFeatures();

#endif // _PAL_VIDEO_INTERNAL_H