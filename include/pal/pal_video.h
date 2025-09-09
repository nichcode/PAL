
#ifndef _PAL_VIDEO_H
#define _PAL_VIDEO_H

#include "pal_event.h"

typedef struct PalMonitor PalMonitor;
typedef struct PalWindow PalWindow;
typedef struct PalIcon PalIcon;
typedef struct PalCursor PalCursor;

typedef enum {
    PAL_VIDEO_FEATURE_HIGH_DPI = PAL_BIT(0),
    PAL_VIDEO_FEATURE_MONITOR_ORIENTATION = PAL_BIT(1),
    PAL_VIDEO_FEATURE_BORDERLESS_WINDOW = PAL_BIT(2),
    PAL_VIDEO_FEATURE_TRANSPARENT_WINDOW = PAL_BIT(3),
    PAL_VIDEO_FEATURE_TOOL_WINDOW = PAL_BIT(4),
    PAL_VIDEO_FEATURE_MONITOR_MODE_SWITCH = PAL_BIT(5),
    PAL_VIDEO_FEATURE_MULTI_MONITORS = PAL_BIT(6),
    PAL_VIDEO_FEATURE_WINDOW_RESIZING = PAL_BIT(7),
    PAL_VIDEO_FEATURE_WINDOW_POSITIONING = PAL_BIT(8),
    PAL_VIDEO_FEATURE_WINDOW_MINMAX = PAL_BIT(9),
    PAL_VIDEO_FEATURE_NO_MAXIMIZEBOX = PAL_BIT(10),
    PAL_VIDEO_FEATURE_NO_MINIMIZEBOX = PAL_BIT(11),
    PAL_VIDEO_FEATURE_MONITOR_GAMMA_CONTROL = PAL_BIT(12),
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
    PAL_WINDOW_STATE_MAXIMIZED,
    PAL_WINDOW_STATE_MINIMIZED,
    PAL_WINDOW_STATE_RESTORED
} PalWindowState;

typedef enum {
    PAL_FLASH_STOP = 0,
    PAL_FLASH_CAPTION = PAL_BIT(0),
    PAL_FLASH_TRAY = PAL_BIT(1)
} PalFlashFlags;

