/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_BACKENDS_LINUX_H
#define _PAL_BACKENDS_LINUX_H

#include "pal/pal_video.h"

typedef struct {
    // clang-format off
    void (*shutdownVideo)();
    void (*updateVideo)();
    PalVideoFeatures (*getVideoFeatures)();
    PalResult (*enumerateMonitors)(int32_t*, PalMonitor**);
    PalResult (*getPrimaryMonitor)(PalMonitor**);
    PalResult (*getMonitorInfo)(PalMonitor*, PalMonitorInfo*);
    PalResult (*enumerateMonitorModes)(PalMonitor*, int32_t*, PalMonitorMode*);
    PalResult (*getCurrentMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*validateMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorOrientation)(PalMonitor*, PalOrientation);

    PalResult (*createWindow)(const PalWindowCreateInfo*, PalWindow**);
    void (*destroyWindow)(PalWindow*);
    PalResult (*maximizeWindow)(PalWindow*);
    PalResult (*minimizeWindow)(PalWindow*);
    PalResult (*restoreWindow)(PalWindow*);
    PalResult (*showWindow)(PalWindow*);
    PalResult (*hideWindow)(PalWindow*);
    PalResult (*flashWindow)(PalWindow*, const PalFlashInfo*);
    PalResult (*getWindowStyle)(PalWindow*, PalWindowStyle*);
    PalResult (*getWindowMonitor)(PalWindow*, PalMonitor**);
    PalResult (*getWindowTitle)(PalWindow*, uint64_t, uint64_t*, char*);
    PalResult (*getWindowPos)(PalWindow*, int32_t*, int32_t*);
    PalResult (*getWindowSize)(PalWindow*, uint32_t*, uint32_t*);
    PalResult (*getWindowState)(PalWindow*, PalWindowState*);
    const PalBool* (*getKeycodeState)();
    const PalBool* (*getScancodeState)();
    const PalBool* (*getMouseState)();
    void (*getMouseDelta)(float*, float*);
    void (*getMouseWheelDelta)(float*, float*);
    PalBool (*isWindowVisible)(PalWindow*);
    PalWindow* (*getFocusWindow)();
    PalResult (*getWindowHandleInfo)(PalWindow*, PalWindowHandleInfo*);
    PalResult (*setWindowOpacity)(PalWindow*, float);
    PalResult (*setWindowStyle)(PalWindow*, PalWindowStyle);
    PalResult (*setWindowTitle)(PalWindow*, const char*);
    PalResult (*setWindowPos)(PalWindow*, int32_t, int32_t);
    PalResult (*setWindowSize)(PalWindow*, uint32_t, uint32_t);
    PalResult (*setFocusWindow)(PalWindow*);

    PalResult (*createIcon)(const PalIconCreateInfo*, PalIcon**);
    void (*destroyIcon)(PalIcon*);
    PalResult (*setWindowIcon)(PalWindow*, PalIcon*);

    PalResult (*createCursor)(const PalCursorCreateInfo*, PalCursor**);
    PalResult (*createCursorFrom)(PalCursorType, PalCursor**);
    void (*destroyCursor)(PalCursor*);
    void (*showCursor)(PalBool);
    PalResult (*clipCursor)(PalWindow*, PalBool);
    PalResult (*getCursorPos)(PalWindow*, int32_t*, int32_t*);
    PalResult (*setCursorPos)(PalWindow*, int32_t, int32_t);
    PalResult (*setWindowCursor)(PalWindow*, PalCursor*);
    PalResult (*attachWindow)(void*, PalWindow**);
    PalResult (*detachWindow)(PalWindow*, void**);
    void* (*getInstance)();
    // clang-format on
} Backend;

// ==================================================
// WIN32
// ==================================================

#ifdef _WIN32
PalResult win32InitVideo(const PalAllocator*, PalEventDriver*, void*);
void win32ShutdownVideo();
void win32UpdateVideo();
PalVideoFeatures win32GetVideoFeatures();

