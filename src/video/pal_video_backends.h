/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_VIDEO_BACKENDS_H
#define _PAL_VIDEO_BACKENDS_H

#include "pal/pal_video.h"

// clang-format off
typedef struct {
    void (*shutdownVideo)();
    void (*updateVideo)();
    PalVideoFeatures (*getVideoFeatures)();
    PalResult (*enumerateMonitors)(uint32_t*, PalMonitor**);
    void (*getPrimaryMonitor)(PalMonitor**);
    void (*getMonitorInfo)(PalMonitor*, PalMonitorInfo*);
    void (*enumerateMonitorModes)(PalMonitor*, uint32_t*, PalMonitorMode*);
    void (*getCurrentMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*validateMonitorMode)(PalMonitor*, PalMonitorMode*);
    PalResult (*setMonitorOrientation)(PalMonitor*, PalOrientation);

    PalResult (*createWindow)(const PalWindowCreateInfo*, PalWindow**);
    void (*destroyWindow)(PalWindow*);
    void (*maximizeWindow)(PalWindow*);
    void (*minimizeWindow)(PalWindow*);
    void (*restoreWindow)(PalWindow*);
    void (*showWindow)(PalWindow*);
    void (*hideWindow)(PalWindow*);
    void (*flashWindow)(PalWindow*, const PalFlashInfo*);
    void (*getWindowStyle)(PalWindow*, PalWindowStyle*);
    void (*getWindowMonitor)(PalWindow*, PalMonitor**);
    void (*getWindowTitle)(PalWindow*, uint64_t, uint64_t*, char*);
    void (*getWindowPos)(PalWindow*, int32_t*, int32_t*);
    void (*getWindowSize)(PalWindow*, uint32_t*, uint32_t*);
    void (*getWindowState)(PalWindow*, PalWindowState*);
    const PalBool* (*getKeycodeState)();
    const PalBool* (*getScancodeState)();
    const PalBool* (*getMouseState)();
    void (*getMouseDelta)(float*, float*);
    void (*getMouseWheelDelta)(float*, float*);
    PalBool (*isWindowVisible)(PalWindow*);
    PalWindow* (*getFocusWindow)();
    void (*getWindowHandleInfo)(PalWindow*, PalWindowHandleInfo*);
    void (*setWindowOpacity)(PalWindow*, float);
    void (*setWindowStyle)(PalWindow*, PalWindowStyle);
    void (*setWindowTitle)(PalWindow*, const char*);
    void (*setWindowPos)(PalWindow*, int32_t, int32_t);
    void (*setWindowSize)(PalWindow*, uint32_t, uint32_t);
    void (*setFocusWindow)(PalWindow*);

    PalResult (*createIcon)(const PalIconCreateInfo*, PalIcon**);
    void (*destroyIcon)(PalIcon*);
    void (*setWindowIcon)(PalWindow*, PalIcon*);

    PalResult (*createCursor)(const PalCursorCreateInfo*, PalCursor**);
    PalResult (*createCursorFrom)(PalCursorType, PalCursor**);
    void (*destroyCursor)(PalCursor*);
    void (*showCursor)(PalBool);
    void (*clipCursor)(PalWindow*, PalBool);
    void (*getCursorPos)(PalWindow*, int32_t*, int32_t*);
    void (*setCursorPos)(PalWindow*, int32_t, int32_t);
    void (*setWindowCursor)(PalWindow*, PalCursor*);
    PalResult (*attachWindow)(void*, PalWindow**);
    PalResult (*detachWindow)(PalWindow*, void**);
    void* (*getInstance)();
} VideoBackend;

// ==================================================
// WIN32
// ==================================================

#ifdef _WIN32
PalResult win32InitVideo(const PalAllocator*, PalEventDriver*, void*);
void win32ShutdownVideo();
void win32UpdateVideo();
PalVideoFeatures win32GetVideoFeatures();

