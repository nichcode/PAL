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

#if PAL_HAS_EVENT_MODULE
#include "pal2/core/pack.h"
#include "pal2/event/driver.h"
#endif // PAL_HAS_EVENT_MODULE

#include "wayland_helper.h"
#include "pal2/video/window.h"
#include "client_protocol.h"
#include "xdg_protocol.h"
#include "video/video_egl.h"

#include "listeners.h"
#include <errno.h>
#include <stdlib.h>

/* =========================================================================
 * Helper
 * ========================================================================= */

static PalBool eglWlBackend(const int fbConfigIndex)
{
    if (fbConfigIndex <= 0) {
        return PAL_FALSE;
    }

    EGLDisplay display = g_VideoEgl.getDisplay((EGLNativeDisplayType)g_Wl.display);
    if (display == EGL_NO_DISPLAY) {
        return PAL_FALSE;
    }

    EGLint numConfigs = 0;
    if (!g_VideoEgl.getConfigs(display, nullptr, 0, &numConfigs) || fbConfigIndex > numConfigs) {
        return PAL_FALSE;
    }

    uint64_t configSize = sizeof(EGLConfig) * (uint64_t)numConfigs;
    EGLConfig* eglConfigs = palAllocate(g_Wl.allocator, configSize, 0);
    if (!eglConfigs) {
        return PAL_FALSE;
    }

    g_VideoEgl.getConfigs(display, eglConfigs, numConfigs, &numConfigs);
    EGLConfig config = eglConfigs[fbConfigIndex - 1];
    palFree(g_Wl.allocator, eglConfigs);

    return (config != nullptr) ? PAL_TRUE : PAL_FALSE;
}


/* =========================================================================
 * API
 * ========================================================================= */

