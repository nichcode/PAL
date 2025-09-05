
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal/pal_event.h"

typedef struct PalDisplay PalDisplay;
typedef struct PalWindow PalWindow;

typedef enum {
    PAL_VIDEO_FEATURE_HIGH_DPI = PAL_BIT(0),
    PAL_VIDEO_FEATURE_DISPLAY_ORIENTATION = PAL_BIT(1),
    PAL_VIDEO_FEATURE_BORDERLESS_WINDOW = PAL_BIT(2),
    PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW = PAL_BIT(3),
    PAL_VIDEO_FEATURE_TOOL_WINDOW = PAL_BIT(4),
    PAL_VIDEO_FEATURE_DISPLAY_MODE_SWITCH = PAL_BIT(5),
    PAL_VIDEO_FEATURE_MULTI_DISPLAYS = PAL_BIT(6),
    PAL_VIDEO_FEATURE_WINDOW_RESIZING = PAL_BIT(7),
    PAL_VIDEO_FEATURE_WINDOW_POSITIONING = PAL_BIT(8),
    PAL_VIDEO_FEATURE_WINDOW_MINMAX = PAL_BIT(9),
    PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX = PAL_BIT(10),
    PAL_VIDEO_FEATURE_NO_MINIMIZEBOX = PAL_BIT(11),
    PAL_VIDEO_FEATURE_DISPLAY_GAMMA_CONTROL = PAL_BIT(12),
    PAL_VIDEO_FEATURE_CLIP_CURSOR = PAL_BIT(13),
    PAL_VIDEO_FEATURE_WINDOW_FLASH_CAPTION = PAL_BIT(14),
    PAL_VIDEO_FEATURE_WINDOW_FLASH_TRAY = PAL_BIT(15)
} PalVideoFeatures;

typedef enum {
    PAL_ORIENTATION_LANDSCAPE,
    PAL_ORIENTATION_PORTRAIT,
    PAL_ORIENTATION_LANDSCAPE_FLIPPED,
    PAL_ORIENTATION_PORTRAIT_FLIPPED
} PalOrientation;

typedef enum {
    PAL_WINDOW_STYLE_RESIZABLE = PAL_BIT(0),
    PAL_WINDOW_STYLE_TRANSPARENT = PAL_BIT(1),
    PAL_WINDOW_STYLE_TOPMOST = PAL_BIT(2),
    PAL_WINDOW_STYLE_NO_MINIMIZEBOX = PAL_BIT(3),
    PAL_WINDOW_STYLE_NO_MAXIMIZEBOX = PAL_BIT(4),
    PAL_WINDOW_STYLE_TOOL = PAL_BIT(5),
    PAL_WINDOW_STYLE_BORDERLESS = PAL_BIT(6)
} PalWindowStyle;

typedef enum {
    PAL_FLASH_STOP = 0,
    PAL_FLASH_CAPTION = PAL_BIT(0),
    PAL_FLASH_TRAY = PAL_BIT(1)
} PalFlashFlags;

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

typedef struct {
    Uint16 bpp;
    Uint16 refreshRate;
    Uint32 width;
    Uint32 height;
} PalDisplayMode;

typedef struct {
    Uint16 interval;
    PalFlashFlags flags;
    Uint32 count;
} PalFlashInfo;

typedef struct {
    bool show;
    bool showMaximized;
    bool showMinimized;
    bool center;
    Uint32 width;
    Uint32 height;
    PalWindowStyle style;
    const char* title;
    PalDisplay* display;
} PalWindowCreateInfo;

PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator *allocator,
    PalEventDriver* eventDriver);

PAL_API void PAL_CALL palShutdownVideo();

PAL_API void PAL_CALL palUpdateVideo();

PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

PAL_API PalResult PAL_CALL palEnumerateDisplays(
    Int32 *count,
    PalDisplay **displays);

PAL_API PalResult PAL_CALL palGetPrimaryDisplay(
    PalDisplay **outDisplay);

PAL_API PalResult PAL_CALL palGetDisplayInfo(
    PalDisplay *display,
    PalDisplayInfo *info);

PAL_API PalResult PAL_CALL palEnumerateDisplayModes(
    PalDisplay *display,
    Int32 *count,
    PalDisplayMode *modes);

PAL_API PalResult PAL_CALL palGetCurrentDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode);

PAL_API PalResult PAL_CALL palSetDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode);

PAL_API PalResult PAL_CALL palValidateDisplayMode(
    PalDisplay *display,
    PalDisplayMode *mode);

PAL_API PalResult PAL_CALL palSetDisplayOrientation(
    PalDisplay *display,
    PalOrientation orientation);

PAL_API PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo *info,
    PalWindow **outWindow);

PAL_API void PAL_CALL palDestroyWindow(
    PalWindow *window);

PAL_API PalResult PAL_CALL palMinimizeWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palMaximizeWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palRestoreWindow(
    PalWindow* window);

PAL_API void PAL_CALL palShowWindow(
    PalWindow* window);

PAL_API void PAL_CALL palHideWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

PAL_API PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle);

PAL_API PalResult PAL_CALL palGetWindowDisplay(
    PalWindow* window, 
    PalDisplay** outDisplay);

PAL_API PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

PAL_API PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

#endif // _PAL_VIDEO_H