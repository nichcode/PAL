
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

PAL_DEFINE_HANDLE(PalVideo);
PAL_DEFINE_HANDLE(PalDisplay);

typedef struct PalDisplayInfo
{
    char name[32];
    int x;
    int y;
    Uint32 width;
    Uint32 height;
    float dpiScaleX;
    float dpiScaleY;
    Uint32 refreshRate;

} PalDisplayInfo;

typedef struct PalDisplayMode
{
    Uint32 width;
    Uint32 height;
    Uint32 refreshRate;

    Uint16 redBits;
    Uint16 greenBits;
    Uint16 blueBits;
    Uint16 alphaBits;

} PalDisplayMode;

_PAPI PalResult _PCALL palCreateVideo(PalAllocator* allocator, PalVideo** outVideo);
_PAPI void _PCALL palDestroyVideo(PalVideo* video);

_PAPI PalResult _PCALL palEnumerateDisplays(
    PalVideo* video, 
    Uint32* count, 
    PalDisplay** displays);

_PAPI PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay* display, 
    Uint32* count, 
    PalDisplayMode* modes);

_PAPI PalResult _PCALL palGetDisplayInfo(PalDisplay* display, PalDisplayInfo* info);
_PAPI PalResult _PCALL palGetDisplayInfo(PalDisplay* display, PalDisplayInfo* info);

#endif // _PAL_VIDEO_H