PalResult win32EnumerateMonitors(int32_t*, PalMonitor**);
PalResult win32GetPrimaryMonitor(PalMonitor**);
PalResult win32GetMonitorInfo(PalMonitor*, PalMonitorInfo*);
PalResult win32EnumerateMonitorModes(PalMonitor*, int32_t*, PalMonitorMode*);
PalResult win32GetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult win32SetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult win32ValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult win32SetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult win32CreateWindow(const PalWindowCreateInfo*, PalWindow**);
void win32DestroyWindow(PalWindow*);
PalResult win32MaximizeWindow(PalWindow*);
PalResult win32MinimizeWindow(PalWindow*);
PalResult win32RestoreWindow(PalWindow*);
PalResult win32ShowWindow(PalWindow*);
PalResult win32HideWindow(PalWindow*);
PalResult win32FlashWindow(PalWindow*, const PalFlashInfo*);
PalResult win32GetWindowStyle(PalWindow*, PalWindowStyle*);
PalResult win32GetWindowMonitor(PalWindow*, PalMonitor**);
PalResult win32GetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
PalResult win32GetWindowPos(PalWindow*, int32_t*, int32_t*);
PalResult win32GetWindowSize(PalWindow*, uint32_t*, uint32_t*);
PalResult win32GetWindowState(PalWindow*, PalWindowState*);
const PalBool* win32GetKeycodeState();
const PalBool* win32GetScancodeState();
const PalBool* win32GetMouseState();
void win32GetMouseDelta(float*, float*);
void win32GetMouseWheelDelta(float*, float*);
PalBool win32IsWindowVisible(PalWindow*);
PalWindow* win32GetFocusWindow();
PalResult win32GetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
PalResult win32SetWindowOpacity(PalWindow*, float);
PalResult win32SetWindowStyle(PalWindow*, PalWindowStyle);
PalResult win32SetWindowTitle(PalWindow*, const char*);
PalResult win32SetWindowPos(PalWindow*, int32_t, int32_t);
PalResult win32SetWindowSize(PalWindow*, uint32_t, uint32_t);
PalResult win32SetFocusWindow(PalWindow*);

PalResult win32CreateIcon(const PalIconCreateInfo*, PalIcon**);
void win32DestroyIcon(PalIcon*);
PalResult win32SetWindowIcon(PalWindow*, PalIcon*);

PalResult win32CreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult win32CreateCursorFrom(PalCursorType, PalCursor**);
void win32DestroyCursor(PalCursor*);
void win32ShowCursor(PalBool);
PalResult win32ClipCursor(PalWindow*, PalBool);
PalResult win32GetCursorPos(PalWindow*, int32_t*, int32_t*);
PalResult win32SetCursorPos(PalWindow*, int32_t, int32_t);
PalResult win32SetWindowCursor(PalWindow*, PalCursor*);
PalResult win32AttachWindow(void*, PalWindow**);
PalResult win32DetachWindow(PalWindow*, void**);
void* win32GetInstance();

