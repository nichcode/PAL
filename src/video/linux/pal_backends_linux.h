/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_BACKENDS_LINUX_H
#define _PAL_BACKENDS_LINUX_H
#ifdef __linux__

#include "pal_video_linux.h"

// ==================================================
// X11
// ==================================================

PalResult xInitVideo();
void xShutdownVideo();
void xUpdateVideo();

PalResult xEnumerateMonitors(int32_t*, PalMonitor**);
PalResult xGetPrimaryMonitor(PalMonitor**);
PalResult xGetMonitorInfo(PalMonitor*, PalMonitorInfo*);
PalResult xEnumerateMonitorModes(PalMonitor*, int32_t*, PalMonitorMode*);
PalResult xGetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xSetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xSetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult xCreateWindow(const PalWindowCreateInfo*, PalWindow**);
void xDestroyWindow(PalWindow*);
PalResult xMaximizeWindow(PalWindow*);
PalResult xMinimizeWindow(PalWindow*);
PalResult xRestoreWindow(PalWindow*);
PalResult xShowWindow(PalWindow*);
PalResult xHideWindow(PalWindow*);
PalResult xFlashWindow(PalWindow*, const PalFlashInfo*);
PalResult xGetWindowStyle(PalWindow*, PalWindowStyle*);
PalResult xGetWindowMonitor(PalWindow*, PalMonitor**);
PalResult xGetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
PalResult xGetWindowPos(PalWindow*, int32_t*, int32_t*);
PalResult xGetWindowSize(PalWindow*, uint32_t*, uint32_t*);
PalResult xGetWindowState(PalWindow*, PalWindowState*);
PalBool xIsWindowVisible(PalWindow*);
PalWindow* xGetFocusWindow();
PalResult xGetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
PalResult xSetWindowOpacity(PalWindow*, float);
PalResult xSetWindowStyle(PalWindow*, PalWindowStyle);
PalResult xSetWindowTitle(PalWindow*, const char*);
PalResult xSetWindowPos(PalWindow*, int32_t, int32_t);
PalResult xSetWindowSize(PalWindow*, uint32_t, uint32_t);
PalResult xSetFocusWindow(PalWindow*);

PalResult xCreateIcon(const PalIconCreateInfo*, PalIcon**);
void xDestroyIcon(PalIcon*);
PalResult xSetWindowIcon(PalWindow*, PalIcon*);

PalResult xCreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult xCreateCursorFrom(PalCursorType, PalCursor**);
void xDestroyCursor(PalCursor*);
void xShowCursor(PalBool);
PalResult xClipCursor(PalWindow*, PalBool);
PalResult xGetCursorPos(PalWindow*, int32_t*, int32_t*);
PalResult xSetCursorPos(PalWindow*, int32_t, int32_t);
PalResult xSetWindowCursor(PalWindow*, PalCursor*);

PalResult xAttachWindow(void*, PalWindow**);
PalResult xDetachWindow(PalWindow*, void**);

static Backend s_XBackend = {
    .shutdownVideo = xShutdownVideo,
    .updateVideo = xUpdateVideo,
    .enumerateMonitors = xEnumerateMonitors,
    .getMonitorInfo = xGetMonitorInfo,
    .getPrimaryMonitor = xGetPrimaryMonitor,
    .enumerateMonitorModes = xEnumerateMonitorModes,
    .getCurrentMonitorMode = xGetCurrentMonitorMode,
    .setMonitorMode = xSetMonitorMode,
    .validateMonitorMode = xValidateMonitorMode,
    .setMonitorOrientation = xSetMonitorOrientation,

    .createWindow = xCreateWindow,
    .destroyWindow = xDestroyWindow,
    .maximizeWindow = xMaximizeWindow,
    .minimizeWindow = xMinimizeWindow,
    .restoreWindow = xRestoreWindow,
    .showWindow = xShowWindow,
    .hideWindow = xHideWindow,
    .flashWindow = xFlashWindow,
    .getWindowStyle = xGetWindowStyle,
    .getWindowMonitor = xGetWindowMonitor,
    .getWindowTitle = xGetWindowTitle,
    .getWindowPos = xGetWindowPos,
    .getWindowSize = xGetWindowSize,
    .getWindowState = xGetWindowState,
    .isWindowVisible = xIsWindowVisible,
    .getFocusWindow = xGetFocusWindow,
    .getWindowHandleInfo = xGetWindowHandleInfo,
    .setWindowOpacity = xSetWindowOpacity,
    .setWindowStyle = xSetWindowStyle,
    .setWindowTitle = xSetWindowTitle,
    .setWindowPos = xSetWindowPos,
    .setWindowSize = xSetWindowSize,
    .setFocusWindow = xSetFocusWindow,

    .createIcon = xCreateIcon,
    .destroyIcon = xDestroyIcon,
    .setWindowIcon = xSetWindowIcon,

    .createCursor = xCreateCursor,
    .createCursorFrom = xCreateCursorFrom,
    .destroyCursor = xDestroyCursor,
    .showCursor = xShowCursor,
    .clipCursor = xClipCursor,
    .getCursorPos = xGetCursorPos,
    .setCursorPos = xSetCursorPos,
    .setWindowCursor = xSetWindowCursor,

    .attachWindow = xAttachWindow,
    .detachWindow = xDetachWindow};

