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

#include "pal2/video/window.h"
#include "platform.h"

PalResult PAL_CALL palCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** window)
{
    if (!g_VideoInitialized) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_OPERATION,
            PAL_RESULT_SOURCE_NONE,
            0);
    }

    if (!info || !window || !info->width || !info->height ||
        info->fbConfigBackend >= PAL_FBCONFIG_BACKEND_COUNT ||
        info->state >= PAL_WINDOW_STATE_COUNT) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_ARGUMENT,
            PAL_RESULT_SOURCE_NONE,
            0);
    }

    return g_VideoBackend->createWindow(info, window);
}

void PAL_CALL palDestroyWindow(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->destroyWindow(window);
}

void PAL_CALL palMinimizeWindow(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->minimizeWindow(window);
}

void PAL_CALL palMaximizeWindow(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->maximizeWindow(window);
}

void PAL_CALL palRestoreWindow(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->restoreWindow(window);
}

void PAL_CALL palShowWindow(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->showWindow(window);
}

void PAL_CALL palHideWindow(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->hideWindow(window);
}

void PAL_CALL palFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(info != nullptr, "info parameter is nullptr");

    g_VideoBackend->flashWindow(window, info);
}

PalResult PAL_CALL palAttachWindow(
    void* windowHandle,
    PalWindow** window)
{
    if (!g_VideoInitialized) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_OPERATION,
            PAL_RESULT_SOURCE_NONE,
            0);
    }

    if (!windowHandle || !window) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_ARGUMENT,
            PAL_RESULT_SOURCE_NONE,
            0);
    }

    return g_VideoBackend->attachWindow(windowHandle, window);
}

PalResult PAL_CALL palDetachWindow(
    PalWindow* window,
    void** windowHandle)
{
    if (!g_VideoInitialized) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_OPERATION,
            PAL_RESULT_SOURCE_NONE,
            0);
    }

    if (!window) {
        return palMakeResult(
            PAL_RESULT_CODE_INVALID_ARGUMENT,
            PAL_RESULT_SOURCE_NONE,
            0);
    }
    
    return g_VideoBackend->detachWindow(window, windowHandle);
}

void PAL_CALL palGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* style)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(style != nullptr, "style parameter is nullptr");

    g_VideoBackend->getWindowStyle(window, style);
}

void PAL_CALL palGetWindowMonitor(
    PalWindow* window,
    PalMonitor** monitor)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(monitor != nullptr, "monitor parameter is nullptr");

    g_VideoBackend->getWindowMonitor(window, monitor);
}

void PAL_CALL palGetWindowTitle(
    PalWindow* window,
    size_t bufferSize,
    size_t* size,
    char* buffer)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(buffer == nullptr || bufferSize > 0, "buffer is not nullptr but bufferSize is 0");

    g_VideoBackend->getWindowTitle(window, bufferSize, size, buffer);
}

void PAL_CALL palGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->getWindowPos(window, x, y);
}

void PAL_CALL palGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->getWindowSize(window, width, height);
}

void PAL_CALL palGetWindowState(
    PalWindow* window,
    PalWindowState* state)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(state != nullptr, "state parameter is nullptr");

    g_VideoBackend->getWindowState(window, state);
}

PalBool PAL_CALL palIsWindowVisible(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    return g_VideoBackend->isWindowVisible(window);
}

PalWindow* PAL_CALL palGetFocusWindow(void)
{
    if (!g_VideoInitialized) {
        return nullptr;
    }

    return g_VideoBackend->getFocusWindow();
}

void PAL_CALL palGetWindowHandleInfo(
    PalWindow* window,
    PalWindowHandleInfo* info)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(info != nullptr, "info parameter is nullptr");

    g_VideoBackend->getWindowHandleInfo(window, info);
}

void PAL_CALL palSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->setWindowOpacity(window, opacity);
}

void PAL_CALL palSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->setWindowStyle(window, style);
}

void PAL_CALL palSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    ASSERT(window != nullptr, "window parameter is nullptr");
    ASSERT(title != nullptr, "title parameter is nullptr");

    g_VideoBackend->setWindowTitle(window, title);
}

void PAL_CALL palSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->setWindowPos(window, x, y);
}

void PAL_CALL palSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->setWindowSize(window, width, height);
}

void PAL_CALL palSetFocusWindow(PalWindow* window)
{
    ASSERT(window != nullptr, "window parameter is nullptr");

    g_VideoBackend->setFocusWindow(window);
}