PalResult wlCreateWindow(
    const PalWindowCreateInfo* info, 
    PalWindow** window)
{
    PalResult result = PAL_RESULT_CODE_PLATFORM_FAILURE;
    struct wl_surface* surface = nullptr;
    struct xdg_surface* xdgSurface = nullptr;
    struct xdg_toplevel* xdgToplevel = nullptr;

    struct zxdg_toplevel_decoration_v1* decoration = nullptr;
    struct wl_buffer* buffer = nullptr;
    struct wl_egl_window* eglWindow = nullptr;

    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (!(info->style & PAL_WINDOW_STYLE_BORDERLESS)) {
        if (!g_Wl.decorationManager) {
            return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
        }
    }

    WindowData* data = wlGetFreeWindowData();
    if (!data) {
        return PAL_RESULT_CODE_OUT_OF_MEMORY;
    }

    memset(data, 0, sizeof(WindowData));
    data->used = PAL_TRUE;
    data->focused = PAL_FALSE;
    data->state = PAL_WINDOW_STATE_RESTORED;

    // create surface
    surface = wlCompositorCreateSurface(g_Wl.compositor);
    if (!surface) {
        goto cleanup;
    }

    wlSurfaceAddListener(surface, &g_SurfaceListener, data);
    xdgSurface = xdgWmBaseGetXdgSurface(g_Wl.xdgBase, surface);
    if (!xdgSurface) {
        goto cleanup;
    }

    xdgToplevel = xdgSurfaceGetToplevel(xdgSurface);
    if (!xdgToplevel) {
        goto cleanup;
    }

    // set APP id
    const char* appID = info->appName;
    if (!appID || strlen(appID) == 0) {
        appID = "PAL";
    }

    const char* title = info->title;
    if (!title) {
        title = "";
    }

    xdgToplevelSetTitle(xdgToplevel, title);
    xdgToplevelSetAppId(xdgToplevel, appID);

    xdgToplevelAddListener(xdgToplevel, &g_XdgToplevelListener, data);
    xdgSurfaceAddListener(xdgSurface, &g_XdgSurfaceListener, data);

    // decorated window
    if (!(info->style & PAL_WINDOW_STYLE_BORDERLESS)) {
        decoration = zxdgGetToplevelDecoration(g_Wl.decorationManager, xdgToplevel);
        if (decoration) {
            zxdgToplevelDecorationV1AddListener(decoration, &g_DecorationListener, surface);
            zxdgToplevelDecorationV1SetMode(decoration, 2);
            data->decoration = decoration;
        }
    }

    data->skipState = PAL_TRUE;
    data->skipConfigure = PAL_TRUE;
    wlSurfaceCommit(surface);
    g_Wl.displayRoundtrip(g_Wl.display);

    if (info->state == PAL_WINDOW_STATE_MAXIMIZED && info->show) {
        xdgToplevelSetMaximized(xdgToplevel);
        wlSurfaceCommit(surface);
        g_Wl.displayRoundtrip(g_Wl.display);
        data->state = PAL_WINDOW_STATE_MAXIMIZED;

    } else {
        data->w = info->width;
        data->h = info->height;
    }

    data->xdgSurface = xdgSurface;
    data->xdgToplevel = xdgToplevel;
    data->window = (PalWindow*)surface;
    data->buffer = nullptr;
    data->isAttached = PAL_FALSE;

    // minimize
    if (info->state == PAL_WINDOW_STATE_MINIMIZED) {
        xdgToplevelSetMinimized(xdgToplevel);
        wlSurfaceCommit(surface);
        data->state = PAL_WINDOW_STATE_MINIMIZED;
    }

    // user provided a config id
    if (info->fbConfigIndex) {
        PalFBConfigBackend backend = info->fbConfigBackend;
        if (backend == PAL_FBCONFIG_BACKEND_PAL_OPENGL) {
            backend = PAL_FBCONFIG_BACKEND_EGL;
        }

        if (backend == PAL_FBCONFIG_BACKEND_EGL) {
            if (!eglWlBackend(info->fbConfigIndex)) {
                result = PAL_RESULT_CODE_INVALID_ARGUMENT;
                goto cleanup;
            }
        } else {
            result = PAL_RESULT_CODE_INVALID_ARGUMENT;
            goto cleanup;
        }

        eglWindow = g_Wl.eglWindowCreate(surface, (int)data->w, (int)data->h);
        if (!eglWindow) {
            result = palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE,
                PAL_RESULT_SOURCE_EGL,
                (uint32_t)g_VideoEgl.getError());

            goto cleanup;
        }
        data->eglWindow = eglWindow;

    } else {
        // create a white buffer for the surface
        buffer = wlCreateShmBuffer((int)data->w, (int)data->h, nullptr, PAL_FALSE);
        if (!buffer) {
            result = palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_POSIX, 
                (uint32_t)errno);

            goto cleanup;
        }

        wlSurfaceAttach(surface, buffer, 0, 0);
        wlSurfaceDamageBuffer(surface, 0, 0, (int)data->w, (int)data->h);
        wlSurfaceCommit(surface);
        data->buffer = buffer;
    }

    struct wl_region* region = wlCompositorCreateRegion(g_Wl.compositor);
    if (region) {
        wlRegionAdd(region, 0, 0, (int)data->w, (int)data->h);
        wlSurfaceSetOpaqueRegion(surface, region);
        wlRegionDestroy(region);
        wlSurfaceCommit(surface);
    }

    g_Wl.displayRoundtrip(g_Wl.display);

#if PAL_HAS_EVENT_MODULE
    if (!g_Wl.decorationManager && g_Wl.eventDriver) {
        PalEventDriver* driver = g_Wl.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_MODE_NONE;
        PalEventType type = PAL_EVENT_TYPE_WINDOW_DECORATION_MODE;
        mode = palGetEventDispatchMode(driver, type);

        if (mode != PAL_DISPATCH_MODE_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = PAL_DECORATION_MODE_CLIENT_SIDE;
            event.data2 = palPackPointer(data->window);
            palPushEvent(driver, &event);
        }
    }
#endif // PAL_HAS_EVENT_MODULE

    data->skipState = PAL_FALSE;
    data->skipConfigure = PAL_FALSE;

    *window = data->window;
    return PAL_RESULT_SUCCESS;

cleanup:
    if (decoration) {
        zxdgToplevelDecorationV1Destroy(decoration);
    }

    if (eglWindow) {
        g_Wl.eglWindowDestroy(eglWindow);
    }

    if (buffer) {
        wlBufferDestroy(buffer);
    }

    if (xdgToplevel) {
        xdgToplevelDestroy(xdgToplevel);
    }

    if (xdgSurface) {
        xdgSurfaceDestroy(xdgSurface);
    }

    if (surface) {
        wlSurfaceDestroy(surface);
    }

    data->used = PAL_FALSE;
    return result;
}

