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
#define _POSIX_C_SOURCE 200809L

#include "wayland_helper.h"
#include "client_protocol.h"
#include "pal2/core/memory.h"

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

static int createShmFile(uint64_t size)
{
    char template[] = "/tmp/pal-shm-XXXXXX";
    int fd = mkstemp(template);
    if (fd < 0) {
        return -1;
    }

    unlink(template);
    if (ftruncate(fd, (off_t)size) < 0) {
        close(fd);
        return -1;
    }

    return fd;
}

struct wl_buffer* wlCreateShmBuffer(
    int width,
    int height,
    const uint8_t* pixels,
    PalBool cursor)
{
    int stride = width * 4;
    uint64_t size = (uint64_t)stride * (uint64_t)height;
    struct wl_buffer* buffer = nullptr;
    struct wl_shm_pool* pool = nullptr;
    void* data = MAP_FAILED;

    int fd = createShmFile(size);
    if (fd == -1) {
        return nullptr;
    }

    data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        close(fd);
        return nullptr;
    }

    enum wl_shm_format format = WL_SHM_FORMAT_XRGB8888;
    if (cursor) {
        format = WL_SHM_FORMAT_ARGB8888;
        uint32_t* dataPixels = (uint32_t*)data;

        // convert from RGBA8 to ARGB32
        for (int i = 0; i < width * height; i++) {
            uint8_t r = pixels[i * 4 + 0]; // Red
            uint8_t g = pixels[i * 4 + 1]; // Green
            uint8_t b = pixels[i * 4 + 2]; // Blue
            uint8_t a = pixels[i * 4 + 3]; // Alpha

            // clang-format off
            dataPixels[i] = ((uint32_t)a << 24) |
                            ((uint32_t)r << 16) |
                            ((uint32_t)g << 8) |
                            ((uint32_t)b);
            // clang-format on
        }
    } else {
        memset(data, 0xFF, size); // white
    }

    pool = wlShmCreatePool(g_Wl.shm, fd, (int32_t)size);
    if (!pool) {
        munmap(data, size);
        close(fd);
        return nullptr;
    }

    buffer = wlShmPoolCreateBuffer(pool, 0, width, height, stride, format);
    wlShmPoolDestroy(pool);
    munmap(data, size);
    close(fd);

    return buffer;
}

MonitorData* wlGetFreeMonitorData(void)
{
    for (uint32_t i = 0; i < g_Wl.maxMonitorData; ++i) {
        if (!g_Wl.monitorData[i].used) {
            memset(&g_Wl.monitorData[i], 0, sizeof(MonitorData));
            g_Wl.monitorData[i].used = PAL_TRUE;
            return &g_Wl.monitorData[i];
        }
    }

    uint32_t oldCount = g_Wl.maxMonitorData;
    uint32_t count = (oldCount > 0) ? (oldCount * 2) : 4;
    MonitorData* data = palAllocate(g_Wl.allocator, sizeof(MonitorData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(MonitorData) * count);
        if (g_Wl.monitorData && oldCount > 0) {
            memcpy(data, g_Wl.monitorData, oldCount * sizeof(MonitorData));
            palFree(g_Wl.allocator, g_Wl.monitorData);
        }

        g_Wl.monitorData = data;
        g_Wl.maxMonitorData = count;

        g_Wl.monitorData[oldCount].used = PAL_TRUE;
        return &g_Wl.monitorData[oldCount];
    }
    return nullptr;
}

MonitorData* wlFindMonitorData(PalMonitor* monitor)
{
    for (uint32_t i = 0; i < g_Wl.maxMonitorData; ++i) {
        if (g_Wl.monitorData[i].used && g_Wl.monitorData[i].monitor == monitor) {
            return &g_Wl.monitorData[i];
        }
    }
    return nullptr;
}

void wlFreeMonitorData(PalMonitor* monitor)
{
    for (uint32_t i = 0; i < g_Wl.maxMonitorData; ++i) {
        if (g_Wl.monitorData[i].used && g_Wl.monitorData[i].monitor == monitor) {
            g_Wl.monitorData[i].used = PAL_FALSE;
        }
    }
}

WindowData* wlGetFreeWindowData(void)
{
    for (uint32_t i = 0; i < g_Wl.maxWindowData; ++i) {
        if (!g_Wl.windowData[i].used) {
            memset(&g_Wl.windowData[i], 0, sizeof(WindowData));
            g_Wl.windowData[i].used = PAL_TRUE;
            return &g_Wl.windowData[i];
        }
    }

    uint32_t oldCount = g_Wl.maxWindowData;
    uint32_t count = (oldCount > 0) ? (oldCount * 2) : 4;
    WindowData* data = palAllocate(g_Wl.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memset(data, 0, sizeof(WindowData) * count);
        if (g_Wl.windowData && oldCount > 0) {
            memcpy(data, g_Wl.windowData, oldCount * sizeof(WindowData));
            palFree(g_Wl.allocator, g_Wl.windowData);
        }

        g_Wl.windowData = data;
        g_Wl.maxWindowData = count;

        g_Wl.windowData[oldCount].used = PAL_TRUE;
        return &g_Wl.windowData[oldCount];
    }
    return nullptr;
}

WindowData* wlFindWindowData(PalWindow* window)
{
    for (uint32_t i = 0; i < g_Wl.maxWindowData; ++i) {
        if (g_Wl.windowData[i].used && g_Wl.windowData[i].window == window) {
            return &g_Wl.windowData[i];
        }
    }
    return nullptr;
}

#endif // PAL_HAS_WAYLAND_BACKEND