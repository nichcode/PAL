
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

PAL_DECLARE_HANDLE(PalVideo);
PAL_DECLARE_HANDLE(PalDisplay);
PAL_DECLARE_HANDLE(PalWindow);

typedef enum PalVideoFeatureFlags {
    PAL_VIDEO_PER_DISPLAY_DPI = PAL_BIT(0),
    PAL_VIDEO_HIGH_DPI = PAL_BIT(1),
    PAL_VIDEO_DISPLAY_ORIENTATION = PAL_BIT(2),
    PAL_VIDEO_BORDERLESS_WINDOW = PAL_BIT(3),
    PAL_VIDEO_FULLSCREEN = PAL_BIT(4),
    PAL_VIDEO_RESIZING = PAL_BIT(5),
    PAL_VIDEO_MULTI_DISPLAYS = PAL_BIT(6),
    PAL_VIDEO_WINDOW_MINMAX = PAL_BIT(7),
    PAL_VIDEO_WINDOW_POSITIONING = PAL_BIT(8),
    PAL_VIDEO_CLIP_CURSOR = PAL_BIT(9),
    PAL_VIDEO_GAMMA_CONTROL = PAL_BIT(10)
} PalVideoFeatureFlags;

typedef enum PalWindowFlags {
    PAL_WINDOW_SHOWN = PAL_BIT(0),
    PAL_WINDOW_MAXIMIZED = PAL_BIT(1),
    PAL_WINDOW_RESIZABLE = PAL_BIT(2),
    PAL_WINDOW_CENTER = PAL_BIT(3),
    PAL_WINDOW_MINIMIZEBOX = PAL_BIT(4),
    PAL_WINDOW_HIGH_DPI = PAL_BIT(5),
    PAL_APPWINDOW = PAL_WINDOW_RESIZABLE | PAL_WINDOW_MINIMIZEBOX | PAL_WINDOW_SHOWN
} PalWindowFlags;

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

typedef struct PalDisplayMode {
    int width;
    int height;
    int refreshRate;

    int redBits;
    int greenBits;
    int blueBits;
    int alphaBits;
} PalDisplayMode;

typedef struct PalVideoConfig {
    PalAllocator* allocator;
} PalVideoConfig;

typedef struct PalWindowConfig {
    const char* title;
    PalDisplay display;
    Uint32 width;
    Uint32 height;
    PalWindowFlags flags;
} PalWindowConfig;

_PAPI PalResult _PCALL palCreateVideo(
    PalVideoConfig* config,
    PalVideo* outVideo);

_PAPI void _PCALL palDestroyVideo(
    PalVideo video);

_PAPI PalResult _PCALL palGetVideoFeatures(
    PalVideo video,
    PalVideoFeatureFlags* features);

_PAPI PalResult _PCALL palUpdateVideo(
    PalVideo video);

_PAPI PalResult _PCALL palEnumerateDisplays(
    PalVideo video,
    int* count,
    PalDisplay* displays);

_PAPI PalResult _PCALL palGetDisplayInfo(
    PalDisplay display, 
    PalDisplayInfo* info);

_PAPI PalResult _PCALL palGetPrimaryDisplay(
    PalVideo video, 
    PalDisplay* outDisplay);

_PAPI PalResult _PCALL palEnumerateDisplayModes(
    PalDisplay display,
    int* count,
    PalDisplayMode* modes);

_PAPI PalResult _PCALL palCreateWindow(
    PalVideo video, 
    PalWindowConfig* config,
    PalWindow* outWindow);

_PAPI void _PCALL palDestroyWindow(PalWindow window);

#endif // _PAL_VIDEO_H