void wlDestroyWindow(PalWindow* window)
{
    WindowData* data = wlFindWindowData(window);
    if (!data || data->isAttached) {
        return;
    }

    if (data->decoration) {
        zxdgToplevelDecorationV1Destroy(data->decoration);
    }

    if (data->eglWindow) {
        g_Wl.eglWindowDestroy(data->eglWindow);
    } else if (data->buffer) {
        wlBufferDestroy(data->buffer);
    }

    xdgToplevelDestroy(data->xdgToplevel);
    xdgSurfaceDestroy(data->xdgSurface);
    wlSurfaceDestroy((struct wl_surface*)window);
    
    g_Wl.displayFlush(g_Wl.display);
    data->used = PAL_FALSE;
}

void wlMaximizeWindow(PalWindow* window)
{
    WindowData* data = wlFindWindowData(window);
    xdgToplevelSetMaximized(data->xdgToplevel);
}

void wlMinimizeWindow(PalWindow* window)
{
    WindowData* data = wlFindWindowData(window);
    xdgToplevelSetMinimized(data->xdgToplevel);
}

void wlRestoreWindow(PalWindow* window)
{
    WindowData* data = wlFindWindowData(window);
    // we can only restore from a maximized state
    xdgToplevelUnsetMaximized(data->xdgToplevel);
}

void wlShowWindow(PalWindow* window)
{
    (void)window;
}

void wlHideWindow(PalWindow* window)
{
    (void)window;
}

void wlFlashWindow(
    PalWindow* window, 
    const PalFlashInfo* info)
{
    (void)window;
    (void)info;
}

PalResult wlAttachWindow(
    void* windowHandle,
    PalWindow** window)
{
    (void)windowHandle;
    (void)window;

    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlDetachWindow(
    PalWindow* window,
    void** windowHandle)
{
    (void)window;
    (void)windowHandle;

    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}
    
void wlGetWindowStyle(
    PalWindow* window, 
    PalWindowStyle* style)
{
    (void)window;
    (void)style;
}

void wlGetWindowMonitor(
    PalWindow* window, 
    PalMonitor** monitor)
{
    (void)window;
    (void)monitor;
}

void wlGetWindowTitle(
    PalWindow* window, 
    uint64_t bufferSize, 
    uint64_t* size, 
    char* buffer)
{
    (void)window;
    (void)bufferSize;
    (void)size;
    (void)buffer;
}

void wlGetWindowPos(
    PalWindow* window, 
    int32_t* x, 
    int32_t* y)
{
    (void)window;
    (void)x;
    (void)y;
}

void wlGetWindowSize(
    PalWindow* window, 
    uint32_t* width, 
    uint32_t* height)
{
    (void)window;
    (void)width;
    (void)height;
}

void wlGetWindowState(
    PalWindow* window, 
    PalWindowState* state)
{
    (void)window;
    (void)state;
}

PalBool wlIsWindowVisible(PalWindow* window)
{
    (void)window;
    
    return PAL_FALSE;
}

PalWindow* wlGetFocusWindow(void)
{
    return nullptr;
}

void wlGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    WindowData* data = wlFindWindowData(window);
    info->nativeInstance = (void*)g_Wl.display;
    info->nativeWindow = (void*)window;
    info->nativeHandle1 = data->xdgSurface;
    info->nativeHandle2 = data->xdgToplevel;
    info->nativeHandle3 = data->eglWindow;
}

void wlSetWindowOpacity(
    PalWindow* window, 
    float opacity)
{
    (void)window;
    (void)opacity;
}

void wlSetWindowStyle(
    PalWindow* window, 
    PalWindowStyle style)
{
    (void)window;
    (void)style;
}

void wlSetWindowTitle(
    PalWindow* window, 
    const char* title)
{
    WindowData* data = wlFindWindowData(window);
    xdgToplevelSetTitle(data->xdgToplevel, title);
    g_Wl.displayFlush(g_Wl.display);
}

void wlSetWindowPos(
    PalWindow* window, 
    int32_t x, 
    int32_t y)
{
    (void)window;
    (void)x;
    (void)y;
}

void wlSetWindowSize(
    PalWindow* window, 
    uint32_t width, 
    uint32_t height)
{
    WindowData* data = wlFindWindowData(window);
    xdgToplevelSetMinSize(data->xdgToplevel, (int)width, (int)height);
    xdgToplevelSetMaxSize(data->xdgToplevel, (int)width, (int)height);
    wlSurfaceCommit((struct wl_surface*)window);
}

void wlSetFocusWindow(PalWindow* window)
{
    (void)window;
}

#endif // PAL_HAS_WAYLAND_BACKEND