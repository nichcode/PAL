
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_core.h"

typedef struct PalDisplay PalDisplay;

typedef enum {
    PAL_VIDEO_FEATURE_HIGH_DPI = PAL_BIT(0),
    PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION = PAL_BIT(1),
    PAL_VIDEO_FEATURE_BORDERLESS_WINDOW = PAL_BIT(2),
    PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH = PAL_BIT(3),
    PAL_VIDEO_FEATURE_MULTI_DISPLAYS = PAL_BIT(4),
    PAL_VIDEO_FEATURE_WINDOW_RESIZING = PAL_BIT(5),
    PAL_VIDEO_FEATURE_WINDOW_POSITIONING = PAL_BIT(6),
    PAL_VIDEO_FEATURE_WINDOW_MINMAX = PAL_BIT(7),
    PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL = PAL_BIT(8),
    PAL_VIDEO_FEATURE_CLIP_CURSOR = PAL_BIT(9),
    PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION = PAL_BIT(10),
    PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY = PAL_BIT(11)
} PalVideoFeatures;

typedef enum {
    PAL_ORIENTATION_LANDSCAPE,
    PAL_ORIENTATION_PORTRAIT,
    PAL_ORIENTATION_LANDSCAPE_FLIPPED,
    PAL_ORIENTATION_PORTRAIT_FLIPPED
} PalOrientation;

typedef struct {
    bool primary;
    Uint16 dpi;
    Uint16 refreshRate;
    Int32 x;
    Int32 y;
    Uint32 width;
    Uint32 height;
    PalOrientation orientation;
    char name[32];
} PalDisplayInfo;

PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator *allocator);

PAL_API void PAL_CALL palShutdownVideo();

PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

PAL_API PalResult PAL_CALL palEnumerateDisplays(
    Int32 *count,
    PalDisplay **displays);

PAL_API PalResult PAL_CALL palGetPrimaryDisplay(
    PalDisplay **outDisplay);

PAL_API PalResult PAL_CALL palGetDisplayInfo(
    PalDisplay *display,
    PalDisplayInfo *info);

#endif // _PAL_VIDEO_H