/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifndef _PAL_VIDEO_BACKENDS_H
#define _PAL_VIDEO_BACKENDS_H

#include "pal2/pal_video.h"

// clang-format off
typedef struct {
    void (*shutdownVideo)();

    void (*updateVideo)();
    
    PalVideoFeatures (*getVideoFeatures)();
    
    PalResult (*enumerateMonitors)(
        uint32_t* count, 
        PalMonitor** outMonitor);

    void (*getPrimaryMonitor)(PalMonitor** outMonitor);

    void (*getMonitorInfo)(
        PalMonitor* monitor, 
        PalMonitorInfo* info);

    void (*enumerateMonitorModes)(
        PalMonitor* monitor, 
        uint32_t* count, 
        PalMonitorMode* mode);

    void (*getCurrentMonitorMode)(
        PalMonitor* monitor, 
        PalMonitorMode* mode);

    PalResult (*setMonitorMode)(
        PalMonitor* monitor, 
        PalMonitorMode* mode);

    PalResult (*validateMonitorMode)(
        PalMonitor* monitor, 
        PalMonitorMode* mode);

    PalResult (*setMonitorOrientation)(
        PalMonitor* monitor, 
        PalOrientation orientation);

    PalResult (*createWindow)(
        const PalWindowCreateInfo* info, 
        PalWindow** outWindow);

    void (*destroyWindow)(PalWindow* window);

    void (*maximizeWindow)(PalWindow* window);

    void (*minimizeWindow)(PalWindow* window);

    void (*restoreWindow)(PalWindow* window);

    void (*showWindow)(PalWindow* window);

    void (*hideWindow)(PalWindow* window);

    void (*flashWindow)(
        PalWindow* window, 
        const PalFlashInfo* info);
        
    void (*getWindowStyle)(
        PalWindow* window, 
        PalWindowStyle* style);

    void (*getWindowMonitor)(
        PalWindow* window, 
        PalMonitor** outMonitor);

    void (*getWindowTitle)(
        PalWindow* window, 
        uint64_t bufferSize, 
        uint64_t* outSize, 
        char* outBuffer);

    void (*getWindowPos)(
        PalWindow* window, 
        int32_t* x, 
        int32_t* y);

    void (*getWindowSize)(
        PalWindow* window, 
        uint32_t* width, 
        uint32_t* height);

    void (*getWindowState)(
        PalWindow* window, 
        PalWindowState* state);

    const PalBool* (*getKeycodeState)();

    const PalBool* (*getScancodeState)();

    const PalBool* (*getMouseState)();

    void (*getMouseDelta)(
        float* dx, 
        float* dy);

    void (*getMouseWheelDelta)(
        float* dx, 
        float* dy);

    PalBool (*isWindowVisible)(PalWindow* window);

    PalWindow* (*getFocusWindow)();

    void (*getWindowHandleInfo)(
        PalWindow* window, 
        PalWindowHandleInfo* info);

    void (*setWindowOpacity)(
        PalWindow* window, 
        float opacity);

    void (*setWindowStyle)(
        PalWindow* window, 
        PalWindowStyle style);

    void (*setWindowTitle)(
        PalWindow* window, 
        const char* title);

    void (*setWindowPos)(
        PalWindow* window, 
        int32_t x, 
        int32_t y);

    void (*setWindowSize)(
        PalWindow* window, 
        uint32_t width, 
        uint32_t height);

    void (*setFocusWindow)(PalWindow* window);

    PalResult (*createIcon)(
        const PalIconCreateInfo* info, 
        PalIcon** outIcon);

    void (*destroyIcon)(PalIcon* icon);
    
    void (*setWindowIcon)(
        PalWindow* window, 
        PalIcon* icon);

    PalResult (*createCursor)(
        const PalCursorCreateInfo* info, 
        PalCursor** outCursor);

    PalResult (*createCursorFrom)(
        PalCursorType type, 
        PalCursor** outCursor);

    void (*destroyCursor)(PalCursor* cursor);

    void (*showCursor)(PalBool show);
    
    void (*clipCursor)(
        PalWindow* window, 
        PalBool clip);

    void (*getCursorPos)(
        PalWindow* window, 
        int32_t* x, 
        int32_t* y);

    void (*setCursorPos)(
        PalWindow* window, 
        int32_t x, 
        int32_t y);

    void (*setWindowCursor)(
        PalWindow* window, 
        PalCursor* cursor);

    PalResult (*attachWindow)(
        void* windowHandle, 
        PalWindow** outWindow);

    PalResult (*detachWindow)(
        PalWindow* window, 
        void** outWindowHandle);

    void* (*getInstance)();
} VideoBackend;

