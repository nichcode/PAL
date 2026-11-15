
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

#include "platform.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

#if defined(_WIN32)
extern const VideoBackend g_Win32Backend;
#endif // _WIN32
#if defined(PAL_HAS_WAYLAND_BACKEND)
extern const VideoBackend g_WaylandBackend;
#endif // PAL_HAS_WAYLAND_BACKEND
#if defined(PAL_HAS_X11_BACKEND)
extern const VideoBackend g_X11Backend;
#endif // PAL_HAS_X11_BACKEND

static const VideoBackend* backends[] = {
#if defined(_WIN32)
    &g_Win32Backend,
#endif // _WIN32
#if defined(PAL_HAS_WAYLAND_BACKEND)
    &g_WaylandBackend,
#endif // PAL_HAS_WAYLAND_BACKEND
#if defined(PAL_HAS_X11_BACKEND)
    &g_X11Backend,
#endif // PAL_HAS_X11_BACKEND
};

const VideoBackend* const* platformGetVideoBackends(uint32_t* count)
{
    *count = ARRAY_SIZE(backends);
    return backends;
}