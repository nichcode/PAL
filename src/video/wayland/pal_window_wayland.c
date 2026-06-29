
/**
    PAL - Prime Abstraction Layer
    Copyright (C) 2025
    Licensed under the Zlib license. See LICENSE file in root.
 */

#if PAL_HAS_WAYLAND_BACKEND == 1
#include "pal_wayland.h"
#include "pal_wayland_protocols.h"
#include <stdlib.h>
#include <errno.h>

EGLConfig eglWlBackend(const int fbConfigIndex)
{
    EGLDisplay display = EGL_NO_DISPLAY;
    display = s_VideoEgl.getDisplay((EGLNativeDisplayType)s_Wl.display);
    if (display == EGL_NO_DISPLAY) {
        return nullptr;
    }

    EGLint numConfigs = 0;
    if (!s_VideoEgl.getConfigs(display, nullptr, 0, &numConfigs)) {
        return nullptr;
    }

    EGLint configSize = sizeof(EGLConfig) * numConfigs;
    EGLConfig* eglConfigs = palAllocate(s_Wl.allocator, configSize, 0);
    if (!eglConfigs) {
        return nullptr;
    }

    s_VideoEgl.getConfigs(display, eglConfigs, numConfigs, &numConfigs);
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
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (info->style & PAL_WINDOW_STYLE_TRANSPARENT) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (info->style & PAL_WINDOW_STYLE_TOOL) {
        return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
    }

    if (!(info->style & PAL_WINDOW_STYLE_BORDERLESS)) {
        if (!s_Wl.decorationManager) {
            // user wants decorated window but its not supported
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

    // create surface
    surface = wlCompositorCreateSurface(s_Wl.compositor);
    if (!surface) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    wlSurfaceAddListener(surface, &s_SurfaceListener, data);
    xdgSurface = xdgWmBaseGetXdgSurface(s_Wl.xdgBase, surface);
    if (!xdgSurface) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
    }

    xdgToplevel = xdgSurfaceGetToplevel(xdgSurface);
    if (!xdgSurface) {
        return PAL_RESULT_CODE_PLATFORM_FAILURE;
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

    if (info->state == PAL_WINDOW_STATE_MAXIMIZED && info->show) {
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
    if (info->state == PAL_WINDOW_STATE_MINIMIZED) {
        xdgToplevelSetMinimized(xdgToplevel);
        wlSurfaceCommit(surface);
        data->state = PAL_WINDOW_STATE_MINIMIZED;
    }

    // user provided a config id
    if (info->fbConfigIndex) {
        PalFBConfigBackend backend = info->fbConfigBackend;
        EGLConfig config = nullptr;

        if (backend == PAL_FBCONFIG_BACKEND_PAL_OPENGL) {
            backend = PAL_FBCONFIG_BACKEND_EGL;
        }

        if (backend == PAL_FBCONFIG_BACKEND_EGL) {
            config = eglWlBackend(info->fbConfigIndex);
        } else {
            return PAL_RESULT_CODE_INVALID_ARGUMENT;
        }

        data->eglWindow = s_Wl.eglWindowCreate(surface, data->w, data->h);
        if (!data->eglWindow) {
            return palMakeResult(
                PAL_RESULT_CODE_PLATFORM_FAILURE, 
                PAL_RESULT_SOURCE_EGL, 
                s_VideoEgl.getError());
        }

    } else {
        // create a white buffer for the surface
        struct wl_buffer* buffer = nullptr;
        buffer = createShmBuffer(data->w, data->h, nullptr, PAL_FALSE);
        if (!buffer) {
            return palMakeResult(PAL_RESULT_CODE_PLATFORM_FAILURE, PAL_RESULT_SOURCE_POSIX, errno);
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
    if (!s_Wl.decorationManager && s_Wl.eventDriver) {
        PalEventDriver* driver = s_Wl.eventDriver;
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

    // Since wayland does not have a way to set unique data
    // to a surface without taking control from users
    // we might implement a simple hash map to do that
    // but at the moment a linear search is fine
    data->skipState = PAL_FALSE;
    data->skipConfigure = PAL_FALSE;
    *outWindow = data->window;
    return PAL_RESULT_SUCCESS;
}

void wlDestroyWindow(PalWindow* window)
{
    WindowData* data = wlFindWindowData(window);
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
    WindowData* data = wlFindWindowData(window);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    xdgToplevelSetMinimized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlMaximizeWindow(PalWindow* window)
{
    WindowData* data = wlFindWindowData(window);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    xdgToplevelSetMaximized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlRestoreWindow(PalWindow* window)
{
    WindowData* data = wlFindWindowData(window);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    // we can only restore from a maximized state
    xdgToplevelUnsetMaximized(data->xdgToplevel);
    return PAL_RESULT_SUCCESS;
}

PalResult wlShowWindow(PalWindow* window)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlHideWindow(PalWindow* window)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlFlashWindow(
    PalWindow* window,
    const PalFlashInfo* info)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowStyle(
    PalWindow* window,
    PalWindowStyle* outStyle)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowMonitor(
    PalWindow* window,
    PalMonitor** outMonitor)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowTitle(
    PalWindow* window,
    uint64_t bufferSize,
    uint64_t* outSize,
    char* outBuffer)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowPos(
    PalWindow* window,
    int32_t* x,
    int32_t* y)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowSize(
    PalWindow* window,
    uint32_t* width,
    uint32_t* height)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlGetWindowState(
    PalWindow* window,
    PalWindowState* outState)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
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
    if (!window || !info) {
        return PAL_RESULT_CODE_INVALID_ARGUMENT;
    }

    WindowData* data = wlFindWindowData(window);
    if (data) {
        info->nativeInstance = (void*)s_Wl.display;
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
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetWindowStyle(
    PalWindow* window,
    PalWindowStyle style)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetWindowTitle(
    PalWindow* window,
    const char* title)
{
    WindowData* data = wlFindWindowData(window);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
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
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlSetWindowSize(
    PalWindow* window,
    uint32_t width,
    uint32_t height)
{
    WindowData* data = wlFindWindowData(window);
    if (!data) {
        return PAL_RESULT_CODE_INVALID_HANDLE;
    }

    xdgToplevelSetMinSize(data->xdgToplevel, width, height);
    xdgToplevelSetMaxSize(data->xdgToplevel, width, height);
    wlSurfaceCommit((struct wl_surface*)window);
    return PAL_RESULT_SUCCESS;
}

PalResult wlSetFocusWindow(PalWindow* window)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlAttachWindow(
    void* windowHandle,
    PalWindow** outWindow)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

PalResult wlDetachWindow(
    PalWindow* window,
    void** outWindowHandle)
{
    return PAL_RESULT_CODE_FEATURE_NOT_SUPPORTED;
}

#endif // PAL_HAS_WAYLAND_BACKEND