// ==================================================
// WIN32
// ==================================================

#ifdef _WIN32
PalResult win32InitVideo(
    const PalAllocator* allocator, 
    PalEventDriver* eventDriver, 
    void* preferredInstance);

void win32ShutdownVideo();

void win32UpdateVideo();

PalVideoFeatures win32GetVideoFeatures();

PalResult win32EnumerateMonitors(
    uint32_t* count, 
    PalMonitor** outMonitor);

void win32GetPrimaryMonitor(PalMonitor** outMonitor);

void win32GetMonitorInfo(
    PalMonitor* monitor, 
    PalMonitorInfo* info);

void win32EnumerateMonitorModes(
    PalMonitor* monitor, 
    uint32_t* count, 
    PalMonitorMode* mode);

void win32GetCurrentMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult win32SetMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult win32ValidateMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult win32SetMonitorOrientation(
    PalMonitor* monitor, 
    PalOrientation orientation);

PalResult win32CreateWindow(
    const PalWindowCreateInfo* info, 
    PalWindow** outWindow);

void win32DestroyWindow(PalWindow* window);

void win32MaximizeWindow(PalWindow* window);

void win32MinimizeWindow(PalWindow* window);

void win32RestoreWindow(PalWindow* window);

void win32ShowWindow(PalWindow* window);

void win32HideWindow(PalWindow* window);

void win32FlashWindow(
    PalWindow* window, 
    const PalFlashInfo* info);
    
void win32GetWindowStyle(
    PalWindow* window, 
    PalWindowStyle* style);

void win32GetWindowMonitor(
    PalWindow* window, 
    PalMonitor** outMonitor);

void win32GetWindowTitle(
    PalWindow* window, 
    uint64_t bufferSize, 
    uint64_t* outSize, 
    char* outBuffer);

void win32GetWindowPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y);

void win32GetWindowSize(
    PalWindow* window, 
    uint32_t* width, 
    uint32_t* height);

void win32GetWindowState(
    PalWindow* window, 
    PalWindowState* state);

const PalBool* win32GetKeycodeState();

const PalBool* win32GetScancodeState();

const PalBool* win32GetMouseState();

void win32GetMouseDelta(
    float* dx, 
    float* dy);

void win32GetMouseWheelDelta(
    float* dx, 
    float* dy);

PalBool win32IsWindowVisible(PalWindow* window);

PalWindow* win32GetFocusWindow();

void win32GetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info);

void win32SetWindowOpacity(
    PalWindow* window, 
    float opacity);

void win32SetWindowStyle(
    PalWindow* window, 
    PalWindowStyle style);

void win32SetWindowTitle(
    PalWindow* window, 
    const char* title);

void win32SetWindowPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y);

void win32SetWindowSize(
    PalWindow* window, 
    uint32_t width, 
    uint32_t height);

void win32SetFocusWindow(PalWindow* window);

PalResult win32CreateIcon(
    const PalIconCreateInfo* info, 
    PalIcon** outIcon);

void win32DestroyIcon(PalIcon* icon);

void win32SetWindowIcon(
    PalWindow* window, 
    PalIcon* icon);

PalResult win32CreateCursor(
    const PalCursorCreateInfo* info, 
    PalCursor** outCursor);

PalResult win32CreateCursorFrom(
    PalCursorType type, 
    PalCursor** outCursor);

void win32DestroyCursor(PalCursor* cursor);

void win32ShowCursor(PalBool show);

void win32ClipCursor(
    PalWindow* window, 
    PalBool clip);

void win32GetCursorPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y);

void win32SetCursorPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y);

void win32SetWindowCursor(
    PalWindow* window, 
    PalCursor* cursor);

