
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_video_backends.h"
#include "pal_video_egl.h"
#include <stdlib.h>

typedef struct {
    PalBool initialized;
    const Backend* backend;
} Video;

EGL s_Egl = {0};
static Video s_Video = {0};

static int compareModes(
    const void* a,
    const void* b)
{
    const PalMonitorMode* mode1 = (const PalMonitorMode*)a;
    const PalMonitorMode* mode2 = (const PalMonitorMode*)b;

    // compare width
    if (mode1->width > mode2->width) {
        return -1;
    }

    if (mode1->width < mode2->width) {
        return 1;
    }

    // compare height
    if (mode1->height > mode2->height) {
        return -1;
    }

    if (mode1->height < mode2->height) {
        return 1;
    }

    // compare refresh rate
    if (mode1->refreshRate > mode2->refreshRate) {
        return -1;
    }

    if (mode1->refreshRate < mode2->refreshRate) {
        return 1;
    }

    // compare bpp
    if (mode1->bpp > mode2->bpp) {
        return -1;
    }

    if (mode1->bpp < mode2->bpp) {
        return 1;
    }

    return 0;
}

PalResult PAL_CALL palInitVideo(
    const PalAllocator* allocator,
    PalEventDriver* eventDriver,
    void* preferredInstance)
{
    if (s_Video.initialized) {
        return PAL_RESULT_SUCCESS;
    }

    if (allocator && (!allocator->allocate || !allocator->free)) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    // check if x11 or wayland is active
    PalBool x11 = PAL_FALSE;
    PalBool wayland = PAL_FALSE;
    const char* session = getenv("XDG_SESSION_TYPE");
    if (session) {
        if (strcmp(session, "wayland") == 0) {
            wayland = PAL_TRUE;
        } else {
            x11 = PAL_TRUE;
        }
    }

    // initialize the backends
#ifdef _WIN32
    PalResult result = win32InitVideo(allocator, eventDriver, preferredInstance);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Video.backend = &s_Win32Backend;
#endif // _WIN32

    if (x11) {
#if PAL_HAS_X11_BACKEND == 1
    PalResult result = xInitVideo(allocator, eventDriver, preferredInstance);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Video.backend = &s_XBackend;
#endif // PAL_HAS_X11_BACKEND

    } else if (wayland) {
#if PAL_HAS_WAYLAND_BACKEND == 1
    PalResult result = wlInitVideo(allocator, eventDriver, preferredInstance);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Video.backend = &s_wlBackend;
#endif // PAL_HAS_WAYLAND_BACKEND
    }

    // check if we found a backend
    if (!s_Video.backend) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    s_Video.initialized = PAL_TRUE;
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo()
{
    if (s_Video.initialized) {
        s_Video.backend->shutdownVideo();
        s_Video.initialized = PAL_FALSE;
    }
}

void PAL_CALL palUpdateVideo()
{
    if (s_Video.initialized) {
        s_Video.backend->updateVideo();
    }
}

PalVideoFeatures PAL_CALL palGetVideoFeatures()
{
    if (s_Video.initialized) {
        s_Video.backend->getVideoFeatures();
    }
}

PalResult PAL_CALL palEnumerateMonitors(
    int32_t* count,
    PalMonitor** outMonitors)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!count || *count == 0 && outMonitors) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->enumerateMonitors(count, outMonitors);
}

PalResult PAL_CALL palGetPrimaryMonitor(PalMonitor** outMonitor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!outMonitor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getPrimaryMonitor(outMonitor);
}

PalResult PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getMonitorInfo(monitor, info);
}

PalResult PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    int32_t* count,
    PalMonitorMode* modes)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!monitor || !count || *count == 0 && modes) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    PalResult result = s_Video.backend->enumerateMonitorModes(monitor, count, modes);
    if (result == PAL_RESULT_SUCCESS && modes) {
        // sort the modes so that they are highest to lowest
        qsort(modes, *count, sizeof(PalMonitorMode), compareModes);
    }
}

PalResult PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!monitor || !mode) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getCurrentMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!monitor || !mode) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setMonitorMode(monitor, mode);
}

PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!monitor || !mode) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->validateMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!monitor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setMonitorOrientation(monitor, orientation);
}

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!info || !outWindow) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->createWindow(info, outWindow);
}

