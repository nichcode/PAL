
/**
 * Copyright (C) 2025-2026 Nicholas Agbo <agbonicholas04@gmail.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef VIDEO_PLATFORM_H
#define VIDEO_PLATFORM_H

#include "shared.h"
#include "pal2/video/init.h"
#include "pal2/video/cursor.h"
#include "pal2/video/icon.h"
#include "pal2/video/monitor.h"

#include "pal2/video/keyboard.h"
#include "pal2/video/mouse.h"
#include "pal2/video/window.h"

/**
 * Platform contract for the video module.
 *
 * Every platform must implement every function declared here. A function
 * that says it has the same semantics and rules as a public `pal*` function
 * has only the platform requirements. A function with no public counterpart
 * is documented in full. These rules apply to all of them:
 *
 * - Arguments are validated by the caller. A platform function can assume
 *   every pointer it receives is valid and must not validate it again.
 *   Any exception will be documented by the function.
 */

/* =========================================================================
 * Init
 * ========================================================================= */

/** Gets the video driver of a video backend. 
 * 
 * Additional rules:
 * - The fucntion must exist before the backend is initialized
*/
typedef PalVideoDriver* (*GetVideoDriverFn)(void);

/** Initializes the video backend. 
 * 
 * Additional rules:
 * - `preferredInstance` is the users requested instance. It may be nullptr.
 * - `selectedInstance` is the actual instance used. This may be the same as `preferredInstance`.
*/
typedef PalResult (*InitVideoFn)(
    const PalAllocator* allocator,
    struct PalEventDriver* eventDriver,
    const void* preferredInstance,
    void** selectedInstance);

/** This function has the same semantics and rules as palShutdownVideo(). */
typedef void (*ShutdownVideoFn)(void);

/** This function has the same semantics and rules as palUpdateVideo().
 * 
 * Additional rules:
 * This function must not block.
*/
typedef void (*UpdateVideoFn)(void);

typedef struct Keyboard {
    PalBool scancodeState[PAL_SCANCODE_COUNT];
    PalBool keycodeState[PAL_KEYCODE_COUNT];
} Keyboard;

typedef struct Mouse {
    PalBool state[PAL_MOUSE_BUTTON_COUNT];
    int32_t dx;
    int32_t dy;
    int32_t WheelX;
    int32_t WheelY;
} Mouse;


/* =========================================================================
 * Monitor
 * ========================================================================= */

/** This function has the same semantics and rules as palEnumerateMonitors(). */
typedef PalResult (*EnumerateMonitorsFn)(
    uint32_t* count,
    PalMonitor** monitors);

/** This function has the same semantics and rules as palGetPrimaryMonitor(). */
typedef void (*GetPrimaryMonitorFn)(PalMonitor** monitor);

/** This function has the same semantics and rules as palGetMonitorInfo(). */
typedef void (*GetMonitorInfoFn)(
    PalMonitor* monitor,
    PalMonitorInfo* info);

/** This function has the same semantics and rules as palEnumerateMonitorModes(). */
typedef void (*EnumerateMonitorModesFn)(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes);

/** This function has the same semantics and rules as palGetCurrentMonitorMode(). */
typedef void (*GetCurrentMonitorModeFn)(
    PalMonitor* monitor,
    PalMonitorMode* mode);

/** This function has the same semantics and rules as palSetMonitorMode(). */
typedef PalResult (*SetMonitorModeFn)(
    PalMonitor* monitor,
    const PalMonitorMode* mode);

/** This function has the same semantics and rules as palValidateMonitorMode(). */
typedef PalResult (*ValidateMonitorModeFn)(
    PalMonitor* monitor,
    const PalMonitorMode* mode);

/** This function has the same semantics and rules as palSetMonitorOrientation(). */
typedef PalResult (*SetMonitorOrientationFn)(
    PalMonitor* monitor,
    PalOrientation orientation);


/* =========================================================================
 * Window
 * ========================================================================= */

/** This function has the same semantics and rules as palCreateWindow(). */
typedef PalResult (*CreateWindowFn)(
    const PalWindowCreateInfo* info, 
    PalWindow** outWindow);

