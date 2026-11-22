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
#include "pal2/video/mouse.h"

Mouse g_Mouse = {0};

static const char* s_MouseButtonNames[PAL_MOUSE_BUTTON_COUNT] = {
    [PAL_MOUSE_BUTTON_UNKNOWN] = "Unknown",

    [PAL_MOUSE_BUTTON_LEFT] = "Left",
    [PAL_MOUSE_BUTTON_RIGHT] = "Right",
    [PAL_MOUSE_BUTTON_MIDDLE] = "Middle",
    [PAL_MOUSE_BUTTON_X1] = "X1",
    [PAL_MOUSE_BUTTON_X2] = "X2",};

const PalBool* PAL_CALL palGetMouseState(void)
{
    return g_Mouse.state;
}

const char* PAL_CALL palGetMouseButtonName(PalMouseButton button)
{
    uint32_t index = (uint32_t)button;
    if (index >= PAL_MOUSE_BUTTON_COUNT || !s_MouseButtonNames[index]) {
        return s_MouseButtonNames[PAL_MOUSE_BUTTON_UNKNOWN];
    }

    return s_MouseButtonNames[index];
}

void PAL_CALL palGetMouseDelta(
    float* dx,
    float* dy)
{
    if (dx) {
        *dx = (float)g_Mouse.dx;
    }

    if (dy) {
        *dy = (float)g_Mouse.dy;
    }
}

void PAL_CALL palGetMouseWheelDelta(
    float* dx,
    float* dy)
{
    if (dx) {
        *dx = (float)g_Mouse.wheelX;
    }

    if (dy) {
        *dy = (float)g_Mouse.wheelY;
    }
}