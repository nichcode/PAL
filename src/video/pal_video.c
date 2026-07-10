
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#include "pal_video_backends.h"
#include "pal_video_egl.h"
#include <stdlib.h>

VideoEGL s_VideoEgl = {0};
static VideoBackend* s_Backend = nullptr;

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
    s_Backend = &s_Win32Backend;
#endif // _WIN32

    if (x11) {
#if PAL_HAS_X11_BACKEND == 1
    PalResult result = xInitVideo(allocator, eventDriver, preferredInstance);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Backend = &s_XBackend;
#endif // PAL_HAS_X11_BACKEND

    } else if (wayland) {
#if PAL_HAS_WAYLAND_BACKEND == 1
    PalResult result = wlInitVideo(allocator, eventDriver, preferredInstance);
    if (result != PAL_RESULT_SUCCESS) {
        return result;
    }
    s_Backend = &s_wlBackend;
#endif // PAL_HAS_WAYLAND_BACKEND
    }

    // check if we found a backend
    if (!s_Backend) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }
    return PAL_RESULT_SUCCESS;
}

void PAL_CALL palShutdownVideo()
{
    s_Backend->shutdownVideo();
}

void PAL_CALL palUpdateVideo()
{
    s_Backend->updateVideo();
}

PalVideoFeatures PAL_CALL palGetVideoFeatures()
{
    return s_Backend->getVideoFeatures();
}

PalResult PAL_CALL palEnumerateMonitors(
    uint32_t* count,
    PalMonitor** outMonitors)
{
    if (!count) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->enumerateMonitors(count, outMonitors);
}

void PAL_CALL palGetPrimaryMonitor(PalMonitor** outMonitor)
{
    return s_Backend->getPrimaryMonitor(outMonitor);
}

void PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    s_Backend->getMonitorInfo(monitor, info);
}

void PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes)
{
    s_Backend->enumerateMonitorModes(monitor, count, modes);
    if (modes) {
        // sort the modes so that they are highest to lowest
        qsort(modes, *count, sizeof(PalMonitorMode), compareModes);
    }
}

void PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return s_Backend->getCurrentMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return s_Backend->setMonitorMode(monitor, mode);
}

PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    return s_Backend->validateMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    return s_Backend->setMonitorOrientation(monitor, orientation);
}

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    if (!info || !outWindow) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->createWindow(info, outWindow);
}

void PAL_CALL palDestroyWindow(PalWindow* window)
{
    return s_Backend->destroyWindow(window);
}

void PAL_CALL palMinimizeWindow(PalWindow* window)
{
    s_Backend->maximizeWindow(window);
}

void PAL_CALL palMaximizeWindow(PalWindow* window)
{
    s_Backend->minimizeWindow(window);
}

void PAL_CALL palRestoreWindow(PalWindow* window)
{
    s_Backend->restoreWindow(window);
}

void PAL_CALL palShowWindow(PalWindow* window)
{
    s_Backend->showWindow(window);
}

void PAL_CALL palHideWindow(PalWindow* window)
{
    s_Backend->hideWindow(window);
}

void PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    s_Backend->flashWindow(window, info);
}

void PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    s_Backend->getWindowStyle(window, outStyle);
}

void PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    s_Backend->getWindowMonitor(window, outMonitor);
}

void PAL_CALL palGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    s_Backend->getWindowTitle(window, bufferSize, outSize, outBuffer);
}

void PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    s_Backend->getWindowPos(window, x, y);
}

void PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height)
{
    s_Backend->getWindowSize(window, width, height);
}

void PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    s_Backend->getWindowState(window, outState);
}

const PalBool* PAL_CALL palGetKeycodeState()
{
    return s_Backend->getKeycodeState();
}

const PalBool* PAL_CALL palGetScancodeState()
{
    return s_Backend->getScancodeState();
}

const PalBool* PAL_CALL palGetMouseState()
{
    return s_Backend->getMouseState();
}

void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy)
{
    s_Backend->getMouseDelta(dx, dy);
}

void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy)
{
    s_Backend->getMouseWheelDelta(dx, dy);
}

PalBool PAL_CALL palIsWindowVisible(PalWindow* window)
{
    return s_Backend->isWindowVisible(window);
}

PalWindow* PAL_CALL palGetFocusWindow()
{
    return s_Backend->getFocusWindow();
}

void PAL_CALL palGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    s_Backend->getWindowHandleInfo(window, info);
}

void PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    s_Backend->setWindowOpacity(window, opacity);
}

void PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    s_Backend->setWindowStyle(window, style);
}

void PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    s_Backend->setWindowTitle(window, title);
}

void PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    s_Backend->setWindowPos(window, x, y);
}

void PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    s_Backend->setWindowSize(window, width, height);
}

void PAL_CALL palSetFocusWindow(PalWindow* window)
{
    s_Backend->setFocusWindow(window);
}

PalResult PAL_CALL palCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** outIcon)
{
    if (!info || !outIcon) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->createIcon(info, outIcon);
}

void PAL_CALL palDestroyIcon(PalIcon* icon)
{
    s_Backend->destroyIcon(icon);
}

void PAL_CALL palSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    s_Backend->setWindowIcon(window, icon);
}

PalResult PAL_CALL palCreateCursor(
    const PalCursorCreateInfo* info,
    PalCursor** outCursor)
{
    if (!info || !outCursor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->createCursor(info, outCursor);
}

PalResult PAL_CALL palCreateCursorFrom(
    PalCursorType type,
    PalCursor** outCursor)
{
    if (!outCursor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->createCursorFrom(type, outCursor);
}

void PAL_CALL palDestroyCursor(PalCursor* cursor)
{
    s_Backend->destroyCursor(cursor);
}

void PAL_CALL palShowCursor(PalBool show)
{
    s_Backend->showCursor(show);
}

void PAL_CALL palClipCursor(
    PalWindow* window,
    PalBool clip)
{
    s_Backend->clipCursor(window, clip);
}

void PAL_CALL palGetCursorPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    s_Backend->getCursorPos(window, x, y);
}

void PAL_CALL palSetCursorPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    s_Backend->setCursorPos(window, x, y);
}

void PAL_CALL palSetWindowCursor(
    PalWindow* window,
    PalCursor* cursor)
{
    s_Backend->setWindowCursor(window, cursor);
}

PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    if (!windowHandle) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->attachWindow(windowHandle, outWindow);
}

PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    if (!window) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return s_Backend->detachWindow(window, outWindowHandle);
}

void* PAL_CALL palGetInstance()
{
    return s_Backend->getInstance();
}