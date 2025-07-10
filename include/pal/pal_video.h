
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

PAL_DEFINE_HANDLE(PalVideo);
PAL_DEFINE_HANDLE(PalDisplay);
PAL_DEFINE_HANDLE(PalWindow);

#define PAL_UNDEFINED -1

typedef enum PalWindowFlags
{
    PAL_WINDOW_SHOWN = PAL_BIT(1),
    PAL_WINDOW_MAXIMIZED = PAL_BIT(2),
    PAL_WINDOW_RESIZABLE = PAL_BIT(3),
    PAL_WINDOW_CENTER = PAL_BIT(4),
    PAL_WINDOW_MINIMIZEBOX = PAL_BIT(5),
    PAL_APPWINDOW = PAL_WINDOW_RESIZABLE | PAL_WINDOW_MINIMIZEBOX | PAL_WINDOW_SHOWN

} PalWindowFlags;

typedef struct PalWindowDesc
{
    const char* title;
    PalDisplay* display;
    Uint32 width;
    Uint32 height;
    PalWindowFlags flags;

} PalWindowDesc;

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
_PAPI PalResult _PCALL palUpdateWindows(PalVideo* video);

_PAPI PalResult _PCALL palEnumerateDisplays(
    PalVideo* video, 
    Uint32* count, 
    PalDisplay** displays);

_PAPI PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay* display, 
    Uint32* count, 
    PalDisplayMode* modes);

_PAPI PalResult _PCALL palGetDisplayInfo(PalDisplay* display, PalDisplayInfo* info);

_PAPI PalResult _PCALL palCreateWindow(
    PalVideo* video, 
    PalWindowDesc* desc, 
    PalWindow** outWindow);

_PAPI void _PCALL palDestroyWindow(PalWindow* window);

#endif // _PAL_VIDEO_H