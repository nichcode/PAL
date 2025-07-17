
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

PAL_DECLARE_HANDLE(PalVideo);
PAL_DECLARE_HANDLE(PalDisplay);

typedef struct PalDisplayInfo {
    char name[32];
    int x;
    int y;
    int width;
    int height;
    int refreshRate;
    float dpiScaleX;
    float dpiScaleY;
} PalDisplayInfo;

typedef struct PalVideoConfig {
    PalAllocator* allocator;
} PalVideoConfig;

_PAPI PalResult _PCALL palCreateVideo(
    PalVideoConfig* config,
    PalVideo** outVideo);

_PAPI void _PCALL palDestroyVideo(
    PalVideo* video);

_PAPI PalResult _PCALL palEnumerateDisplays(
    PalVideo* video,
    int* count,
    PalDisplay** displays);

_PAPI PalResult _PCALL palGetDisplayInfo(
    PalDisplay* display, 
    PalDisplayInfo* info);

_PAPI PalResult _PCALL palGetPrimaryDisplay(
    PalVideo* video, 
    PalDisplay** outDisplay);

#endif // _PAL_VIDEO_H