
#ifndef _PAL_VIDEO_INTERNAL_H
#define _PAL_VIDEO_INTERNAL_H

#include "pal/pal_video.h"

typedef struct PalVideoInstance
{
    PalAllocator* allocator;
    PalEventInstance* eventInstance;
    Uint32 nextWindowID;
} PalVideoInstance;

#endif // _PAL_VIDEO_INTERNAL_H