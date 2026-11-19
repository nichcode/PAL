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

#if PAL_HAS_X11_BACKEND

#include "x11_helper.h"
#include "pal2/core/memory.h"

RRMode x11FindMode(
    XRRScreenResources* resources,
    const PalMonitorMode* mode)
{
    for (int i = 0; i < resources->nmode; ++i) {
        XRRModeInfo* info = &resources->modes[i];
        double tmp = (double)info->hTotal * (double)info->vTotal;
        double rate = (double)info->dotClock / tmp;

        // compare with width, height and refresh rate
        if (info->width == mode->width && info->height == mode->height &&
            (uint32_t)(rate + 0.5) == mode->refreshRate) {
            return info->id;
        }
    }
    return None;
}

void x11SendWMEvent(
    Window window,
    Atom type,
    long a,
    long b,
    long c,
    long d,
    PalBool add)
{
    XEvent e = {0};
    e.xclient.type = ClientMessage;
    e.xclient.send_event = True;
    e.xclient.window = window;
    e.xclient.message_type = type;
    e.xclient.format = 32;

    if (add) {
        e.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
    } else {
        e.xclient.data.l[0] = 0; // _NET_WM_STATE_REMOVE
    }

    e.xclient.data.l[1] = a;
    e.xclient.data.l[2] = b;
    e.xclient.data.l[3] = c;
    e.xclient.data.l[4] = d;

    g_X11.sendEvent(
        g_X11.display,
        g_X11.root,
        False,
        SubstructureNotifyMask | SubstructureRedirectMask,
        &e);
}

MonitorData* x11GetFreeMonitorData(void)
{
    for (uint32_t i = 0; i < g_X11.maxMonitorData; ++i) {
        if (!g_X11.monitorData[i].used) {
            memset(&g_X11.monitorData[i], 0, sizeof(MonitorData));
            g_X11.monitorData[i].used = PAL_TRUE;
            return &g_X11.monitorData[i];
        }
    }

    uint32_t oldCount = g_X11.maxMonitorData;
    uint32_t count = (oldCount > 0) ? (oldCount * 2) : 4;
    MonitorData* data = palAllocate(g_X11.allocator, sizeof(MonitorData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(MonitorData) * count);
        if (g_X11.monitorData && oldCount > 0) {
            memcpy(data, g_X11.monitorData, oldCount * sizeof(MonitorData));
            palFree(g_X11.allocator, g_X11.monitorData);
        }

        g_X11.monitorData = data;
        g_X11.maxMonitorData = count;

        g_X11.monitorData[oldCount].used = PAL_TRUE;
        return &g_X11.monitorData[oldCount];
    }
    return nullptr;
}

MonitorData* x11FindMonitorData(PalMonitor* monitor)
{
    for (uint32_t i = 0; i < g_X11.maxMonitorData; ++i) {
        if (g_X11.monitorData[i].used && g_X11.monitorData[i].monitor == monitor) {
            return &g_X11.monitorData[i];
        }
    }
    return nullptr;
}

void x11FreeMonitorData(PalMonitor* monitor)
{
    for (uint32_t i = 0; i < g_X11.maxMonitorData; ++i) {
        if (g_X11.monitorData[i].used && g_X11.monitorData[i].monitor == monitor) {
            g_X11.monitorData[i].used = PAL_FALSE;
        }
    }
}

WindowData* x11GetFreeWindowData(void)
{
    for (uint32_t i = 0; i < g_X11.maxWindowData; ++i) {
        if (!g_X11.windowData[i].used) {
            memset(&g_X11.windowData[i], 0, sizeof(WindowData));
            g_X11.windowData[i].used = PAL_TRUE;
            return &g_X11.windowData[i];
        }
    }

    uint32_t oldCount = g_X11.maxWindowData;
    uint32_t count = (oldCount > 0) ? (oldCount * 2) : 4;
    WindowData* data = palAllocate(g_X11.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(WindowData) * count);
        if (g_X11.windowData && oldCount > 0) {
            memcpy(data, g_X11.windowData, oldCount * sizeof(WindowData));
            palFree(g_X11.allocator, g_X11.windowData);
        }

        g_X11.windowData = data;
        g_X11.maxWindowData = count;

        g_X11.windowData[oldCount].used = PAL_TRUE;
        return &g_X11.windowData[oldCount];
    }
    return nullptr;
}

WindowData* x11FindWindowData(PalWindow* window)
{
    for (uint32_t i = 0; i < g_X11.maxWindowData; ++i) {
        if (g_X11.windowData[i].used && g_X11.windowData[i].window == window) {
            return &g_X11.windowData[i];
        }
    }
    return nullptr;
}

#endif // PAL_HAS_X11_BACKEND