
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#ifdef __linux__
#if PAL_HAS_WAYLAND_BACKEND == 1

#include "pal_wayland.h"
#include "pal_wayland_protocols.h"
#include "pal_shared.h"
#include <stdlib.h>

EGLConfig eglWlBackend(const int fbConfigIndex)
{
    EGLDisplay display = EGL_NO_DISPLAY;
    display = s_Egl.eglGetDisplay((EGLNativeDisplayType)s_Wl.display);
    if (display == EGL_NO_DISPLAY) {
        return nullptr;
    }

    EGLint numConfigs = 0;
    if (!s_Egl.eglGetConfigs(display, nullptr, 0, &numConfigs)) {
        return nullptr;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Video.allocator, configSize, 0);
    if (!eglConfigs) {
        return nullptr;
    }

    s_Egl.eglGetConfigs(display, eglConfigs, numConfigs, &numConfigs);
    return eglConfigs[fbConfigIndex];
}

PalResult wlCreateWindow(
    const PalWindowCreateInfo* info,
    PalWindow** outWindow)
{
    struct wl_surface* surface = nullptr;
    struct xdg_surface* xdgSurface = nullptr;
    struct xdg_toplevel* xdgToplevel = nullptr;

    if (info->style & PAL_WINDOW_STYLE_TOPMOST) {
        return palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        return palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        return palMakeResult(
            PAL_RESULT_FEATURE_NOT_SUPPORTED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!(info->style & PAL_WINDOW_STYLE_BORDERLESS)) {
        if (!s_Wl.decorationManager) {
            // user wants decorated window but its not supported
            return palMakeResult(
                PAL_RESULT_FEATURE_NOT_SUPPORTED, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }
    }

    WindowData* data = getFreeWindowData();
    if (!data) {
        return palMakeResult(
            PAL_RESULT_OUT_OF_MEMORY, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    memset(data, 0, sizeof(WindowData));
    data->used = PAL_TRUE;
    data->focused = PAL_FALSE;

    // create surface
    surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!surface) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    wlSurfaceAddListener(surface, &s_SurfaceListener, data);
    xdgSurface = xdgWmBaseGetXdgSurface(s_Wl.xdgBase, surface);
    if (!xdgSurface) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    xdgToplevel = xdgSurfaceGetToplevel(xdgSurface);
    if (!xdgSurface) {
        return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // set APP id
    const char* appID = info->appName;
    if (!appID || strlen(appID) == 0) {
        appID = s_Video.className;
    }

    const char* title = info->title;
    if (!title) {
        title = "";
    }

    xdgToplevelSetTitle(xdgToplevel, title);
    xdgToplevelSetAppId(xdgToplevel, appID);

    xdgToplevelAddListener(xdgToplevel, &s_XdgToplevelListener, data);
    xdgSurfaceAddListener(xdgSurface, &s_XdgSurfaceListener, data);

    // decorated window
    if (!(info->style & PAL_WINDOW_STYLE_BORDERLESS)) {
        struct zxdg_toplevel_decoration_v1* decoration = nullptr;
        decoration = zxdgGetToplevelDecoration(s_Wl.decorationManager, xdgToplevel);
        zxdgToplevelDecorationV1AddListener(decoration, &s_DecorationListener, surface);
        zxdgToplevelDecorationV1SetMode(decoration, 2);
        data->decoration = decoration;
    }

    data->skipState = PAL_TRUE;
    data->skipConfigure = PAL_TRUE;
    wlSurfaceCommit(surface);
    s_Wl.displayRoundtrip(s_Wl.display);

    if (info->maximized && info->show) {
        // we need the maximized size the compositor will use
        // and use that to create the buffer
        xdgToplevelSetMaximized(xdgToplevel);
        wlSurfaceCommit(surface);
        s_Wl.displayRoundtrip(s_Wl.display);
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
    data->state = PAL_WINDOW_STATE_RESTORED;

    // minimize
    // This is just a requeest, the compositor might ignore it
    if (info->minimized) {
        xdgToplevelSetMinimized(xdgToplevel);
        wlSurfaceCommit(surface);
        data->state = PAL_WINDOW_STATE_MINIMIZED;
    }

    // user provided a config id
    if (info->fbConfigIndex) {
        PalFBConfigBackend backend = info->fbConfigBackend;
        EGLConfig config = nullptr;

        if (backend == PAL_CONFIG_BACKEND_PAL_OPENGL) {
            backend = PAL_CONFIG_BACKEND_EGL;
        }

        if (backend == PAL_CONFIG_BACKEND_EGL) {
            config = eglWlBackend(info->fbConfigIndex);

        } else {
            return palMakeResult(
                PAL_RESULT_INVALID_ARGUMENT, 
                PAL_RESULT_SOURCE_LINUX, 
                errno);
        }

        data->eglWindow = s_Wl.eglWindowCreate(surface, data->w, data->h);
        if (!data->eglWindow) {
            return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
        }

    } else {
        // create a white buffer for the surface
        struct wl_buffer* buffer = nullptr;
        buffer = createShmBuffer(data->w, data->h, nullptr, PAL_FALSE);
        if (!buffer) {
            return palMakeResult(
            PAL_RESULT_PLATFORM_FAILURE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
        }

        wlSurfaceAttach(surface, buffer, 0, 0);
        wlSurfaceDamageBuffer(surface, 0, 0, data->w, data->h);
        wlSurfaceCommit(surface);
        data->buffer = buffer;
    }

    struct wl_region* region = wlCompositorCreateRegion(s_Wl.compositor);
    if (region) {
        wlRegionAdd(region, 0, 0, data->w, data->h);
        wlSurfaceSetOpaqueRegion(surface, region);
        wlRegionDestroy(region);
        wlSurfaceCommit(surface);
    }

    s_Wl.displayRoundtrip(s_Wl.display);
    if (!s_Wl.decorationManager && s_Video.eventDriver) {
        PalEventDriver* driver = s_Video.eventDriver;
        PalDispatchMode mode = PAL_DISPATCH_NONE;
        PalEventType type = PAL_EVENT_WINDOW_DECORATION_MODE;
        mode = palGetEventDispatchMode(driver, type);

        if (mode != PAL_DISPATCH_NONE) {
            PalEvent event = {0};
            event.type = type;
            event.data = PAL_DECORATION_MODE_CLIENT_SIDE;
            event.data2 = palPackPointer(data->window);
            palPushEvent(driver, &event);
        }
    }

    // Since wayland does not have a way to set unique data
    // to a surface without taking control from users
    // we might implement a simple hash map to do that
    // but at the moment a linear search is fine
    // FIXME: Implement a window hash map

    data->skipState = PAL_FALSE;
    data->skipConfigure = PAL_FALSE;
    *outWindow = data->window;
    return PAL_RESULT_SUCCESS;
}

void wlDestroyWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data || (data && data->isAttached)) {
        return;
    }

    if (data->decoration) {
        zxdgToplevelDecorationV1Destroy(data->decoration);
    }

    if (data->eglWindow) {
        s_Wl.eglWindowDestroy(data->eglWindow);
    } else {
        wlBufferDestroy(data->buffer);
    }

    xdgToplevelDestroy(data->xdgToplevel);
    xdgSurfaceDestroy(data->xdgSurface);
    wlSurfaceDestroy((struct wl_surface*)window);
    data->used = PAL_FALSE;
}

PalResult wlMinimizeWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    xdgToplevelSetMinimized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlMaximizeWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    xdgToplevelSetMaximized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlRestoreWindow(PalWindow* window)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    // we can only restore from a maximized state
    xdgToplevelUnsetMaximized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlShowWindow(PalWindow* window)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlHideWindow(PalWindow* window)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalBool wlIsWindowVisible(PalWindow* window)
{
    return PAL_FALSE;
}

PalWindow* wlGetFocusWindow()
{
    // Wayland does not let client query focused window
    return nullptr;
}

PalResult wlGetWindowHandleInfo(
    PalWindow* window, 
    PalWindowHandleInfo* info)
{
    if (!s_Video.initialized) {
        return palMakeResult(
            PAL_RESULT_NOT_INITIALIZED, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    if (!window || !info) {
        return palMakeResult(
            PAL_RESULT_INVALID_ARGUMENT, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    WindowData* data = findWindowData(window);
    if (data) {
        info->nativeDisplay = (void*)s_Wl.display;
        info->nativeWindow = (void*)window;
        info->nativeHandle1 = data->xdgSurface;
        info->nativeHandle2 = data->xdgToplevel;
        info->nativeHandle3 = data->eglWindow;
    }

    return PAL_RESULT_SUCCESS;
}

PalResult wlSetWindowOpacity(
    PalWindow* window,
    float opacity)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    xdgToplevelSetTitle(data->xdgToplevel, title);
    s_Wl.displayFlush(s_Wl.display);
    return PAL_RESULT_SUCCESS;
}

PalResult wlSetWindowPos(
    PalWindow* window,
    int32_t x,
    int32_t y)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    WindowData* data = findWindowData(window);
    if (!data) {
        return palMakeResult(
            PAL_RESULT_INVALID_HANDLE, 
            PAL_RESULT_SOURCE_LINUX, 
            errno);
    }

    xdgToplevelSetMinSize(data->xdgToplevel, width, height);
    xdgToplevelSetMaxSize(data->xdgToplevel, width, height);
    wlSurfaceCommit((struct wl_surface*)window);
    return PAL_RESULT_SUCCESS;
}

PalResult wlSetFocusWindow(PalWindow* window)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

PalResult wlDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    return palMakeResult(
        PAL_RESULT_FEATURE_NOT_SUPPORTED, 
        PAL_RESULT_SOURCE_LINUX, 
        errno);
}

#endif // PAL_HAS_WAYLAND_BACKEND
#endif // __linux__