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
#include "pal2/video/monitor.h"
#include <stdlib.h>

/* =========================================================================
 * Helper
 * ========================================================================= */

static int compareModes(
    const void* a,
    const void* b)
{
    const PalMonitorMode* mode1 = (const PalMonitorMode*)a;
    const PalMonitorMode* mode2 = (const PalMonitorMode*)b;

    if (mode1->width > mode2->width) {
        return -1;
    }

    if (mode1->width < mode2->width) {
        return 1;
    }

    if (mode1->height > mode2->height) {
        return -1;
    }

    if (mode1->height < mode2->height) {
        return 1;
    }

    if (mode1->refreshRate > mode2->refreshRate) {
        return -1;
    }

    if (mode1->refreshRate < mode2->refreshRate) {
        return 1;
    }

    if (mode1->bpp > mode2->bpp) {
        return -1;
    }

    if (mode1->bpp < mode2->bpp) {
        return 1;
    }

    return 0;
}


/* =========================================================================
 * API
 * ========================================================================= */

PalResult PAL_CALL palEnumerateMonitors(
    uint32_t* count,
    PalMonitor** monitors)
{
    if (!g_VideoInitialized) {
        return PAL_RESULT_CODE_INVALID_OPERATION;
    }

    if (!count || (!monitors && *count > 0)) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return g_VideoBackend->enumerateMonitors(count, monitors);
}

void PAL_CALL palGetPrimaryMonitor(PalMonitor** monitor)
{
    ASSERT(monitor != nullptr, "monitor parameter is nullptr");

    g_VideoBackend->getPrimaryMonitor(monitor);
}

void PAL_CALL palGetMonitorInfo(
    PalMonitor* monitor,
    PalMonitorInfo* info)
{
    ASSERT(monitor != nullptr, "monitor parameter is nullptr");
    ASSERT(info != nullptr, "info parameter is nullptr");

    g_VideoBackend->getMonitorInfo(monitor, info);
}

void PAL_CALL palEnumerateMonitorModes(
    PalMonitor* monitor,
    uint32_t* count,
    PalMonitorMode* modes)
{
    ASSERT(monitor != nullptr, "monitor parameter is nullptr");
    ASSERT(count != nullptr, "count parameter is nullptr");
    ASSERT(modes == nullptr || *count > 0, "modes parameter is not nullptr but *count is 0");

    g_VideoBackend->enumerateMonitorModes(monitor, count, modes);
    if (modes) {
        qsort(modes, *count, sizeof(PalMonitorMode), compareModes);
    }
}

void PAL_CALL palGetCurrentMonitorMode(
    PalMonitor* monitor,
    PalMonitorMode* mode)
{
    ASSERT(monitor != nullptr, "monitor parameter is nullptr");
    ASSERT(mode != nullptr, "mode parameter is nullptr");

    g_VideoBackend->getCurrentMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    if (!monitor || !mode) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return g_VideoBackend->setMonitorMode(monitor, mode);
}

PalResult PAL_CALL palValidateMonitorMode(
    PalMonitor* monitor,
    const PalMonitorMode* mode)
{
    if (!monitor || !mode) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return g_VideoBackend->validateMonitorMode(monitor, mode);
}

PalResult PAL_CALL palSetMonitorOrientation(
    PalMonitor* monitor,
    PalOrientation orientation)
{
    if (!monitor) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    return g_VideoBackend->setMonitorOrientation(monitor, orientation);
}