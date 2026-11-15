
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

#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

static int createShmFile(uint64_t size)
{
    char template[] = "/tmp/pal-shm-XXXXXX";
    int fd = mkstemp(template);
    if (fd < 0) {
        return -1;
    }

    unlink(template);
    if (ftruncate(fd, (off_t)size) < 0) {
        return -1;
    }

    return fd;
}

struct wl_buffer* createShmBuffer(
    int width,
    int height,
    const uint8_t* pixels,
    PalBool cursor)
{
    int stride = width * 4;
    uint64_t size = (uint64_t)(stride * height);
    struct wl_buffer* buffer = nullptr;
    struct wl_shm_pool* pool = nullptr;
    void* data = nullptr;

    int fd = createShmFile(size);
    if (fd == -1) {
        return nullptr;
    }

    data = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
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
            dataPixels[i] = ((unsigned long)a << 24) |
                            ((unsigned long)r << 16) |
                            ((unsigned long)g << 8) |
                            ((unsigned long)b);
            // clang-format on
        }

    } else {
        memset(data, 0xFF, size); // white
    }

    pool = wlShmCreatePool(g_Wayland.shm, fd, (int32_t)size);
    if (!pool) {
        return nullptr;
    }

    buffer = wlShmPoolCreateBuffer(pool, 0, width, height, stride, format);
    if (!buffer) {
        return nullptr;
    }

    wlShmPoolDestroy(pool);
    munmap(data, size);
    close(fd);
    return buffer;
}

MonitorData* getFreeMonitorData(void)
{
    for (uint32_t i = 0; i < g_Wayland.maxMonitorData; ++i) {
        if (!g_Wayland.monitorData[i].used) {
            g_Wayland.monitorData[i].used = PAL_TRUE;
            return &g_Wayland.monitorData[i];
        }
    }

    // resize the data array
    MonitorData* data = nullptr;
    uint32_t count = g_Wayland.maxMonitorData * 2; // double the size
    uint32_t freeIndex = g_Wayland.maxMonitorData + 1;
    data = palAllocate(g_Wayland.allocator, sizeof(MonitorData) * count, 0);
    if (data) {
        memcpy(data, g_Wayland.monitorData, g_Wayland.maxMonitorData * sizeof(MonitorData));

        palFree(g_Wayland.allocator, g_Wayland.monitorData);
        g_Wayland.monitorData = data;
        g_Wayland.maxWindowData = count;

        g_Wayland.monitorData[freeIndex].used = PAL_TRUE;
        return &g_Wayland.monitorData[freeIndex];
    }
    return nullptr;
}

MonitorData* findMonitorData(PalMonitor* monitor)
{
    for (uint32_t i = 0; i < g_Wayland.maxMonitorData; ++i) {
        if (g_Wayland.monitorData[i].used && g_Wayland.monitorData[i].monitor == monitor) {
            return &g_Wayland.monitorData[i];
        }
    }
    return nullptr;
}

void freeMonitorData(PalMonitor* monitor)
{
    for (uint32_t i = 0; i < g_Wayland.maxMonitorData; ++i) {
        if (g_Wayland.monitorData[i].used && g_Wayland.monitorData[i].monitor == monitor) {
            g_Wayland.monitorData[i].used = PAL_FALSE;
        }
    }
}

WindowData* getFreeWindowData(void)
{
    for (uint32_t i = 0; i < g_Wayland.maxWindowData; ++i) {
        if (!g_Wayland.windowData[i].used) {
            g_Wayland.windowData[i].used = PAL_TRUE;
            return &g_Wayland.windowData[i];
        }
    }

    // resize the data array
    WindowData* data = nullptr;
    uint32_t count = g_Wayland.maxWindowData * 2; // double the size
    uint32_t freeIndex = g_Wayland.maxWindowData + 1;
    data = palAllocate(g_Wayland.allocator, sizeof(WindowData) * count, 0);
    if (data) {
        memcpy(data, g_Wayland.windowData, g_Wayland.maxWindowData * sizeof(WindowData));

        palFree(g_Wayland.allocator, g_Wayland.windowData);
        g_Wayland.windowData = data;
        g_Wayland.maxWindowData = count;

        g_Wayland.windowData[freeIndex].used = PAL_TRUE;
        return &g_Wayland.windowData[freeIndex];
    }
    return nullptr;
}

WindowData* findWindowData(PalWindow* window)
{
    for (uint32_t i = 0; i < g_Wayland.maxWindowData; ++i) {
        if (g_Wayland.windowData[i].used && g_Wayland.windowData[i].window == window) {
            return &g_Wayland.windowData[i];
        }
    }
    return nullptr;
}

#endif // PAL_HAS_WAYLAND_BACKEND