typedef enum {
    PAL_SCANCODE_UNKNOWN = 0,

    // Letters
    PAL_SCANCODE_A,
    PAL_SCANCODE_B,
    PAL_SCANCODE_C,
    PAL_SCANCODE_D,
    PAL_SCANCODE_E,
    PAL_SCANCODE_F,
    PAL_SCANCODE_G,
    PAL_SCANCODE_H,
    PAL_SCANCODE_I,
    PAL_SCANCODE_J,
    PAL_SCANCODE_K,
    PAL_SCANCODE_L,
    PAL_SCANCODE_M,
    PAL_SCANCODE_N,
    PAL_SCANCODE_O,
    PAL_SCANCODE_P,
    PAL_SCANCODE_Q,
    PAL_SCANCODE_R,
    PAL_SCANCODE_S,
    PAL_SCANCODE_T,
    PAL_SCANCODE_U,
    PAL_SCANCODE_V,
    PAL_SCANCODE_W,
    PAL_SCANCODE_X,
    PAL_SCANCODE_Y,
    PAL_SCANCODE_Z,

    // Numbers (top row)
    PAL_SCANCODE_0,
    PAL_SCANCODE_1,
    PAL_SCANCODE_2,
    PAL_SCANCODE_3,
    PAL_SCANCODE_4,
    PAL_SCANCODE_5,
    PAL_SCANCODE_6,
    PAL_SCANCODE_7,
    PAL_SCANCODE_8,
    PAL_SCANCODE_9,

    // Function
    PAL_SCANCODE_F1,
    PAL_SCANCODE_F2,
    PAL_SCANCODE_F3,
    PAL_SCANCODE_F4,
    PAL_SCANCODE_F5,
    PAL_SCANCODE_F6,
    PAL_SCANCODE_F7,
    PAL_SCANCODE_F8,
    PAL_SCANCODE_F9,
    PAL_SCANCODE_F10,
    PAL_SCANCODE_F11,
    PAL_SCANCODE_F12,

    // Control
    PAL_SCANCODE_ESCAPE,
    PAL_SCANCODE_ENTER,
    PAL_SCANCODE_TAB,
    PAL_SCANCODE_BACKSPACE,
    PAL_SCANCODE_SPACE,
    PAL_SCANCODE_CAPSLOCK,
    PAL_SCANCODE_NUMLOCK,
    PAL_SCANCODE_SCROLLLOCK,
    PAL_SCANCODE_LSHIFT,
    PAL_SCANCODE_RSHIFT,
    PAL_SCANCODE_LCTRL,
    PAL_SCANCODE_RCTRL,
    PAL_SCANCODE_LALT,
    PAL_SCANCODE_RALT,

    // Arrows
    PAL_SCANCODE_LEFT,
    PAL_SCANCODE_RIGHT,
    PAL_SCANCODE_UP,
    PAL_SCANCODE_DOWN,

    // Navigation
    PAL_SCANCODE_INSERT,
    PAL_SCANCODE_DELETE,
    PAL_SCANCODE_HOME,
    PAL_SCANCODE_END,
    PAL_SCANCODE_PAGEUP,
    PAL_SCANCODE_PAGEDOWN,

    // Keypad
    PAL_SCANCODE_KP_0,
    PAL_SCANCODE_KP_1,
    PAL_SCANCODE_KP_2,
    PAL_SCANCODE_KP_3,
    PAL_SCANCODE_KP_4,
    PAL_SCANCODE_KP_5,
    PAL_SCANCODE_KP_6,
    PAL_SCANCODE_KP_7,
    PAL_SCANCODE_KP_8,
    PAL_SCANCODE_KP_9,
    PAL_SCANCODE_KP_ENTER,
    PAL_SCANCODE_KP_ADD,
    PAL_SCANCODE_KP_SUBTRACT,
    PAL_SCANCODE_KP_MULTIPLY,
    PAL_SCANCODE_KP_DIVIDE,
    PAL_SCANCODE_KP_DECIMAL,
    PAL_SCANCODE_KP_EQUAL,

    // Misc
    PAL_SCANCODE_PRINTSCREEN,
    PAL_SCANCODE_PAUSE,
    PAL_SCANCODE_MENU,
    PAL_SCANCODE_APOSTROPHE,
    PAL_SCANCODE_BACKSLASH,
    PAL_SCANCODE_COMMA,
    PAL_SCANCODE_EQUAL,
    PAL_SCANCODE_GRAVEACCENT,
    PAL_SCANCODE_SUBTRACT,
    PAL_SCANCODE_PERIOD,
    PAL_SCANCODE_SEMICOLON,
    PAL_SCANCODE_SLASH,
    PAL_SCANCODE_LBRACKET,
    PAL_SCANCODE_RBRACKET,
    PAL_SCANCODE_LSUPER,
    PAL_SCANCODE_RSUPER,

    PAL_SCANCODE_MAX
} PalScancode;

