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

#if PAL_HAS_WAYLAND_BACKEND

#include "wayland_helper.h"
#include "pal2/video/icon.h"

PalResult wlCreateIcon(
    const PalIconCreateInfo* info,
    PalIcon** icon)
{
    (void)info;
    (void)icon;
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

void wlDestroyIcon(PalIcon* icon)
{
    (void)icon;
}

void wlSetWindowIcon(
    PalWindow* window,
    PalIcon* icon)
{
    (void)window;
    (void)icon;
}

#endif // PAL_HAS_WAYLAND_BACKEND