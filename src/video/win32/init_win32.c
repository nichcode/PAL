
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
#include "pal2/video/init.h"

static PalVideoDriver s_Win32Driver = {0};

PalVideoDriver* win32GetVideoDriver()
{
    s_Win32Driver.type = PAL_VIDEO_DRIVER_TYPE_WIN32;
    s_Win32Driver.features = PAL_VIDEO_FEATURE_BORDERLESS_WINDOW;

    return &s_Win32Driver;
}

const VideoBackend g_Win32Backend = {
    .getDriver = win32GetVideoDriver
};