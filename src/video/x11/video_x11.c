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

#if PAL_HAS_X11_BACKEND

PalVideoDriver* x11GetVideoDriver(void);
PalResult x11InitVideo(const PalAllocator*, struct PalEventDriver*, const void*, void**);
void x11ShutdownVideo(void);
void x11UpdateVideo(void);
PalResult x11EnumerateMonitors(uint32_t*, PalMonitor**);
void x11GetPrimaryMonitor(PalMonitor**);
void x11GetMonitorInfo(PalMonitor*, PalMonitorInfo*);
void x11EnumerateMonitorModes(PalMonitor*, uint32_t*, PalMonitorMode*);
void x11GetCurrentMonitorMode(PalMonitor*, PalMonitorMode*);
PalResult x11SetMonitorMode(PalMonitor*, const PalMonitorMode*);
PalResult x11ValidateMonitorMode(PalMonitor*, const PalMonitorMode*);
PalResult x11SetMonitorOrientation(PalMonitor*, PalOrientation);

PalResult x11CreateWindow(const PalWindowCreateInfo*, PalWindow**);
void x11DestroyWindow(PalWindow*);
void x11MaximizeWindow(PalWindow*);
void x11MinimizeWindow(PalWindow*);
void x11RestoreWindow(PalWindow*);
void x11ShowWindow(PalWindow*);
void x11HideWindow(PalWindow*);
void x11FlashWindow(PalWindow*, const PalFlashInfo*);
PalResult x11AttachWindow(void*, PalWindow**);
PalResult x11DetachWindow(PalWindow*, void**);
void x11GetWindowStyle(PalWindow*, PalWindowStyle*);
void x11GetWindowMonitor(PalWindow*, PalMonitor**);
void x11GetWindowTitle(PalWindow*, uint64_t, uint64_t*, char*);
void x11GetWindowPos(PalWindow*, int32_t*, int32_t*);
void x11GetWindowSize(PalWindow*, uint32_t*, uint32_t*);
void x11GetWindowState(PalWindow*, PalWindowState*);
PalBool x11IsWindowVisible(PalWindow*);
PalWindow* x11GetFocusWindow();
void x11GetWindowHandleInfo(PalWindow*, PalWindowHandleInfo*);
void x11SetWindowOpacity(PalWindow*, float);
void x11SetWindowStyle(PalWindow*, PalWindowStyle);
void x11SetWindowTitle(PalWindow*, const char*);
void x11SetWindowPos(PalWindow*, int32_t, int32_t);
void x11SetWindowSize(PalWindow*, uint32_t, uint32_t);
void x11SetFocusWindow(PalWindow*);

PalResult x11CreateIcon(const PalIconCreateInfo*, PalIcon**);
void x11DestroyIcon(PalIcon*);
void x11SetWindowIcon(PalWindow*, PalIcon*);
PalResult x11CreateCursor(const PalCursorCreateInfo*, PalCursor**);
PalResult x11CreateCursorFrom(PalCursorType, PalCursor**);
void x11DestroyCursor(PalCursor*);
void x11ShowCursor(PalBool);
void x11ClipCursor(PalWindow*, PalBool);
void x11GetCursorPos(PalWindow*, int32_t*, int32_t*);
void x11SetCursorPos(PalWindow*, int32_t, int32_t);
void x11SetWindowCursor(PalWindow*, PalCursor*);

const VideoBackend g_WaylandBackend = {
    .getDriver = x11GetVideoDriver,
    .init = x11InitVideo,
    .shutdown = x11ShutdownVideo,
    .update = x11UpdateVideo,
    .enumerateMonitors = x11EnumerateMonitors,
    .getPrimaryMonitor = x11GetPrimaryMonitor,
    .getMonitorInfo = x11GetMonitorInfo,
    .enumerateMonitorModes = x11EnumerateMonitorModes,
    .getCurrentMonitorMode = x11GetCurrentMonitorMode,
    .setMonitorMode = x11SetMonitorMode,
    .validateMonitorMode = x11ValidateMonitorMode,
    .setMonitorOrientation = x11SetMonitorOrientation,

    .createWindow = x11CreateWindow,
    .destroyWindow = x11DestroyWindow,
    .maximizeWindow = x11MaximizeWindow,
    .minimizeWindow = x11MinimizeWindow,
    .restoreWindow = x11RestoreWindow,
    .showWindow = x11ShowWindow,
    .hideWindow = x11HideWindow,
    .flashWindow = x11FlashWindow,
    .attachWindow = x11AttachWindow,
    .detachWindow = x11DetachWindow,
    .getWindowStyle = x11GetWindowStyle,
    .getWindowMonitor = x11GetWindowMonitor,
    .getWindowTitle = x11GetWindowTitle,
    .getWindowPos = x11GetWindowPos,
    .getWindowSize = x11GetWindowSize,
    .getWindowState = x11GetWindowState,
    .isWindowVisible = x11IsWindowVisible,
    .getFocusWindow = x11GetFocusWindow,
    .getWindowHandleInfo = x11GetWindowHandleInfo,
    .setWindowOpacity = x11SetWindowOpacity,
    .setWindowStyle = x11SetWindowStyle,
    .setWindowTitle = x11SetWindowTitle,
    .setWindowPos = x11SetWindowPos,
    .setWindowSize = x11SetWindowSize,
    .setFocusWindow = x11SetFocusWindow,

    .createIcon = x11CreateIcon,
    .destroyIcon = x11DestroyIcon,
    .setWindowIcon = x11SetWindowIcon,
    .createCursor = x11CreateCursor,
    .createCursorFrom = x11CreateCursorFrom,
    .destroyCursor = x11DestroyCursor,
    .showCursor = x11ShowCursor,
    .clipCursor = x11ClipCursor,
    .getCursorPos = x11GetCursorPos,
    .setCursorPos = x11SetCursorPos,
    .setWindowCursor = x11SetWindowCursor
};

#endif // PAL_HAS_X11_BACKEND