typedef enum {
    PAL_KEYCODE_UNKNOWN = 0,

    // Letters
    PAL_KEYCODE_A,
    PAL_KEYCODE_B,
    PAL_KEYCODE_C,
    PAL_KEYCODE_D,
    PAL_KEYCODE_E,
    PAL_KEYCODE_F,
    PAL_KEYCODE_G,
    PAL_KEYCODE_H,
    PAL_KEYCODE_I,
    PAL_KEYCODE_J,
    PAL_KEYCODE_K,
    PAL_KEYCODE_L,
    PAL_KEYCODE_M,
    PAL_KEYCODE_N,
    PAL_KEYCODE_O,
    PAL_KEYCODE_P,
    PAL_KEYCODE_Q,
    PAL_KEYCODE_R,
    PAL_KEYCODE_S,
    PAL_KEYCODE_T,
    PAL_KEYCODE_U,
    PAL_KEYCODE_V,
    PAL_KEYCODE_W,
    PAL_KEYCODE_X,
    PAL_KEYCODE_Y,
    PAL_KEYCODE_Z,

    // Numbers (top row)
    PAL_KEYCODE_0,
    PAL_KEYCODE_1,
    PAL_KEYCODE_2,
    PAL_KEYCODE_3,
    PAL_KEYCODE_4,
    PAL_KEYCODE_5,
    PAL_KEYCODE_6,
    PAL_KEYCODE_7,
    PAL_KEYCODE_8,
    PAL_KEYCODE_9,

    // Function
    PAL_KEYCODE_F1,
    PAL_KEYCODE_F2,
    PAL_KEYCODE_F3,
    PAL_KEYCODE_F4,
    PAL_KEYCODE_F5,
    PAL_KEYCODE_F6,
    PAL_KEYCODE_F7,
    PAL_KEYCODE_F8,
    PAL_KEYCODE_F9,
    PAL_KEYCODE_F10,
    PAL_KEYCODE_F11,
    PAL_KEYCODE_F12,

    // Control
    PAL_KEYCODE_ESCAPE,
    PAL_KEYCODE_ENTER,
    PAL_KEYCODE_TAB,
    PAL_KEYCODE_BACKSPACE,
    PAL_KEYCODE_SPACE,
    PAL_KEYCODE_CAPSLOCK,
    PAL_KEYCODE_NUMLOCK,
    PAL_KEYCODE_SCROLLLOCK,
    PAL_KEYCODE_LSHIFT,
    PAL_KEYCODE_RSHIFT,
    PAL_KEYCODE_LCTRL,
    PAL_KEYCODE_RCTRL,
    PAL_KEYCODE_LALT,
    PAL_KEYCODE_RALT,

    // Arrows
    PAL_KEYCODE_LEFT,
    PAL_KEYCODE_RIGHT,
    PAL_KEYCODE_UP,
    PAL_KEYCODE_DOWN,

    // Navigation
    PAL_KEYCODE_INSERT,
    PAL_KEYCODE_DELETE,
    PAL_KEYCODE_HOME,
    PAL_KEYCODE_END,
    PAL_KEYCODE_PAGEUP,
    PAL_KEYCODE_PAGEDOWN,

    // Keypad
    PAL_KEYCODE_KP_0,
    PAL_KEYCODE_KP_1,
    PAL_KEYCODE_KP_2,
    PAL_KEYCODE_KP_3,
    PAL_KEYCODE_KP_4,
    PAL_KEYCODE_KP_5,
    PAL_KEYCODE_KP_6,
    PAL_KEYCODE_KP_7,
    PAL_KEYCODE_KP_8,
    PAL_KEYCODE_KP_9,
    PAL_KEYCODE_KP_ENTER,
    PAL_KEYCODE_KP_ADD,
    PAL_KEYCODE_KP_SUBTRACT,
    PAL_KEYCODE_KP_MULTIPLY,
    PAL_KEYCODE_KP_DIVIDE,
    PAL_KEYCODE_KP_DECIMAL,
    PAL_KEYCODE_KP_EQUAL,

    // Misc
    PAL_KEYCODE_PRINTSCREEN,
    PAL_KEYCODE_PAUSE,
    PAL_KEYCODE_MENU,
    PAL_KEYCODE_APOSTROPHE,
    PAL_KEYCODE_BACKSLASH,
    PAL_KEYCODE_COMMA,
    PAL_KEYCODE_EQUAL,
    PAL_KEYCODE_GRAVEACCENT,
    PAL_KEYCODE_SUBTRACT,
    PAL_KEYCODE_PERIOD,
    PAL_KEYCODE_SEMICOLON,
    PAL_KEYCODE_SLASH,
    PAL_KEYCODE_LBRACKET,
    PAL_KEYCODE_RBRACKET,
    PAL_KEYCODE_LSUPER,
    PAL_KEYCODE_RSUPER,

    PAL_KEYCODE_MAX
} PalKeycode;

typedef enum {
    PAL_MOUSE_BUTTON_UNKNOWN = 0,

    PAL_MOUSE_BUTTON_LEFT,
    PAL_MOUSE_BUTTON_RIGHT,
    PAL_MOUSE_BUTTON_MIDDLE,
    PAL_MOUSE_BUTTON_X1,
    PAL_MOUSE_BUTTON_X2,

    PAL_MOUSE_BUTTON_MAX
} PalMouseButton;

typedef struct {
    bool primary;
    Uint32 dpi;
    Uint32 refreshRate;
    Int32 x;
    Int32 y;
    Uint32 width;
    Uint32 height;
    PalOrientation orientation;
    char name[32];
} PalMonitorInfo;

typedef struct {
    Uint32 bpp;
    Uint32 refreshRate;
    Uint32 width;
    Uint32 height;
} PalMonitorMode;

typedef struct {
    Uint32 interval;
    PalFlashFlags flags;
    Uint32 count;
} PalFlashInfo;

