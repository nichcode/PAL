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
#include <string.h>

#if defined(_WIN32)
extern const GLBackend g_WglBackend;
#endif // _WIN32
#if defined(PAL_HAS_WAYLAND_BACKEND) || defined(PAL_HAS_X11_BACKEND)
extern const GLBackend g_EglBackend;
#endif // PAL_HAS_WAYLAND_BACKEND

static const GLBackend* backends[] = {
#if defined(_WIN32)
    &g_WglBackend,
#endif // _WIN32
#if defined(PAL_HAS_WAYLAND_BACKEND) || defined(PAL_HAS_X11_BACKEND)
    &g_EglBackend
#endif // PAL_HAS_WAYLAND_BACKEND
    nullptr
};

const GLBackend* const* platformGetGLBackends(uint32_t* count)
{
    *count = (uint32_t)(ARRAY_SIZE(backends) - 1);
    return backends;
}

PalBool checkString(
    const char* string,
    const char* strings)
{
    const char* start = strings;
    size_t stringLen = strlen(string);

    for (;;) {
        const char* where = nullptr;
        const char* terminator = nullptr;

        where = strstr(start, string);
        if (!where) {
            return PAL_FALSE;
        }

        // the string was found, we find the terminator by adding the sizeof the strings
        terminator = where + stringLen;
        if (where == start || *(where - 1) == ' ') {
            if (*terminator == ' ' || *terminator == '\0') {
                return PAL_TRUE;
            }
        }

        start = terminator;
    }
}