/** This function has the same semantics and rules as palDestroyWindow(). */
typedef void (*DestroyWindowFn)(PalWindow* window);

/** This function has the same semantics and rules as palMaximizeWindow(). */
typedef void (*MaximizeWindowFn)(PalWindow* window);

/** This function has the same semantics and rules as palMinimizeWindow(). */
typedef void (*MinimizeWindowFn)(PalWindow* window);

/** This function has the same semantics and rules as palRestoreWindow(). */
typedef void (*RestoreWindowFn)(PalWindow* window);

/** This function has the same semantics and rules as palShowWindow(). */
typedef void (*ShowWindowFn)(PalWindow* window);

/** This function has the same semantics and rules as palHideWindow(). */
typedef void (*HideWindowFn)(PalWindow* window);

/** This function has the same semantics and rules as palFlashWindow(). */
typedef void (*FlashWindowFn)(
    PalWindow* window, 
    const PalFlashInfo* info);

/** This function has the same semantics and rules as palGetWindowStyle(). */
typedef void (*GetWindowStyleFn)(
    PalWindow* window, 
    PalWindowStyle* style);

/** This function has the same semantics and rules as palGetWindowMonitor(). */
typedef void (*GetWindowMonitorFn)(
    PalWindow* window, 
    PalMonitor** outMonitor);

/** This function has the same semantics and rules as palGetWindowTitle(). */
typedef void (*GetWindowTitleFn)(
    PalWindow* window, 
    uint64_t bufferSize, 
    uint64_t* outSize, 
    char* outBuffer);

/** This function has the same semantics and rules as palGetWindowPos(). */
typedef void (*GetWindowPosFn)(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y);

/** This function has the same semantics and rules as palGetWindowSize(). */
typedef void (*GetWindowSizeFn)(
    PalWindow* window, 
    uint32_t* width, 
    uint32_t* height);

/** This function has the same semantics and rules as palGetWindowState(). */
typedef void (*GetWindowStateFn)(
    PalWindow* window, 
    PalWindowState* state);

/** This function has the same semantics and rules as palIsWindowVisible(). */
typedef PalBool (*IsWindowVisibleFn)(PalWindow* window);

/** This function has the same semantics and rules as palGetFocusWindow(). */
typedef PalWindow* (*GetFocusWindowFn)(void);

/** This function has the same semantics and rules as palGetWindowHandleInfo(). */
typedef void (*GetWindowHandleInfoFn)(
    PalWindow* window, 
    PalWindowHandleInfo* info);

/** This function has the same semantics and rules as palSetWindowOpacity(). */
typedef void (*SetWindowOpacityFn)(
    PalWindow* window, 
    float opacity);

/** This function has the same semantics and rules as palSetWindowStyle(). */
typedef void (*SetWindowStyleFn)(
    PalWindow* window, 
    PalWindowStyle style);

/** This function has the same semantics and rules as palSetWindowTitle(). */
typedef void (*SetWindowTitleFn)(
    PalWindow* window, 
    const char* title);

/** This function has the same semantics and rules as palSetWindowPos(). */
typedef void (*SetWindowPosFn)(
    PalWindow* window, 
    int32_t x, 
    int32_t y);

/** This function has the same semantics and rules as palSetWindowSize(). */
typedef void (*SetWindowSizeFn)(
    PalWindow* window, 
    uint32_t width, 
    uint32_t height);

/** This function has the same semantics and rules as palSetFocusWindow(). */
typedef void (*SetFocusWindowFn)(PalWindow* window);


/* =========================================================================
 * Icon
 * ========================================================================= */

/** This function has the same semantics and rules as palCreateIcon(). */
typedef PalResult (*CreateIconFn)(
    const PalIconCreateInfo* info,
    PalIcon** icon);

/** This function has the same semantics and rules as palDestroyIcon(). */
typedef void (*DestroyIconFn)(PalIcon* icon);

/** This function has the same semantics and rules as palSetWindowIcon(). */
typedef void (*SetWindowIconFn)(
    PalWindow* window,
    PalIcon* icon);


