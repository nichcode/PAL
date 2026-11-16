
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

#include "video/platform.h"

#if PAL_HAS_WAYLAND_BACKEND

PalVideoDriver* wlGetVideoDriver(void);
PalResult wlInitVideo(const PalAllocator*, struct PalEventDriver*, const void*, void**);
void wlShutdownVideo(void);
void wlUpdateVideo(void);
PalResult wlEnumerateMonitors(uint32_t*, PalMonitor**);
void wlGetPrimaryMonitor(PalMonitor**);
void wlGetMonitorInfo(PalMonitor*, PalMonitorInfo*);
void wlEnumerateMonitorModes(PalMonitor*, uint32_t*, PalMonitorMode*);
void wlGetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlSetMonitorMode(PalMonitor*, const PalMonitorMode*);
PalResult wlValidateMonitorMode(PalMonitor*, const PalMonitorMode*);
PalResult wlSetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult wlCreateWindow(const PalWindowCreateInfo*, PalWindow**);
void wlDestroyWindow(PalWindow*);
void wlMaximizeWindow(PalWindow*);
void wlMinimizeWindow(PalWindow*);
void wlRestoreWindow(PalWindow*);
void wlShowWindow(PalWindow*);
void wlHideWindow(PalWindow*);
void wlFlashWindow(PalWindow*, const PalFlashInfo*);
void wlGetWindowStyle(PalWindow*, PalWindowStyle*);
void wlGetWindowMonitor(PalWindow*, PalMonitor**);
void wlGetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
void wlGetWindowPos(PalWindow*, int32_t*, int32_t*);
void wlGetWindowSize(PalWindow*, uint32_t*, uint32_t*);
void wlGetWindowState(PalWindow*, PalWindowState*);
PalBool wlIsWindowVisible(PalWindow*);
PalWindow* wlGetFocusWindow();
void wlGetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
void wlSetWindowOpacity(PalWindow*, float);
void wlSetWindowStyle(PalWindow*, PalWindowStyle);
void wlSetWindowTitle(PalWindow*, const char*);
void wlSetWindowPos(PalWindow*, int32_t, int32_t);
void wlSetWindowSize(PalWindow*, uint32_t, uint32_t);
void wlSetFocusWindow(PalWindow*);

PalResult wlCreateIcon(const PalIconCreateInfo*, PalIcon**);
void wlDestroyIcon(PalIcon*);
void wlSetWindowIcon(PalWindow*, PalIcon*);
PalResult wlCreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult wlCreateCursorFrom(PalCursorType, PalCursor**);
void wlDestroyCursor(PalCursor*);
void wlShowCursor(PalBool);
void wlClipCursor(PalWindow*, PalBool);
void wlGetCursorPos(PalWindow*, int32_t*, int32_t*);
void wlSetCursorPos(PalWindow*, int32_t, int32_t);
void wlSetWindowCursor(PalWindow*, PalCursor*);

const VideoBackend g_WaylandBackend = {
    .getDriver = wlGetVideoDriver,
    .init = wlInitVideo,
    .shutdown = wlUpdateVideo,
    .update = wlUpdateVideo,
    .enumerateMonitors = wlEnumerateMonitors,
    .getPrimaryMonitor = wlGetPrimaryMonitor,
    .getMonitorInfo = wlGetMonitorInfo,
    .enumerateMonitorModes = wlEnumerateMonitorModes,
    .getCurrentMonitorMode = wlGetCurrentMonitorMode,
    .setMonitorMode = wlSetMonitorMode,
    .validateMonitorMode = wlValidateMonitorMode,
    .setMonitorOrientation = wlSetMonitorOrientation,

    .createWindow = wlCreateWindow,
    .destroyWindow = wlDestroyWindow,
    .maximizeWindow = wlMaximizeWindow,
    .minimizeWindow = wlMinimizeWindow,
    .restoreWindow = wlRestoreWindow,
    .showWindow = wlShowWindow,
    .hideWindow = wlHideWindow,
    .flashWindow = wlFlashWindow,
    .getWindowStyle = wlGetWindowStyle,
    .getWindowMonitor = wlGetWindowMonitor,
    .getWindowTitle = wlGetWindowTitle,
    .getWindowPos = wlGetWindowPos,
    .getWindowSize = wlGetWindowSize,
    .getWindowState = wlGetWindowState,
    .isWindowVisible = wlIsWindowVisible,
    .getFocusWindow = wlGetFocusWindow,
    .getWindowHandleInfo = wlGetWindowHandleInfo,
    .setWindowOpacity = wlSetWindowOpacity,
    .setWindowStyle = wlSetWindowStyle,
    .setWindowTitle = wlSetWindowTitle,
    .setWindowPos = wlSetWindowPos,
    .setWindowSize = wlSetWindowSize,
    .setFocusWindow = wlSetFocusWindow,

    .createIcon = wlCreateIcon,
    .destroyIcon = wlDestroyIcon,
    .setWindowIcon = wlSetWindowIcon,
    .createCursor = wlCreateCursor,
    .createCursorFrom = wlCreateCursorFrom,
    .destroyCursor = wlDestroyCursor,
    .showCursor = wlShowCursor,
    .clipCursor = wlClipCursor,
    .getCursorPos = wlGetCursorPos,
    .setCursorPos = wlSetCursorPos,
    .setWindowCursor = wlSetWindowCursor
};

#endif // PAL_HAS_WAYLAND_BACKEND