PalResult win32AttachWindow(
    void* windowHandle, 
    PalWindow** outWindow);

PalResult win32DetachWindow(
    PalWindow* window, 
    void** outWindowHandle);
    
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
    .detachWindow = win32DetachWindow
};

#endif // _WIN32

// ==================================================
// X11
// ==================================================

#if PAL_HAS_X11_BACKEND == 1
PalResult xInitVideo(
    const PalAllocator* allocator, 
    PalEventDriver* eventDriver, 
    void* preferredInstance);

void xShutdownVideo();

void xUpdateVideo();

PalVideoFeatures xGetVideoFeatures();

PalResult xEnumerateMonitors(
    uint32_t* count, 
    PalMonitor** outMonitor);

void xGetPrimaryMonitor(PalMonitor** outMonitor);

void xGetMonitorInfo(
    PalMonitor* monitor, 
    PalMonitorInfo* info);

void xEnumerateMonitorModes(
    PalMonitor* monitor, 
    uint32_t* count, 
    PalMonitorMode* mode);

void xGetCurrentMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult xSetMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult xValidateMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult xSetMonitorOrientation(
    PalMonitor* monitor, 
    PalOrientation orientation);

PalResult xCreateWindow(
    const PalWindowCreateInfo* info, 
    PalWindow** outWindow);

void xDestroyWindow(PalWindow* window);

void xMaximizeWindow(PalWindow* window);

void xMinimizeWindow(PalWindow* window);

void xRestoreWindow(PalWindow* window);

void xShowWindow(PalWindow* window);

void xHideWindow(PalWindow* window);

void xFlashWindow(
    PalWindow* window, 
    const PalFlashInfo* info);
    
void xGetWindowStyle(
    PalWindow* window, 
    PalWindowStyle* style);

void xGetWindowMonitor(
    PalWindow* window, 
    PalMonitor** outMonitor);

void xGetWindowTitle(
    PalWindow* window, 
    uint64_t bufferSize, 
    uint64_t* outSize, 
    char* outBuffer);

void xGetWindowPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y);

void xGetWindowSize(
    PalWindow* window, 
    uint32_t* width, 
    uint32_t* height);

void xGetWindowState(
    PalWindow* window, 
    PalWindowState* state);

const PalBool* xGetKeycodeState();

const PalBool* xGetScancodeState();

const PalBool* xGetMouseState();

void xGetMouseDelta(
    float* dx, 
    float* dy);

void xGetMouseWheelDelta(
    float* dx, 
    float* dy);

PalBool xIsWindowVisible(PalWindow* window);

PalWindow* xGetFocusWindow();

void xGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info);

void xSetWindowOpacity(
    PalWindow* window, 
    float opacity);

void xSetWindowStyle(
    PalWindow* window, 
    PalWindowStyle style);

void xSetWindowTitle(
    PalWindow* window, 
    const char* title);

void xSetWindowPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y);

void xSetWindowSize(
    PalWindow* window, 
    uint32_t width, 
    uint32_t height);

void xSetFocusWindow(PalWindow* window);

PalResult xCreateIcon(
    const PalIconCreateInfo* info, 
    PalIcon** outIcon);

void xDestroyIcon(PalIcon* icon);

void xSetWindowIcon(
    PalWindow* window, 
    PalIcon* icon);

PalResult xCreateCursor(
    const PalCursorCreateInfo* info, 
    PalCursor** outCursor);

PalResult xCreateCursorFrom(
    PalCursorType type, 
    PalCursor** outCursor);

void xDestroyCursor(PalCursor* cursor);

void xShowCursor(PalBool show);

void xClipCursor(
    PalWindow* window, 
    PalBool clip);

void xGetCursorPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y);

void xSetCursorPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y);

void xSetWindowCursor(
    PalWindow* window, 
    PalCursor* cursor);

PalResult xAttachWindow(
    void* windowHandle, 
    PalWindow** outWindow);

PalResult xDetachWindow(
    PalWindow* window, 
    void** outWindowHandle);
    
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
    .detachWindow = xDetachWindow
};

#endif // PAL_HAS_X11_BACKEND