/* =========================================================================
 * Cursor
 * ========================================================================= */

/** This function has the same semantics and rules as palCreateCursor(). */
typedef PalResult (*CreateCursorFn)(
    const PalCursorCreateInfo* info,
    PalCursor** cursor);

/** This function has the same semantics and rules as palCreateCursorFrom(). */
typedef PalResult (*CreateCursorFromFn)(
    PalCursorType type,
    PalCursor** cursor);

/** This function has the same semantics and rules as palDestroyCursor(). */
typedef void (*DestroyCursorFn)(PalCursor* cursor);

/** This function has the same semantics and rules as palShowCursor(). */
typedef void (*ShowCursorFn)(PalBool show);

/** This function has the same semantics and rules as palClipCursor(). */
typedef void (*ClipCursorFn)(
    PalWindow* window,
    PalBool clip);

/** This function has the same semantics and rules as palGetCursorPos(). */
typedef void (*GetCursorPosFn)(
    PalWindow* window,
    int32_t* x,
    int32_t* y);

/** This function has the same semantics and rules as palSetCursorPos(). */
typedef void (*SetCursorPosFn)(
    PalWindow* window,
    int32_t x,
    int32_t y);

/** This function has the same semantics and rules as palSetWindowCursor(). */
typedef void (*SetWindowCursorFn)(
    PalWindow* window,
    PalCursor* cursor);


/** A video backend. */
typedef struct VideoBackend {
    GetVideoDriverFn getDriver;
    InitVideoFn init;
    ShutdownVideoFn shutdown;
    UpdateVideoFn update;
    EnumerateMonitorsFn enumerateMonitors;
    GetPrimaryMonitorFn getPrimaryMonitor;
    GetMonitorInfoFn getMonitorInfo;
    EnumerateMonitorModesFn enumerateMonitorModes;
    GetCurrentMonitorModeFn getCurrentMonitorMode;
    SetMonitorModeFn setMonitorMode;
    ValidateMonitorModeFn validateMonitorMode;
    SetMonitorOrientationFn setMonitorOrientation;

    CreateWindowFn createWindow;
    DestroyWindowFn destroyWindow;
    MaximizeWindowFn maximizeWindow;
    MinimizeWindowFn minimizeWindow;
    RestoreWindowFn restoreWindow;
    ShowWindowFn showWindow;
    HideWindowFn hideWindow;
    FlashWindowFn flashWindow;
    GetWindowStyleFn getWindowStyle;
    GetWindowMonitorFn getWindowMonitor;
    GetWindowTitleFn getWindowTitle;
    GetWindowPosFn getWindowPos;
    GetWindowSizeFn getWindowSize;
    GetWindowStateFn getWindowState;
    IsWindowVisibleFn isWindowVisible;
    GetFocusWindowFn getFocusWindow;
    GetWindowHandleInfoFn getWindowHandleInfo;
    SetWindowOpacityFn setWindowOpacity;
    SetWindowStyleFn setWindowStyle;
    SetWindowTitleFn setWindowTitle;
    SetWindowPosFn setWindowPos;
    SetWindowSizeFn setWindowSize;
    SetFocusWindowFn setFocusWindow;

    CreateIconFn createIcon;
    DestroyIconFn destroyIcon;
    SetWindowIconFn setWindowIcon;
    CreateCursorFn createCursor;
    CreateCursorFromFn createCursorFrom;
    DestroyCursorFn destroyCursor;
    ShowCursorFn showCursor;
    ClipCursorFn clipCursor;
    GetCursorPosFn getCursorPos;
    SetCursorPosFn setCursorPos;
    SetWindowCursorFn setWindowCursor;
} VideoBackend;


extern const VideoBackend* g_VideoBackend; /**< Active video backend. */
extern Keyboard g_Keyboard;
extern Mouse g_Mouse;
extern PalBool g_VideoInitialized;

/** Gets all supported backends per platform. */
const VideoBackend* const* platformGetVideoBackends(uint32_t* count);

#endif // VIDEO_PLATFORM_H