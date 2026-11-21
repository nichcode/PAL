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

#ifdef _WIN32

PalVideoDriver* win32GetVideoDriver(void);
PalResult win32InitVideo(const PalAllocator*, struct PalEventDriver*, const void*, void**);
void win32ShutdownVideo(void);
void win32UpdateVideo(void);
PalResult win32EnumerateMonitors(uint32_t*, PalMonitor**);
void win32GetPrimaryMonitor(PalMonitor**);
void win32GetMonitorInfo(PalMonitor*, PalMonitorInfo*);
void win32EnumerateMonitorModes(PalMonitor*, uint32_t*, PalMonitorMode*);
void win32GetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult win32SetMonitorMode(PalMonitor*, const PalMonitorMode*);
PalResult win32ValidateMonitorMode(PalMonitor*, const PalMonitorMode*);
PalResult win32SetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult win32CreateWindow(const PalWindowCreateInfo*, PalWindow**);
void win32DestroyWindow(PalWindow*);
void win32MaximizeWindow(PalWindow*);
void win32MinimizeWindow(PalWindow*);
void win32RestoreWindow(PalWindow*);
void win32ShowWindow(PalWindow*);
void win32HideWindow(PalWindow*);
void win32FlashWindow(PalWindow*, const PalFlashInfo*);
PalResult win32AttachWindow(void*, PalWindow**);
PalResult win32DetachWindow(PalWindow*, void**);
void win32GetWindowStyle(PalWindow*, PalWindowStyle*);
void win32GetWindowMonitor(PalWindow*, PalMonitor**);
void win32GetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
void win32GetWindowPos(PalWindow*, int32_t*, int32_t*);
void win32GetWindowSize(PalWindow*, uint32_t*, uint32_t*);
void win32GetWindowState(PalWindow*, PalWindowState*);
PalBool win32IsWindowVisible(PalWindow*);
PalWindow* win32GetFocusWindow(void);
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

const VideoBackend g_win32Backend = {
    .getDriver = win32GetVideoDriver,
    .init = win32InitVideo,
    .shutdown = win32ShutdownVideo,
    .update = win32UpdateVideo,
    .enumerateMonitors = win32EnumerateMonitors,
    .getPrimaryMonitor = win32GetPrimaryMonitor,
    .getMonitorInfo = win32GetMonitorInfo,
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
    .attachWindow = win32AttachWindow,
    .detachWindow = win32DetachWindow,
    .getWindowStyle = win32GetWindowStyle,
    .getWindowMonitor = win32GetWindowMonitor,
    .getWindowTitle = win32GetWindowTitle,
    .getWindowPos = win32GetWindowPos,
    .getWindowSize = win32GetWindowSize,
    .getWindowState = win32GetWindowState,
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
    .setWindowCursor = win32SetWindowCursor
};

#endif // _WIN32