typedef struct {
    Uint32 width;
    Uint32 height;
    const Uint8* pixels;
} PalIconCreateInfo;

typedef struct {
    Uint32 width;
    Uint32 height;
    Int32 xHotspot;
    Int32 yHotspot;
    const Uint8* pixels;
} PalCursorCreateInfo;

typedef struct {
    void* nativeDisplay;
    void* nativeWindow;
} PalWindowHandleInfo;

typedef struct {
    bool show;
    bool showMaximized;
    bool showMinimized;
    bool center;
    Uint32 width;
    Uint32 height;
    PalWindowStyle style;
    const char* title;
    PalMonitor* monitor;
} PalWindowCreateInfo;

PAL_API PalResult PAL_CALL palInitVideo(
    const PalAllocator *allocator,
    PalEventDriver* eventDriver);

PAL_API void PAL_CALL palShutdownVideo();

PAL_API void PAL_CALL palUpdateVideo();

PAL_API PalVideoFeatures PAL_CALL palGetVideoFeatures();

PAL_API PalResult PAL_CALL palEnumerateMonitors(
    Int32 *count,
    PalMonitor **outMonitors);

PAL_API PalResult PAL_CALL palGetPrimaryMonitor(
    PalMonitor **outMonitor);

PAL_API PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor *monitor,
    PalMonitorInfo *info);

PAL_API PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor *monitor,
    Int32 *count,
    PalMonitorMode *modes);

PAL_API PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode);

PAL_API PalResult PAL_CALL palSetMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode);

PAL_API PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor *monitor,
    PalMonitorMode *mode);

PAL_API PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor *monitor,
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

PAL_API PalResult PAL_CALL palShowWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palHideWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info);

PAL_API PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle);

PAL_API PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window, 
    PalMonitor** outMonitor);

PAL_API PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    char** outTitle);

PAL_API PalResult PAL_CALL palGetWindowPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y);

PAL_API PalResult PAL_CALL palGetWindowClientPos(
    PalWindow* window, 
    Int32* x, 
    Int32* y);

PAL_API PalResult PAL_CALL palGetWindowSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height);

PAL_API PalResult PAL_CALL palGetWindowClientSize(
    PalWindow* window, 
    Uint32* width, 
    Uint32* height);

PAL_API PalResult PAL_CALL palGetWindowState(
    PalWindow* window, 
    PalWindowState* state);

PAL_API const bool* PAL_CALL palGetKeycodeState();

PAL_API const bool* PAL_CALL palGetScancodeState();

PAL_API const bool* PAL_CALL palGetMouseState();

PAL_API void PAL_CALL palGetMouseDelta(
    Int32* dx,
    Int32* dy);

void PAL_CALL palGetMouseWheelDelta(
    Int32* dx, 
    Int32* dy);

PAL_API bool PAL_CALL palIsWindowVisible(
    PalWindow* window);

PAL_API PalWindow* PAL_CALL palGetFocusWindow();

PAL_API PalWindow* PAL_CALL palGetForegroundWindow();

PAL_API PalWindowHandleInfo PAL_CALL palGetWindowHandleInfo(
    PalWindow* window);

PAL_API PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity);

PAL_API PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style);

PAL_API PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window, 
    const char* title);

PAL_API PalResult PAL_CALL palSetWindowPos(
    PalWindow* window, 
    Int32 x, 
    Int32 y);

PAL_API PalResult PAL_CALL palSetWindowSize(
    PalWindow* window, 
    Uint32 width, 
    Uint32 height);

PAL_API PalResult PAL_CALL palSetFocusWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palSetForegroundWindow(
    PalWindow* window);

PAL_API PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon);

PAL_API void PAL_CALL palDestroyIcon(
    PalIcon* icon);

PAL_API PalResult PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon);

PAL_API PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor);

PAL_API void PAL_CALL palDestroyCursor(
    PalCursor* cursor);

PAL_API void PAL_CALL palShowCursor(
    bool show);

PAL_API void PAL_CALL palClipCursor(
    PalWindow* window,
    bool clip);

PAL_API void PAL_CALL palGetCursorPos(
    PalWindow* window,
    Int32* x,
    Int32* y);

PAL_API void PAL_CALL palSetCursorPos(
    PalWindow* window,
    Int32 x,
    Int32 y);

PAL_API void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor);

#endif // _PAL_VIDEO_H