// ==================================================
// Wayland
// ==================================================

PalResult wlInitVideo();
void wlShutdownVideo();
void wlUpdateVideo();

PalResult wlEnumerateMonitors(int32_t*, PalMonitor**);
PalResult wlGetPrimaryMonitor(PalMonitor**);
PalResult wlGetMonitorInfo(PalMonitor*, PalMonitorInfo*);
PalResult wlEnumerateMonitorModes(PalMonitor*, int32_t*, PalMonitorMode*);
PalResult wlGetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlSetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlSetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult wlCreateWindow(const PalWindowCreateInfo*, PalWindow**);
void wlDestroyWindow(PalWindow*);
PalResult wlMaximizeWindow(PalWindow*);
PalResult wlMinimizeWindow(PalWindow*);
PalResult wlRestoreWindow(PalWindow*);
PalResult wlShowWindow(PalWindow*);
PalResult wlHideWindow(PalWindow*);
PalResult wlFlashWindow(PalWindow*, const PalFlashInfo*);
PalResult wlGetWindowStyle(PalWindow*, PalWindowStyle*);
PalResult wlGetWindowMonitor(PalWindow*, PalMonitor**);
PalResult wlGetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
PalResult wlGetWindowPos(PalWindow*, int32_t*, int32_t*);
PalResult wlGetWindowSize(PalWindow*, uint32_t*, uint32_t*);
PalResult wlGetWindowState(PalWindow*, PalWindowState*);
PalBool wlIsWindowVisible(PalWindow*);
PalWindow* wlGetFocusWindow();
PalResult wlGetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
PalResult wlSetWindowOpacity(PalWindow*, float);
PalResult wlSetWindowStyle(PalWindow*, PalWindowStyle);
PalResult wlSetWindowTitle(PalWindow*, const char*);
PalResult wlSetWindowPos(PalWindow*, int32_t, int32_t);
PalResult wlSetWindowSize(PalWindow*, uint32_t, uint32_t);
PalResult wlSetFocusWindow(PalWindow*);

PalResult wlCreateIcon(const PalIconCreateInfo*, PalIcon**);
void wlDestroyIcon(PalIcon*);
PalResult wlSetWindowIcon(PalWindow*, PalIcon*);

PalResult wlCreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult wlCreateCursorFrom(PalCursorType, PalCursor**);
void wlDestroyCursor(PalCursor*);
void wlShowCursor(PalBool);
PalResult wlClipCursor(PalWindow*, PalBool);
PalResult wlGetCursorPos(PalWindow*, int32_t*, int32_t*);
PalResult wlSetCursorPos(PalWindow*, int32_t, int32_t);
PalResult wlSetWindowCursor(PalWindow*, PalCursor*);

PalResult wlAttachWindow(void*, PalWindow**);
PalResult wlDetachWindow(PalWindow*, void**);

static Backend s_wlBackend = {
    .shutdownVideo = wlShutdownVideo,
    .updateVideo = wlUpdateVideo,
    .enumerateMonitors = wlEnumerateMonitors,
    .getMonitorInfo = wlGetMonitorInfo,
    .getPrimaryMonitor = wlGetPrimaryMonitor,
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
    .setWindowCursor = wlSetWindowCursor,

    .attachWindow = wlAttachWindow,
    .detachWindow = wlDetachWindow};

#endif // __linux__
#endif // _PAL_BACKENDS_LINUX_H