static Backend s_Win32Backend = {
    .shutdownVideo = win32ShutdownVideo,
    .updateVideo = win32UpdateVideo,
    .getVideoFeatures = win32GetVideoFeatures,
    .enumerateMonitors = win32EnumerateMonitors,
    .getMonitorInfo = win32GetMonitorInfo,
    .getPrimaryMonitor = win32GetPrimaryMonitor,
    .enumerateMonitorModes = win32EnumerateMonitorModes,
    .getCurrentMonitorMode = win32GetCurrentMonitorMode,
    .setMonitorMode = win32SetMonitorMode,
    .validateMonitorMode = win32ValidateMonitorMode,
    .setMonitorOrientation = win32SetMonitorOrientation,

    .createWindow = win32CreateWindow,
    .destroyWindow = win32DestroyWindow,
    .maximizeWindow = win32MaximizeWindow,
    .minimizeWindow = win32MinimizeWindow,
    .restoreWindow = win32RestoreWindow,
    .showWindow = win32ShowWindow,
    .hideWindow = win32HideWindow,
    .flashWindow = win32FlashWindow,
    .getWindowStyle = win32GetWindowStyle,
    .getWindowMonitor = win32GetWindowMonitor,
    .getWindowTitle = win32GetWindowTitle,
    .getWindowPos = win32GetWindowPos,
    .getWindowSize = win32GetWindowSize,
    .getWindowState = win32GetWindowState,
    .getKeycodeState = win32GetKeycodeState,
    .getScancodeState = win32GetScancodeState,
    .getMouseState = win32GetMouseState,
    .getMouseDelta = win32GetMouseDelta,
    .getMouseWheelDelta = win32GetMouseWheelDelta,
    .isWindowVisible = win32IsWindowVisible,
    .getFocusWindow = win32GetFocusWindow,
    .getWindowHandleInfo = win32GetWindowHandleInfo,
    .setWindowOpacity = win32SetWindowOpacity,
    .setWindowStyle = win32SetWindowStyle,
    .setWindowTitle = win32SetWindowTitle,
    .setWindowPos = win32SetWindowPos,
    .setWindowSize = win32SetWindowSize,
    .setFocusWindow = win32SetFocusWindow,

    .createIcon = win32CreateIcon,
    .destroyIcon = win32DestroyIcon,
    .setWindowIcon = win32SetWindowIcon,

    .createCursor = win32CreateCursor,
    .createCursorFrom = win32CreateCursorFrom,
    .destroyCursor = win32DestroyCursor,
    .showCursor = win32ShowCursor,
    .clipCursor = win32ClipCursor,
    .getCursorPos = win32GetCursorPos,
    .setCursorPos = win32SetCursorPos,
    .setWindowCursor = win32SetWindowCursor,
    .attachWindow = win32AttachWindow,
    .getInstance = win32GetInstance,
    .detachWindow = win32DetachWindow};

#endif // _WIN32

// ==================================================
// X11
// ==================================================

#if PAL_HAS_X11_BACKEND == 1
PalResult xInitVideo(const PalAllocator*, PalEventDriver*, void*);
void xShutdownVideo();
void xUpdateVideo();
PalVideoFeatures xGetVideoFeatures();

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
const PalBool* xGetKeycodeState();
const PalBool* xGetScancodeState();
const PalBool* xGetMouseState();
void xGetMouseDelta(float*, float*);
void xGetMouseWheelDelta(float*, float*);
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
void* xGetInstance();

static Backend s_XBackend = {
    .shutdownVideo = xShutdownVideo,
    .updateVideo = xUpdateVideo,
    .getVideoFeatures = xGetVideoFeatures,
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
    .getKeycodeState = xGetKeycodeState,
    .getScancodeState = xGetScancodeState,
    .getMouseState = xGetMouseState,
    .getMouseDelta = xGetMouseDelta,
    .getMouseWheelDelta = xGetMouseWheelDelta,
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
    .getInstance = xGetInstance,
    .detachWindow = xDetachWindow};

#endif // PAL_HAS_X11_BACKEND

// ==================================================
// Wayland
// ==================================================

#if PAL_HAS_WAYLAND_BACKEND == 1
PalResult wlInitVideo(const PalAllocator*, PalEventDriver*, void*);
void wlShutdownVideo();
void wlUpdateVideo();
void wlUpdateVideo();
PalVideoFeatures wlGetVideoFeatures();

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
const PalBool* wlGetKeycodeState();
const PalBool* wlGetScancodeState();
const PalBool* wlGetMouseState();
void wlGetMouseDelta(float*, float*);
void wlGetMouseWheelDelta(float*, float*);
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
void* wlGetInstance();

static Backend s_wlBackend = {
    .shutdownVideo = wlShutdownVideo,
    .updateVideo = wlUpdateVideo,
    .getVideoFeatures = wlGetVideoFeatures,
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
    .getKeycodeState = wlGetKeycodeState,
    .getScancodeState = wlGetScancodeState,
    .getMouseState = wlGetMouseState,
    .getMouseDelta = wlGetMouseDelta,
    .getMouseWheelDelta = wlGetMouseWheelDelta,
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
    .getInstance = wlGetInstance,
    .detachWindow = wlDetachWindow};

#endif // PAL_HAS_WAYLAND_BACKEND

#endif // _PAL_BACKENDS_LINUX_H