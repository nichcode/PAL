
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

PAL_DECLARE_HANDLE(PalVideo);

typedef struct PalVideoConfig {
    PalAllocator* allocator;
} PalVideoConfig;

_PAPI PalResult _PCALL palCreateVideo(
    PalVideoConfig* config,
    PalVideo** outVideo);

_PAPI void _PCALL palDestroyVideo(
    PalVideo* video);

#endif // _PAL_VIDEO_H