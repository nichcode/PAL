
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

PalVideoDriver* waylandGetVideoDriver(void);
PalResult waylandInitVideo(const PalAllocator*, struct PalEventDriver*, const void*, void**);
void waylandShutdownVideo(void);
void waylandUpdateVideo(void);
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
    .getDriver = waylandGetVideoDriver,
    .init = waylandInitVideo,
    .shutdown = waylandUpdateVideo,
    .update = waylandUpdateVideo,
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