void PAL_CALL palDestroyWindow(PalWindow* window)
{
    if (s_Video.initialized && window) {
        return s_Video.backend->destroyWindow(window);
    }
}

PalResult PAL_CALL palMinimizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->maximizeWindow(window);
}

PalResult PAL_CALL palMaximizeWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->minimizeWindow(window);
}

PalResult PAL_CALL palRestoreWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->restoreWindow(window);
}

PalResult PAL_CALL palShowWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->showWindow(window);
}

PalResult PAL_CALL palHideWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->hideWindow(window);
}

PalResult PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->flashWindow(window, info);
}

PalResult PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window || !outStyle) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getWindowStyle(window, outStyle);
}

PalResult PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window || !outMonitor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getWindowMonitor(window, outMonitor);
}

PalResult PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window || !outBuffer) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getWindowTitle(window, bufferSize, outSize, outBuffer);
}

PalResult PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getWindowPos(window, x, y);
}

PalResult PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getWindowSize(window, width, height);
}

PalResult PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window || !outState) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getWindowState(window, outState);
}

const PalBool* PAL_CALL palGetKeycodeState()
{
    if (s_Video.initialized) {
        return s_Video.backend->getKeycodeState();
    }
    return nullptr;
}

const PalBool* PAL_CALL palGetScancodeState()
{
    if (s_Video.initialized) {
        return s_Video.backend->getScancodeState();
    }
    return nullptr;
}

const PalBool* PAL_CALL palGetMouseState()
{
    if (s_Video.initialized) {
        return s_Video.backend->getMouseState();
    }
    return nullptr;
}

void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy)
{
    if (s_Video.initialized) {
        return s_Video.backend->getMouseDelta(dx, dy);
    }
}

void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy)
{
    if (s_Video.initialized) {
        return s_Video.backend->getMouseWheelDelta(dx, dy);
    }
}

PalBool PAL_CALL palIsWindowVisible(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->isWindowVisible(window);
}

PalWindow* PAL_CALL palGetFocusWindow()
{
    if (!s_Video.initialized) {
        return nullptr;
    }

    return s_Video.backend->getFocusWindow();
}

PalResult PAL_CALL palGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    if (s_Video.initialized) {
        return s_Video.backend->getWindowHandleInfo(window, info);
    }
    return PAL_RESULT_CODE_NOT_INITIALIZED;
}

PalResult PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    if (opacity < 0.0f) {
        opacity = 0.0f;
    }

    if (opacity > 1.0f) {
        opacity = 1.0f;
    }

    return s_Video.backend->setWindowOpacity(window, opacity);
}

PalResult PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setWindowStyle(window, style);
}

PalResult PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window || !title) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setWindowTitle(window, title);
}

PalResult PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setWindowPos(window, x, y);
}

PalResult PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setWindowSize(window, width, height);
}

PalResult PAL_CALL palSetFocusWindow(PalWindow* window)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setFocusWindow(window);
}

PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!info || !outIcon) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->createIcon(info, outIcon);
}

void PAL_CALL palDestroyIcon(PalIcon* icon)
{
    if (s_Video.initialized && icon) {
        s_Video.backend->destroyIcon(icon);
    }
}

PalResult PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setWindowIcon(window, icon);
}

PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!info || !outCursor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->createCursor(info, outCursor);
}

PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!outCursor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->createCursorFrom(type, outCursor);
}

void PAL_CALL palDestroyCursor(PalCursor* cursor)
{
    if (s_Video.initialized && cursor) {
        s_Video.backend->destroyCursor(cursor);
    }
}

void PAL_CALL palShowCursor(PalBool show)
{
    if (s_Video.initialized) {
        s_Video.backend->showCursor(show);
    }
}

PalResult PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->clipCursor(window, clip);
}

PalResult PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->getCursorPos(window, x, y);
}

PalResult PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setCursorPos(window, x, y);
}

PalResult PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->setWindowCursor(window, cursor);
}

PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!windowHandle) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->attachWindow(windowHandle, outWindow);
}

PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    if (!s_Video.initialized) {
        return PAL_RESULT_CODE_NOT_INITIALIZED;
    }

    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Video.backend->detachWindow(window, outWindowHandle);
}

void* PAL_CALL palGetInstance()
{
    if (s_Video.initialized) {
        return s_Video.backend->getInstance();
    }
    return nullptr;
}