PalResult win32EnumerateMonitors(uint32_t*, PalMonitor**);
void win32GetPrimaryMonitor(PalMonitor**);
void win32GetMonitorInfo(PalMonitor*, PalMonitorInfo*);
void win32EnumerateMonitorModes(PalMonitor*, uint32_t*, PalMonitorMode*);
void win32GetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult win32SetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult win32ValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult win32SetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult win32CreateWindow(const PalWindowCreateInfo*, PalWindow**);
void win32DestroyWindow(PalWindow*);
void win32MaximizeWindow(PalWindow*);
void win32MinimizeWindow(PalWindow*);
void win32RestoreWindow(PalWindow*);
void win32ShowWindow(PalWindow*);
void win32HideWindow(PalWindow*);
void win32FlashWindow(PalWindow*, const PalFlashInfo*);
void win32GetWindowStyle(PalWindow*, PalWindowStyle*);
void win32GetWindowMonitor(PalWindow*, PalMonitor**);
void win32GetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
void win32GetWindowPos(PalWindow*, int32_t*, int32_t*);
void win32GetWindowSize(PalWindow*, uint32_t*, uint32_t*);
void win32GetWindowState(PalWindow*, PalWindowState*);
const PalBool* win32GetKeycodeState();
const PalBool* win32GetScancodeState();
const PalBool* win32GetMouseState();
void win32GetMouseDelta(float*, float*);
void win32GetMouseWheelDelta(float*, float*);
PalBool win32IsWindowVisible(PalWindow*);
PalWindow* win32GetFocusWindow();
void win32GetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
void win32SetWindowOpacity(PalWindow*, float);
void win32SetWindowStyle(PalWindow*, PalWindowStyle);
void win32SetWindowTitle(PalWindow*, const char*);
void win32SetWindowPos(PalWindow*, int32_t, int32_t);
void win32SetWindowSize(PalWindow*, uint32_t, uint32_t);
void win32SetFocusWindow(PalWindow*);

PalResult win32CreateIcon(const PalIconCreateInfo*, PalIcon**);
void win32DestroyIcon(PalIcon*);
void win32SetWindowIcon(PalWindow*, PalIcon*);

PalResult win32CreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult win32CreateCursorFrom(PalCursorType, PalCursor**);
void win32DestroyCursor(PalCursor*);
void win32ShowCursor(PalBool);
void win32ClipCursor(PalWindow*, PalBool);
void win32GetCursorPos(PalWindow*, int32_t*, int32_t*);
void win32SetCursorPos(PalWindow*, int32_t, int32_t);
void win32SetWindowCursor(PalWindow*, PalCursor*);
PalResult win32AttachWindow(void*, PalWindow**);
PalResult win32DetachWindow(PalWindow*, void**);
void* win32GetInstance();