// ==================================================
// Wayland
// ==================================================

#if PAL_HAS_WAYLAND_BACKEND == 1
PalResult wlInitVideo(
    const PalAllocator* allocator, 
    PalEventDriver* eventDriver, 
    void* preferredInstance);

void wlShutdownVideo();

void wlUpdateVideo();

PalVideoFeatures wlGetVideoFeatures();

PalResult wlEnumerateMonitors(
    uint32_t* count, 
    PalMonitor** outMonitor);

void wlGetPrimaryMonitor(PalMonitor** outMonitor);

void wlGetMonitorInfo(
    PalMonitor* monitor, 
    PalMonitorInfo* info);

void wlEnumerateMonitorModes(
    PalMonitor* monitor, 
    uint32_t* count, 
    PalMonitorMode* mode);

void wlGetCurrentMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult wlSetMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult wlValidateMonitorMode(
    PalMonitor* monitor, 
    PalMonitorMode* mode);

PalResult wlSetMonitorOrientation(
    PalMonitor* monitor, 
    PalOrientation orientation);

PalResult wlCreateWindow(
    const PalWindowCreateInfo* info, 
    PalWindow** outWindow);

void wlDestroyWindow(PalWindow* window);

void wlMaximizeWindow(PalWindow* window);

void wlMinimizeWindow(PalWindow* window);

void wlRestoreWindow(PalWindow* window);

void wlShowWindow(PalWindow* window);

void wlHideWindow(PalWindow* window);

void wlFlashWindow(
    PalWindow* window, 
    const PalFlashInfo* info);
    
void wlGetWindowStyle(
    PalWindow* window, 
    PalWindowStyle* style);

void wlGetWindowMonitor(
    PalWindow* window, 
    PalMonitor** outMonitor);

void wlGetWindowTitle(
    PalWindow* window, 
    uint64_t bufferSize, 
    uint64_t* outSize, 
    char* outBuffer);

void wlGetWindowPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y);

void wlGetWindowSize(
    PalWindow* window, 
    uint32_t* width, 
    uint32_t* height);

void wlGetWindowState(
    PalWindow* window, 
    PalWindowState* state);

const PalBool* wlGetKeycodeState();

const PalBool* wlGetScancodeState();

const PalBool* wlGetMouseState();

void wlGetMouseDelta(
    float* dx, 
    float* dy);

void wlGetMouseWheelDelta(
    float* dx, 
    float* dy);

PalBool wlIsWindowVisible(PalWindow* window);

PalWindow* wlGetFocusWindow();

void wlGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info);

void wlSetWindowOpacity(
    PalWindow* window, 
    float opacity);

void wlSetWindowStyle(
    PalWindow* window, 
    PalWindowStyle style);

void wlSetWindowTitle(
    PalWindow* window, 
    const char* title);

void wlSetWindowPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y);

void wlSetWindowSize(
    PalWindow* window, 
    uint32_t width, 
    uint32_t height);

void wlSetFocusWindow(PalWindow* window);

PalResult wlCreateIcon(
    const PalIconCreateInfo* info, 
    PalIcon** outIcon);

void wlDestroyIcon(PalIcon* icon);

void wlSetWindowIcon(
    PalWindow* window, 
    PalIcon* icon);

PalResult wlCreateCursor(
    const PalCursorCreateInfo* info, 
    PalCursor** outCursor);

PalResult wlCreateCursorFrom(
    PalCursorType type, 
    PalCursor** outCursor);

void wlDestroyCursor(PalCursor* cursor);

void wlShowCursor(PalBool show);

void wlClipCursor(
    PalWindow* window, 
    PalBool clip);

void wlGetCursorPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y);

void wlSetCursorPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y);

void wlSetWindowCursor(
    PalWindow* window, 
    PalCursor* cursor);

PalResult wlAttachWindow(
    void* windowHandle, 
    PalWindow** outWindow);

PalResult wlDetachWindow(
    PalWindow* window, 
    void** outWindowHandle);
    
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
    .detachWindow = nullptr
};

// clang-format on

#endif // PAL_HAS_WAYLAND_BACKEND

#endif // _PAL_VIDEO_BACKENDS_H