static VideoBackend s_Win32Backend = {
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

PalResult xEnumerateMonitors(uint32_t*, PalMonitor**);
void xGetPrimaryMonitor(PalMonitor**);
void xGetMonitorInfo(PalMonitor*, PalMonitorInfo*);
void xEnumerateMonitorModes(PalMonitor*, uint32_t*, PalMonitorMode*);
void xGetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xSetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult xSetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult xCreateWindow(const PalWindowCreateInfo*, PalWindow**);
void xDestroyWindow(PalWindow*);
void xMaximizeWindow(PalWindow*);
void xMinimizeWindow(PalWindow*);
void xRestoreWindow(PalWindow*);
void xShowWindow(PalWindow*);
void xHideWindow(PalWindow*);
void xFlashWindow(PalWindow*, const PalFlashInfo*);
void xGetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
void xGetWindowPos(PalWindow*, int32_t*, int32_t*);
void xGetWindowSize(PalWindow*, uint32_t*, uint32_t*);
void xGetWindowState(PalWindow*, PalWindowState*);
const PalBool* xGetKeycodeState();
const PalBool* xGetScancodeState();
const PalBool* xGetMouseState();
void xGetMouseDelta(float*, float*);
void xGetMouseWheelDelta(float*, float*);
PalBool xIsWindowVisible(PalWindow*);
PalWindow* xGetFocusWindow();
void xGetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
void xSetWindowOpacity(PalWindow*, float);
void xSetWindowTitle(PalWindow*, const char*);
void xSetWindowPos(PalWindow*, int32_t, int32_t);
void xSetWindowSize(PalWindow*, uint32_t, uint32_t);
void xSetFocusWindow(PalWindow*);

PalResult xCreateIcon(const PalIconCreateInfo*, PalIcon**);
void xDestroyIcon(PalIcon*);
void xSetWindowIcon(PalWindow*, PalIcon*);

PalResult xCreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult xCreateCursorFrom(PalCursorType, PalCursor**);
void xDestroyCursor(PalCursor*);
void xShowCursor(PalBool);
void xClipCursor(PalWindow*, PalBool);
void xGetCursorPos(PalWindow*, int32_t*, int32_t*);
void xSetCursorPos(PalWindow*, int32_t, int32_t);
void xSetWindowCursor(PalWindow*, PalCursor*);
PalResult xAttachWindow(void*, PalWindow**);
PalResult xDetachWindow(PalWindow*, void**);
void* xGetInstance();

static VideoBackend s_XBackend = {
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
    .getWindowStyle = nullptr,
    .getWindowMonitor = nullptr,
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
    .setWindowStyle = nullptr,
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

PalResult wlEnumerateMonitors(uint32_t*, PalMonitor**);
void wlGetMonitorInfo(PalMonitor*, PalMonitorInfo*);
void wlEnumerateMonitorModes(PalMonitor*, uint32_t*, PalMonitorMode*);
void wlGetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlSetMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlValidateMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult wlSetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult wlCreateWindow(const PalWindowCreateInfo*, PalWindow**);
void wlDestroyWindow(PalWindow*);
void wlMaximizeWindow(PalWindow*);
void wlMinimizeWindow(PalWindow*);
void wlRestoreWindow(PalWindow*);
const PalBool* wlGetKeycodeState();
const PalBool* wlGetScancodeState();
const PalBool* wlGetMouseState();
void wlGetMouseDelta(float*, float*);
void wlGetMouseWheelDelta(float*, float*);
PalBool wlIsWindowVisible(PalWindow*);
void wlGetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
void wlSetWindowTitle(PalWindow*, const char*);
void wlSetWindowSize(PalWindow*, uint32_t, uint32_t);

PalResult wlCreateIcon(const PalIconCreateInfo*, PalIcon**);
void wlDestroyIcon(PalIcon*);

PalResult wlCreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult wlCreateCursorFrom(PalCursorType, PalCursor**);
void wlDestroyCursor(PalCursor*);
void wlSetWindowCursor(PalWindow*, PalCursor*);
void* wlGetInstance();

static VideoBackend s_wlBackend = {
    .shutdownVideo = wlShutdownVideo,
    .updateVideo = wlUpdateVideo,
    .getVideoFeatures = wlGetVideoFeatures,
    .enumerateMonitors = wlEnumerateMonitors,
    .getMonitorInfo = wlGetMonitorInfo,
    .getPrimaryMonitor = nullptr,
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
    .showWindow = nullptr,
    .hideWindow = nullptr,
    .flashWindow = nullptr,
    .getWindowStyle = nullptr,
    .getWindowMonitor = nullptr,
    .getWindowTitle = nullptr,
    .getWindowPos = nullptr,
    .getWindowSize = nullptr,
    .getWindowState = nullptr,
    .getKeycodeState = wlGetKeycodeState,
    .getScancodeState = wlGetScancodeState,
    .getMouseState = wlGetMouseState,
    .getMouseDelta = wlGetMouseDelta,
    .getMouseWheelDelta = wlGetMouseWheelDelta,
    .isWindowVisible = wlIsWindowVisible,
    .getFocusWindow = nullptr,
    .getWindowHandleInfo = wlGetWindowHandleInfo,
    .setWindowOpacity = nullptr,
    .setWindowStyle = nullptr,
    .setWindowTitle = wlSetWindowTitle,
    .setWindowPos = nullptr,
    .setWindowSize = wlSetWindowSize,
    .setFocusWindow = nullptr,

    .createIcon = wlCreateIcon,
    .destroyIcon = wlDestroyIcon,
    .setWindowIcon = nullptr,

    .createCursor = wlCreateCursor,
    .createCursorFrom = wlCreateCursorFrom,
    .destroyCursor = wlDestroyCursor,
    .showCursor = nullptr,
    .clipCursor = nullptr,
    .getCursorPos = nullptr,
    .setCursorPos = nullptr,
    .setWindowCursor = wlSetWindowCursor,
    .attachWindow = nullptr,
    .getInstance = wlGetInstance,
    .detachWindow = nullptr};

// clang-format on

#endif // PAL_HAS_WAYLAND_BACKEND

#endif // _PAL_VIDEO_BACKENDS_H